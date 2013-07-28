#include "message.h"
#include "SDP.h"
#include <iostream>
#include <stdlib.h>
using namespace std;

int main() {

	// Create a new message
	FSR_Message fsr = FSR_Message();
	XBeeAddress64 addr = XBeeAddress64();
	addr.setMsb(0x12345678);
	addr.setLsb(0x9abcdef0);

	cout << "msb: " << std::hex << (uint32_t) addr.getMsb() << std::endl;
	cout << "lsb: " << std::hex << (uint32_t) addr.getLsb() << std::endl;

	fsr.setAddress64(addr);

	fsr.setAddress16(0x1616);

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

	// Service Discovery Protocol
	SDP sdp = SDP();
	sdp.processMessage(v);

	delete message;

}
