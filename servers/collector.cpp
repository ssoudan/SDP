/*
 Service Discovery Protocol - data collector
 @author: Sebastien Soudan <sebastien.soudan@gmail.com>
*/

#include "message.h"
#include "SDP.h"

#include <iostream>
#include <stdlib.h>
#include <assert.h>
#include <fstream>
#include <boost/assign/list_of.hpp>
#include "stream.h"
using namespace std;

#include "Serial.h"

#define LOCAL_ADDR64_HIGH 0x0013a200
#define LOCAL_ADDR64_LOW  0x408b60d9

#define MAX_ROW 2000

#define HEADER "date\tCellar(Temp/C)\tCellar(Humidity/%)\tTerrace(Temp/C)\tCellar(State)\tRoom1(Temp/C)\tRoom1(Pressure/kPa)\tKitchen(Temp/C)\tKitchen(Light/%)\tStairs(Light/%)\tStairs(LED)\tStairs(Movement)\tWaterTank(Depth/cm)"

class Record {
public:
	uint32_t date;
	float cellar_Temperature;
	float cellar_Humidity;
	float terrace_Temperature;
	bool cellar_FanState;		
	float room_1_Temperature;
	float room_1_Pressure;
	float kitchen_Temperature;
	float kitchen_Light;
	float stairs_Light;
	bool stairs_LedState;
	bool stairs_Movement;
	uint16_t waterTank_Depth;

private:
	friend std::ostream& operator<<(std::ostream&, const Record&);
	
};

	const string currentDateTime(time_t date) {
	    time_t     now = date;
	    struct tm  tstruct;
	    char       buf[80];
	    tstruct = *localtime(&now);
	    strftime(buf, sizeof(buf), "%Y%m%d%H%M", &tstruct);

	    return buf;
	};

std::ostream& operator<<(std::ostream &strm, const Record &a) {
  return strm 	<< currentDateTime(a.date) << "\t"
				<< a.cellar_Temperature << "\t"
				<< a.cellar_Humidity << "\t"
				<< a.terrace_Temperature << "\t"
				<< a.cellar_FanState << "\t"
				<< a.room_1_Temperature << "\t"
				<< a.room_1_Pressure << "\t"
				<< a.kitchen_Temperature << "\t"
				<< a.kitchen_Light << "\t"
				<< a.stairs_Light << "\t"
				<< a.stairs_LedState << "\t"
				<< a.stairs_Movement << "\t"
				<< a.waterTank_Depth
				<< endl;
}

Record record;


class ClientSDP : public SDP {
private:

	static std::map<std::string, ServiceDiscovery::ServiceLocation> serviceLocationString2EnumMap;
	static std::map<ServiceDiscovery::ServiceLocation, std::string> serviceLocationEnum2StringMap;

	static std::map<std::string, ServiceDiscovery::ServiceType> serviceTypeString2EnumMap;
	static std::map<ServiceDiscovery::ServiceType, std::string> serviceTypeEnum2StringMap;

	static std::map<std::string, ServiceDiscovery::ActionType> actionTypeString2EnumMap;
	static std::map<ServiceDiscovery::ActionType, std::string> actionTypeEnum2StringMap;


	boost::asio::io_service io;
	boost::asio::serial_port port;
	Serial serial;
public:

	ClientSDP(string device, XBeeAddress64 local64) : io(), port(io, device), serial(&port, 1500) {	

		port.set_option(boost::asio::serial_port_base::baud_rate(9600));
		// Set hardware flow control (RTS/CTS)	
		// port.set_option(boost::asio::serial_port::flow_control(boost::asio::serial_port::flow_control::hardware));

		setXBee(new XBee(serial));
		setLocal64(local64);

	};

	/**
		Test DoAction Encoding/Decoding
	*/
	void doAction(string service, string location, string action,  void (*method)(uint8_t *buffer, uint8_t size)) {

#if defined(DEBUG)
		cerr << "[..] doAction(" << service << ", " << location << ", " << action << ")" << std::endl;
#endif

		ActionStatus as = UNDEF_STATUS;
		SDPState state = UNKNOWN_STATE;

		int counter = 20;
		while (counter-- > 0) {
			as = SDP::doAction(serviceTypeString2EnumMap[service], serviceLocationString2EnumMap[location], actionTypeString2EnumMap[action], 0, NULL, method);

#if defined(DEBUG)
			if (as == REQUESTED)
				cerr << "Action " << action << " requested" << endl;
#endif
			state = readPackets();

			if (state == DAR_RECEIVED) {
#if defined(DEBUG)
				cerr << "state:  DAR_RECEIVED" << endl;
#endif
				return;
			} 
#if defined(DEBUG)
			else 
				cerr << "state: " << (unsigned int) state << endl;			
#endif
		}

	}

};


std::map<std::string, ServiceDiscovery::ServiceLocation> ClientSDP::serviceLocationString2EnumMap = boost::assign::map_list_of("UNDEF_LOCATION", UNDEF_LOCATION)("TERRACE", TERRACE)("CELLAR", CELLAR)("ROOM_1", ROOM_1)("KITCHEN", KITCHEN)("STAIRS", STAIRS)("WATERTANK", WATERTANK);
std::map<ServiceDiscovery::ServiceLocation, std::string> ClientSDP::serviceLocationEnum2StringMap = boost::assign::map_list_of(UNDEF_LOCATION, "UNDEF_LOCATION")(TERRACE, "TERRACE")(CELLAR, "CELLAR")(ROOM_1, "ROOM_1")(KITCHEN, "KITCHEN")(STAIRS, "STAIRS")(WATERTANK, "WATERTANK");

std::map<std::string, ServiceDiscovery::ServiceType> ClientSDP::serviceTypeString2EnumMap = boost::assign::map_list_of("UNDEF_SERVICE", UNDEF_SERVICE)("SDP_RTC", SDP_RTC)("TEMPERATURE",TEMPERATURE)("PRESSURE", PRESSURE)("FAN", FAN)("HUMIDITY", HUMIDITY)("LIGHT", LIGHT)("MOVEMENT", MOVEMENT)("LED", LED)("DEPTH", DEPTH);
std::map<ServiceDiscovery::ServiceType, std::string> ClientSDP::serviceTypeEnum2StringMap = boost::assign::map_list_of(UNDEF_SERVICE, "UNDEF_SERVICE")(SDP_RTC, "SDP_RTC")(TEMPERATURE,"TEMPERATURE")(PRESSURE, "PRESSURE")(FAN, "FAN")(HUMIDITY, "HUMIDITY")(LIGHT, "LIGHT")(MOVEMENT, "MOVEMENT")(LED, "LED")(DEPTH, "DEPTH");

std::map<std::string, ServiceDiscovery::ActionType> ClientSDP::actionTypeString2EnumMap = boost::assign::map_list_of("UNDEF_ACTION", UNDEF_ACTION) ("GET_VALUE", GET_VALUE) ("SET_VALUE", SET_VALUE)  ("GET_STATE", GET_STATE) ("START", START) ("STOP", STOP) ("AUTO", AUTO);
std::map<ServiceDiscovery::ActionType, std::string> ClientSDP::actionTypeEnum2StringMap = boost::assign::map_list_of(UNDEF_ACTION, "UNDEF_ACTION") (GET_VALUE, "GET_VALUE") (SET_VALUE, "SET_VALUE")  (GET_STATE, "GET_STATE") (START, "START") (STOP, "STOP") (AUTO, "AUTO");

void humidity_callback(uint8_t *buffer, uint8_t size) {
	float humidity;

	memcpy(&humidity, buffer, 4); 

	record.cellar_Humidity = humidity;

#if defined(DEBUG)
	cerr << "humidity (%): " << humidity << endl;
#endif
}

void cellar_temperature_callback(uint8_t *buffer, uint8_t size) {
	float temp;

	memcpy(&temp, buffer, 4); 

	record.cellar_Temperature = temp;

#if defined(DEBUG)
	cerr << "temp (C): " << temp << endl;
#endif
}

void kitchen_temperature_callback(uint8_t *buffer, uint8_t size) {
	float temp;

	memcpy(&temp, buffer, 4); 

	record.kitchen_Temperature = temp;

#if defined(DEBUG)
	cerr << "temp (C): " << temp << endl;
#endif
}

void kitchen_light_callback(uint8_t *buffer, uint8_t size) {
	float temp;

	memcpy(&temp, buffer, 4); 

	record.kitchen_Light = temp;

#if defined(DEBUG)
	cerr << "light (%): " << temp << endl;
#endif
}

void stairs_light_callback(uint8_t *buffer, uint8_t size) {
	float temp;

	memcpy(&temp, buffer, 4); 

	record.stairs_Light = temp;

#if defined(DEBUG)
	cerr << "light (%): " << temp << endl;
#endif
}

void terrace_temperature_callback(uint8_t *buffer, uint8_t size) {
	float temp;

	memcpy(&temp, buffer, 4); 

	record.terrace_Temperature = temp;

#if defined(DEBUG)
	cerr << "temp (C): " << temp << endl;
#endif
}

void room1_temperature_callback(uint8_t *buffer, uint8_t size) {
	float temp;

	memcpy(&temp, buffer, 4); 

	record.room_1_Temperature = temp;

#if defined(DEBUG)
	cerr << "temp (C): " << temp << endl;
#endif
}


void room1_pressure_callback(uint8_t *buffer, uint8_t size) {
	float pressure;

	memcpy(&pressure, buffer, 4); 

	record.room_1_Pressure = pressure;

#if defined(DEBUG)
	cerr << "pressure (kPa): " << pressure << endl;
#endif
}


void fan_state_callback(uint8_t *buffer, uint8_t size) {
	bool state;

	memcpy(&state, buffer, 1); 

	record.cellar_FanState = state;

#if defined(DEBUG)
	cerr << "Fan state: " << state << endl;
#endif
}


void stairs_movement_callback(uint8_t *buffer, uint8_t size) {
	bool state;

	memcpy(&state, buffer, 1); 

	record.stairs_Movement = state;

#if defined(DEBUG)
	cerr << "Movement: " << state << endl;
#endif
}

void stairs_led_callback(uint8_t *buffer, uint8_t size) {
	bool state;

	memcpy(&state, buffer, 1); 

	record.stairs_LedState = state;

#if defined(DEBUG)
	cerr << "LED state: " << state << endl;
#endif
}

void rtc_callback(uint8_t *buffer, uint8_t size) {
  uint32_t date = convertArraytoUint32(buffer);

  record.date = date;

#if defined(DEBUG)
  cerr << "epoc: " << (unsigned int) date << endl;
#endif
}

void waterTank_depth_callback(uint8_t *buffer, uint8_t size) {
  uint16_t depth = convertArraytoUint16(buffer);

  record.waterTank_Depth = depth;

#if defined(DEBUG)
  cerr << "depth: " << depth << endl;
#endif
}


int main(int argc, char ** argv) {	 

	string device = "/dev/tty.usbserial-AD025F4I";

	if (argc > 1)
		device = argv[1];
 
	// TODO get the local address from command line too

 	try {
		ClientSDP sdp(device,  XBeeAddress64(LOCAL_ADDR64_HIGH, LOCAL_ADDR64_LOW));
	 

		uint32_t date;
		float cellar_Temperature;
		float cellar_Humidity;
		float terrace_Temperature;
		bool cellar_FanState;		
		float room_1_Temperature;
		float room_1_Pressure;

		std::fstream outputFile;
		int count = 0;
		outputFile.open ("/var/www/last-data.tsv", std::fstream::out | std::fstream::trunc); 

		outputFile << HEADER << endl;
		
		cout << HEADER << endl;

		while (true) {

			sdp.doAction("TEMPERATURE", "CELLAR", "GET_VALUE", &cellar_temperature_callback);

			sdp.doAction("HUMIDITY", "CELLAR", "GET_VALUE", &humidity_callback);

			sdp.doAction("FAN", "CELLAR", "GET_STATE", &fan_state_callback);

			sdp.doAction("TEMPERATURE", "TERRACE", "GET_VALUE", &terrace_temperature_callback);

			sdp.doAction("TEMPERATURE", "ROOM_1", "GET_VALUE", &room1_temperature_callback);

			sdp.doAction("PRESSURE", "ROOM_1", "GET_VALUE", &room1_pressure_callback);

			sdp.doAction("SDP_RTC", "UNDEF_LOCATION", "GET_VALUE", &rtc_callback);

			sdp.doAction("TEMPERATURE", "KITCHEN", "GET_VALUE", &kitchen_temperature_callback);

			sdp.doAction("LIGHT", "KITCHEN", "GET_VALUE", &kitchen_light_callback);

			sdp.doAction("LIGHT", "STAIRS", "GET_VALUE", &stairs_light_callback);

			sdp.doAction("LED", "STAIRS", "GET_STATE", &stairs_led_callback);

			sdp.doAction("MOVEMENT", "STAIRS", "GET_VALUE", &stairs_movement_callback);

			sdp.doAction("DEPTH", "WATERTANK", "GET_VALUE", &waterTank_depth_callback);
			
			cout << record;
			outputFile << record;

			count ++;

			if (count > MAX_ROW) {
				count = 0;

				// close
				outputFile.close();

				// open/truncate
				outputFile.open ("/var/www/last-data.tsv", std::fstream::out | std::fstream::trunc); 

				// add header
				outputFile << HEADER << endl;

			}

			sleep(6);
		}
	} catch (exception& e) { 
        cerr << "Exception: " << e.what() << endl; 
        cerr << "Check " << device << " is the tty to your XBee." << endl;
        return -1;
    }

}

