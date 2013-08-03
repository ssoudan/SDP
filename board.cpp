// /*
//  Service Discovery Protocol - Tests
//  @author: Sebastien Soudan <sebastien.soudan@gmail.com>
// */

// #include "message.h"
// #include "SDP.h"
// #include <stdlib.h>
// #include <assert.h>
// using namespace std;


// /**
// 	Test FS Encoding/Decoding
// */
// void testFS() {

	

// 	// Create a new message
// 	FS_Message fs = FS_Message();
	
// 	fs.setServiceType(RTC);

// 	// Encode a message 
// 	uint8_t buffer[20];

// 	size_t size = fs.Encode(buffer, sizeof(buffer));

// 	// Attempt to decode a message from a buffer
// 	Message *message = Message::Decode(buffer, size);

// 	FS_Message* v = (FS_Message*) (message);

// 	assert (v->getServiceType() == RTC);

// 	// Service Discovery Protocol
// 	SDP sdp = SDP();
// 	sdp.processMessage(v);

// 	delete message;

	
// }


// /**
// 	Test FSR Encoding/Decoding
// */
// void testFSR() {

	

// 	// Create a new message
// 	FSR_Message fsr = FSR_Message();
// 	XBeeAddress64 addr = XBeeAddress64();
// 	addr.setMsb(0x12345678);
// 	addr.setLsb(0x9abcdef0);

	
	

// 	fsr.setAddress64(addr);

// 	fsr.setAddress16(0x1616);

	

// 	// Encode a message 
// 	uint8_t buffer[20];

// 	size_t size = fsr.Encode(buffer, sizeof(buffer));

// 	// Attempt to decode a message from a buffer
// 	Message *message = Message::Decode(buffer, size);

// 	FSR_Message* v = (FSR_Message*) (message);

// 	assert (v->getServiceType() == UNDEF_SERVICE);
// 	assert (v->getAddress64().getMsb() == addr.getMsb());
// 	assert (v->getAddress64().getLsb() == addr.getLsb());
// 	assert (v->getAddress16() == fsr.getAddress16());

// 	// Service Discovery Protocol
// 	SDP sdp = SDP();
// 	sdp.processMessage(v);

// 	delete message;

	
// }


// /**
// 	Test DA Encoding/Decoding
// */
// void testDA() {

	

// 	// Create a new message
// 	DA_Message da = DA_Message();
	
// 	da.setServiceType(RTC);
// 	da.setActionType(GET_VALUE);
// 	da.setActionParameterSize(3);
// 	assert(da.getActionParameterSize() == 3);

// 	uint8_t array[3] = {4, 3, 2}; 
// 	da.setActionParameter(array);

// 	// Encode a message 
// 	uint8_t buffer[20];

// 	size_t size = da.Encode(buffer, sizeof(buffer));

// 	// Attempt to decode a message from a buffer
// 	Message *message = Message::Decode(buffer, size);

// 	DA_Message* v = (DA_Message*) message;

// 	assert (v->getServiceType() == RTC);
// 	assert (v->getActionType() == GET_VALUE);
// 	assert (v->getActionParameterSize() == 3);
// 	// TODO add comparaison of the parameter

// 	// Service Discovery Protocol
// 	SDP sdp = SDP();
// 	sdp.processMessage(v);

// 	delete message;

	
// }

// /**
// 	Test DAR Encoding/Decoding
// */
// void testDAR() {

	

// 	// Create a new message
// 	DAR_Message dar = DAR_Message();
	
// 	dar.setServiceType(RTC);
// 	dar.setActionType(GET_VALUE);
// 	dar.setActionStatus(DONE);
// 	dar.setActionResultSize(3);
// 	assert(dar.getActionResultSize() == 3);

// 	uint8_t array[3] = {4, 3, 2}; 
// 	dar.setActionResult(array);

	

// 	// Encode a message 
// 	uint8_t buffer[20];

// 	size_t size = dar.Encode(buffer, sizeof(buffer));

// 	// Attempt to decode a message from a buffer
// 	Message *message = Message::Decode(buffer, size);

// 	DAR_Message* v = (DAR_Message*) (message);

// 	assert (v->getServiceType() == RTC);
// 	assert (v->getActionType() == GET_VALUE);
// 	assert (v->getActionStatus() == DONE);
// 	assert (v->getActionResultSize() == 3);
// 	// TODO add comparaison of the parameter

// 	// Service Discovery Protocol
// 	SDP sdp = SDP();
// 	sdp.processMessage(v);

// 	delete message;
// }



// int main() {

// 	testFS();

	

// 	testFSR();

	

// 	testDA();

	

// 	testDAR();

// }

