#include "message.h"
#include <assert.h> 
#include "util.h"


FS_Message *FS_Message::Decode(uint8_t *buffer, size_t size) {
	// TODO
	return NULL;
};

FSR_Message *FSR_Message::Decode(uint8_t *buffer, size_t size) {
	assert (buffer != NULL);

	assert (size >= 1 + 4 + 4 + 2);

	FSR_Message *fsr = new FSR_Message();

	ServiceType serviceType = (ServiceType) buffer[0];
	fsr->setServiceType(serviceType);
 
 	XBeeAddress64 addr64 = XBeeAddress64();
 	addr64.setMsb(convertArraytoUint32(&buffer[1]));
 	addr64.setLsb(convertArraytoUint32(&buffer[1 + 4]));
    fsr->setAddress64(addr64);

    XBeeAddress16 addr16 = convertArraytoUint16(&buffer[1 + 4 + 4]);
    
    fsr->setAddress16(addr16);

	return fsr;
};

DA_Message *DA_Message::Decode(uint8_t *buffer, size_t size) {
	// TODO
	return NULL;
};

DAR_Message *DAR_Message::Decode(uint8_t *buffer, size_t size) {
	// TODO
	return NULL;
};

Message *Message::Decode(uint8_t *buffer, size_t size) {
    if (size < 1) return 0; /* or throw some exception */
	switch(buffer[0]) {
		case FS: return FS_Message::Decode(buffer, size);
		case FSR: return FSR_Message::Decode(buffer, size);
		case DA: return DA_Message::Decode(buffer, size);
		case DAR: return DAR_Message::Decode(buffer, size);
    default: return 0; // or throw an exception
	}
};

size_t FS_Message::Encode(uint8_t *buffer, size_t limit) {
	assert (buffer != NULL);
		
	if (limit < 1)
		// Buffer too small;
		return 0;
	
	buffer[0] = FS;

	return 1;
};

size_t FSR_Message::Encode(uint8_t *buffer, size_t limit) {
	assert (buffer != NULL);
		
	if (limit < 1 + 4 + 4 + 2) {
		// Buffer too small;
		ERROR("Buffer too small.");	
		return 0;
	}
	
	// Message type: 8 bits
	// addr64: 64 bits
	// addr16: 16 bits

	buffer[0] = FSR;
	
	XBeeAddress64 addr64 = FSR_Message::getAddress64();

	convertUint32toArray(addr64.getMsb(), &buffer[1]);
	convertUint32toArray(addr64.getLsb(), &buffer[1+4]);
	convertUint32toArray(addr64.getLsb(), &buffer[1+4]);

	XBeeAddress16 addr16 = FSR_Message::getAddress16();
	convertUint16toArray(addr16, &buffer[1+4+4]);

	return 1 + 4 + 4 + 2;
};

size_t DA_Message::Encode(uint8_t *buffer, size_t limit) {
	// TODO
	return NULL;
};

size_t DAR_Message::Encode(uint8_t *buffer, size_t limit) {
	// TODO
	return NULL;
};