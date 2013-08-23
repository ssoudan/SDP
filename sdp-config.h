/*
 Service Discovery Protocol - config file
 @author: Sebastien Soudan <sebastien.soudan@gmail.com>
*/

#ifndef SDP_CONFIG_H_
#define SDP_CONFIG_H_




/**
	Number of services to keep in the Remote Service Directory
	- those that we use on remote endpoints.
 */
#define RSD_SIZE 10

/**
	Number of callback to keep in the Service Callback Directory
	- the methods that will be called on DAR message reception.
 */	
#define SCD_SIZE 10

/**
	Number of services to keep in the Local Service Directory 
	- those that can be used by others.
 */	
#define LSD_SIZE 10

/**
	Size of the buffer to be passed to service methods.
 */	
#define ACTION_BUFFER_SIZE 10

/*
	Debug statement - disable them to save memory (.data).
 */
#if defined(DEBUG)
	#if defined(LOCAL)
		#define INFO(x) do { cerr << x << endl; } while (0)
		#define MOREINFO(x, y) do { cerr << x << y << endl; } while (0)
		#define ERROR(x) do { cerr << x << endl; } while (0)
	#else 
		#define INFO(x) Serial.println(x)	
		#define MOREINFO(x, y) do {Serial.print(x); Serial.println(y); } while (0)
		#define ERROR(x) Serial.println(x)
	#endif 

#else
	#define INFO(x) 
	#define MOREINFO(x, y) 
	#define ERROR(x) 
#endif

/*
	LOCAL  - build for local machine (mac, whatever...)
	*else* - build for the Arduino
 */
#if defined(LOCAL)	

	#include <stdint.h>
	#include <cstring>
	#include <assert.h>
	#include <iostream>

	#include "stream.h"

	using namespace std;

	#define F(x) x
	#define ASSERT(x) assert(x)

	#include "XBee.h"
	// /*

	// 	/!\ Those redefinition of XBee classes are just for tests on local machine.

	//  */

	// #define ZB_TX_STATUS_RESPONSE 1
	// #define SUCCESS 1
	// #define ZB_RX_RESPONSE 2	
	// #define MODEM_STATUS_RESPONSE 3
	// #define ASSOCIATED 4
	// #define DISASSOCIATED 5	

	// typedef uint16_t XBeeAddress16;

	// class XBeeAddress64 {
	// private:
	// 	uint64_t addr;
	// public:
	// 	XBeeAddress64() {
	// 		addr = 0;
	// 	};

	// 	XBeeAddress64(long int msb, long int lsb) {
	// 		addr = 0;
	// 		setMsb(msb);
	// 		setLsb(lsb);
	// 	};

	// 	inline uint32_t getMsb() const {
	// 		return (uint32_t) (addr >> 32);
	// 	};

	// inline uint32_t getLsb() const {
	// 		return (uint32_t) (addr & 0xffffffff);
	// 	};

	// 	inline void setMsb(const uint32_t msb) {
	// 		addr = (addr & 0xffffffff) + ((uint64_t) msb << 32);
	// 	};

	// 	inline void setLsb(const uint32_t lsb) {
	// 		addr = (addr & 0xffffffff00000000ll) + lsb;
	// 	};

	// };

	// class ModemStatusResponse
	// {
	// public:
	// 	ModemStatusResponse() {};
	// 	~ModemStatusResponse() {};
	// 	uint8_t getStatus() { return ASSOCIATED; };
	// };

	// class ZBTxRequest {
	// private:
	// public:
	//  	ZBTxRequest(XBeeAddress64 addr64, uint8_t *message, size_t length) {} ;
	// };

	// class ZBTxStatusResponse {
	// private:
	// 	uint8_t status;
	// public:
	// 	inline void setDeliveryStatus(uint8_t status) { this->status = status; };
	// 	inline uint8_t getDeliveryStatus() { return status; };
	// };

	// class ZBRxResponse
	// {
	// private:	
	// 	XBeeAddress64 addr64;
	// public:
	// 	ZBRxResponse() {
	// 		addr64 = XBeeAddress64();
	// 	};
	// 	~ZBRxResponse() {};

	// 	/* data */
	// 	//inline void setDataLength(uint8_t len) {}; 
	// 	//inline void setRemoteAddress64(XBeeAddress64 addr64) {};
	// 	//inline void setData(uint8_t *data) {};
	// 	inline uint8_t getDataLength() { return 0; }; 
	// 	inline XBeeAddress64& getRemoteAddress64() { return addr64; };
	// 	inline uint8_t* getData() { return NULL; };
	// };

	// class Response {
	// public:
	// 	Response() {};
	// 	inline uint8_t getApiId() { return ZB_TX_STATUS_RESPONSE; };
	// 	inline uint8_t getZBTxStatusResponse(ZBTxStatusResponse &response) { response.setDeliveryStatus(SUCCESS); };	
	// 	inline uint8_t isError() { return 0; };
	// 	inline uint8_t isAvailable() { return 1; };
	// 	inline ModemStatusResponse getModemStatusResponse(ModemStatusResponse& modem) {
	// 		//modem.setStatus(ZZZ);
	// 	};
	// 	inline uint8_t getZBRxResponse(ZBRxResponse &response) { 
	// 		//response.setDataLength(XX); 
	// 		//response.setRemoteAddress64(XX);
	// 		//response.setData(XX);
	// 	};

	// };

	// class XBee {
	// private:
	// 	Response response;
	// public:
	// 	XBee() { response = Response(); };

	// 	inline void send(ZBTxRequest zbTx) { cerr << "XBee::send" << endl; };
	// 	inline uint8_t readPacket(uint16_t size) { return 1; };
	// 	inline Response getResponse() {return response; };
	// 	inline void readPacket() { cerr << "XBee::readPacket" << endl; };
	// 	inline void getZBRxResponse(ZBRxResponse &response) { 
	// 		// response.setDataLength(XXX); 
	// 		// response.setRemoteAddress64(XXX);
	// 		// response.setData(XXX);
	// 	};

	// };

#else 	
	/*
		When building for the board, just import XBee.h and ignore the assert
	 */

//	#include "Arduino.h"
//	#include <XBee.h>
	#define ASSERT(x)

#endif



/***************************************
And because there is no limit:

TODO:
 - detect XBee local address
 - add test with registration and action request
 - change the transmitPacket methods, it should not absord message this way, nor delay the execution
 - introduce timeout in case the service provider died
 - extend the library of service/actions 
 - add room/location to the service definition: 
 	"I want the [value] of the [temperature] of [my room]." 
 	"I want to [switch on] the [light] of the [corridor]"
***************************************/



#endif
