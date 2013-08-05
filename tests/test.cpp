/*
 Service Discovery Protocol - Tests
 @author: Sebastien Soudan <sebastien.soudan@gmail.com>
*/

#include "message.h"
#include "SDP.h"
#include <iostream>
#include <stdlib.h>
#include <assert.h>
using namespace std;


/**
	Test FS Encoding/Decoding
*/
void testFS() {

	cout << "[..] Tests of decode/encode of FS" << std::endl;

	XBee xbee = XBee();

	// Create a new message
	FS_Message fs = FS_Message();
	
	fs.setServiceType(ServiceDiscovery::SDP_RTC);

	cout << fs.getServiceType() << std::endl;

	// Encode a message 
	uint8_t buffer[20];

	size_t size = fs.Encode(buffer, sizeof(buffer));

	cout << "size: " << std::dec << (int) size << std::endl;	

	for (int i = 0 ; i < size ; i++) {
		cout << i << " " << hex <<  (int) buffer[i] << std::endl;
	}

	// Attempt to decode a message from a buffer
	Message *message = Message::Decode(buffer, size);

	const FS_Message* v = dynamic_cast<FS_Message*>(message);

	if (v != NULL) {
		cout << "decoded (service type): " << hex << (uint8_t) v->getServiceType() << endl;		
	}

	assert (v->getServiceType() == ServiceDiscovery::SDP_RTC);

	// Service Discovery Protocol
	SDP sdp = SDP(&xbee);
	
	XBeeAddress64 addr64 = XBeeAddress64();

	sdp.processMessage(addr64, v);

	delete message;

	cout << "[OK] Tests of decode/encode of FS" << std::endl;
}


/**
	Test FSR Encoding/Decoding
*/
void testFSR() {

	cout << "[..] Tests of decode/encode of FSR" << std::endl;

	XBee xbee = XBee();

	// Create a new message
	FSR_Message fsr = FSR_Message();
	XBeeAddress64 addr = XBeeAddress64();
	addr.setMsb(0x12345678);
	addr.setLsb(0x9abcdef0);

	cout << "msb: " << std::hex << (uint32_t) addr.getMsb() << std::endl;
	cout << "lsb: " << std::hex << (uint32_t) addr.getLsb() << std::endl;

	fsr.setAddress64(addr);

	// fsr.setAddress16(0x1616);

	cout << fsr.getServiceType() << std::endl;

	// Encode a message 
	uint8_t buffer[20];

	size_t size = fsr.Encode(buffer, sizeof(buffer));

	cout << "size: " << std::dec << (int) size << std::endl;	

	for (int i = 0 ; i < size ; i++) {
		cout << i << " " << hex <<  (int) buffer[i] << std::endl;
	}

	// Attempt to decode a message from a buffer
	Message *message = Message::Decode(buffer, size);

	FSR_Message* v = dynamic_cast<FSR_Message*>(message);

	if (v != NULL) {
		cout << "decoded (msb addr): " << hex << (uint32_t) v->getAddress64().getMsb() << endl;
		cout << "decoded (lsb addr): " << hex << (uint32_t) v->getAddress64().getLsb() << endl;
	}

	assert (v->getServiceType() == UNDEF_SERVICE);
	assert (v->getAddress64().getMsb() == addr.getMsb());
	assert (v->getAddress64().getLsb() == addr.getLsb());
	// assert (v->getAddress16() == fsr.getAddress16());

	// Service Discovery Protocol
	SDP sdp = SDP(&xbee);
	
	XBeeAddress64 addr64 = XBeeAddress64();

	sdp.processMessage(addr64, v);

	delete message;

	cout << "[OK] Tests of decode/encode of FSR" << std::endl;
}


/**
	Test DA Encoding/Decoding
*/
void testDA() {

	cout << "[..] Tests of decode/encode of DA" << std::endl;

	XBee xbee = XBee();

	// Create a new message
	DA_Message da = DA_Message();
	
	da.setServiceType(ServiceDiscovery::SDP_RTC);
	da.setActionType(GET_VALUE);
	da.setActionParameterSize(3);
	assert(da.getActionParameterSize() == 3);

	uint8_t array[3] = {4, 3, 2}; 
	da.setActionParameter(array);

	cout << da.getServiceType() << std::endl;

	// Encode a message 
	uint8_t buffer[20];

	size_t size = da.Encode(buffer, sizeof(buffer));

	cout << "size: " << std::dec << (int) size << std::endl;	

	for (int i = 0 ; i < size ; i++) {
		cout << i << " " << hex <<  (int) buffer[i] << std::endl;
	}

	// Attempt to decode a message from a buffer
	Message *message = Message::Decode(buffer, size);

	DA_Message* v = dynamic_cast<DA_Message*>(message);

	if (v != NULL) {
		cout << "decoded (service type): " << hex << (int) v->getServiceType() << endl;		
		cout << "decoded (action type): " << hex << (int) v->getActionType() << endl;		
		cout << "decoded (action parameter size): " << hex << (int) v->getActionParameterSize() << endl;		
	}

	assert (v->getServiceType() == ServiceDiscovery::SDP_RTC);
	assert (v->getActionType() == ServiceDiscovery::GET_VALUE);
	assert (v->getActionParameterSize() == 3);
	// TODO add comparaison of the parameter

	// Service Discovery Protocol
	SDP sdp = SDP(&xbee);

	XBeeAddress64 addr64 = XBeeAddress64();

	sdp.processMessage(addr64, v);

	delete message;

	cout << "[OK] Tests of decode/encode of DA" << std::endl;
}

/**
	Test DAR Encoding/Decoding
*/
void testDAR() {

	cout << "[..] Tests of decode/encode of DAR" << std::endl;

	XBee xbee = XBee();

	// Create a new message
	DAR_Message dar = DAR_Message();
	
	dar.setServiceType(ServiceDiscovery::SDP_RTC);
	dar.setActionType(ServiceDiscovery::GET_VALUE);
	dar.setActionStatus(ServiceDiscovery::DONE);
	dar.setActionResultSize(3);
	assert(dar.getActionResultSize() == 3);

	uint8_t array[3] = {4, 3, 2}; 
	dar.setActionResult(array);

	cout << dar.getServiceType() << std::endl;

	// Encode a message 
	uint8_t buffer[20];

	size_t size = dar.Encode(buffer, sizeof(buffer));

	cout << "size: " << std::dec << (int) size << std::endl;	

	for (int i = 0 ; i < size ; i++) {
		cout << i << " " << hex <<  (int) buffer[i] << std::endl;
	}

	// Attempt to decode a message from a buffer
	Message *message = Message::Decode(buffer, size);

	DAR_Message* v = dynamic_cast<DAR_Message*>(message);

	if (v != NULL) {
		cout << "decoded (service type): " << hex << (int) v->getServiceType() << endl;		
		cout << "decoded (action type): " << hex << (int) v->getActionType() << endl;		
		cout << "decoded (action status): " << hex << (int) v->getActionStatus() << endl;		
		cout << "decoded (action result size): " << hex << (int) v->getActionResultSize() << endl;		
	}

	assert (v->getServiceType() == ServiceDiscovery::SDP_RTC);
	assert (v->getActionType() == ServiceDiscovery::GET_VALUE);
	assert (v->getActionStatus() == ServiceDiscovery::DONE);
	assert (v->getActionResultSize() == 3);
	// TODO add comparaison of the parameter

	// Service Discovery Protocol
	SDP sdp = SDP(&xbee);

	XBeeAddress64 addr64 = XBeeAddress64();

	sdp.processMessage(addr64, v);

	delete message;

	cout << "[OK] Tests of decode/encode of DAR" << std::endl;
}


void testRegistration() {

	//RegistrationStatus res = sdp.registerService(ServiceDiscovery::SDP_RTC, ServiceDiscovery::GET_VALUE, &getDate);

}


int main() {	 

	testFS();

	cout << std::endl << std::endl;

	testFSR();

	cout << std::endl << std::endl;

	testDA();

	cout << std::endl << std::endl;

	testDAR();

	cout << std::endl << std::endl;

	testRegistration();

}

