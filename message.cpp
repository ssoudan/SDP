/*
 Service Discovery Protocol - Message
 @author: Sebastien Soudan <sebastien.soudan@gmail.com>
*/

#include "message.h"
#include "util.h"

FS_Message *FS_Message::Decode(const uint8_t *buffer, const size_t size) {
	ASSERT (buffer != NULL);

	ASSERT (size >= 2);

	FS_Message *fs = new FS_Message();

	// Skip first byte

	ServiceType serviceType = (ServiceType) buffer[1];
	fs->setServiceType(serviceType);
 
	return fs;
};

FSR_Message *FSR_Message::Decode(const uint8_t *buffer, const size_t size) {
	ASSERT (buffer != NULL);

	ASSERT (size >= 1 + 1 + 4 + 4 + 2);

	FSR_Message *fsr = new FSR_Message();

	// Skip first byte

	ServiceType serviceType = (ServiceType) buffer[1];
	fsr->setServiceType(serviceType);
 
 	XBeeAddress64 addr64 = XBeeAddress64();
 	addr64.setMsb(convertArraytoUint32(&buffer[1 + 1]));
 	addr64.setLsb(convertArraytoUint32(&buffer[1 + 1 + 4]));
    fsr->setAddress64(addr64);

    XBeeAddress16 addr16 = convertArraytoUint16(&buffer[1 + 1 + 4 + 4]);
    
    fsr->setAddress16(addr16);

	return fsr;
};

DA_Message *DA_Message::Decode(const uint8_t *buffer, const size_t size) {
	ASSERT (buffer != NULL);

	ASSERT (size >= 4);

	DA_Message *da = new DA_Message();

	// Skip first byte

	ServiceType serviceType = (ServiceType) buffer[1];
	da->setServiceType((ServiceType) serviceType);
  	da->setActionType((ActionType) buffer[2]);

  	const uint8_t psize = buffer[3];
  	da->setActionParameterSize(psize);

	ASSERT (size >= 4 + psize);

 // 	uint8_t* array = new uint8_t[psize];
 // 	memcpy(array, &buffer[4], psize);
	    
    da->setActionParameter(&buffer[4]);

	return da;
};

DAR_Message *DAR_Message::Decode(const uint8_t *buffer, const size_t size) {
	ASSERT (buffer != NULL);

	ASSERT (size >= 5);

	DAR_Message *dar = new DAR_Message();

	// Skip first byte

	ServiceType serviceType = (ServiceType) buffer[1];
	dar->setServiceType((ServiceType) serviceType);
  	dar->setActionType((ActionType) buffer[2]);
	dar->setActionStatus((ActionStatus) buffer[3]);

  	const uint8_t psize = buffer[4];
  	dar->setActionResultSize(psize);

	ASSERT (size >= 5 + psize);

//  	uint8_t* array = new uint8_t[psize];
// 	memcpy(array, &buffer[5], psize);
	    
    dar->setActionResult(&buffer[5]);

	return dar;
};

Message *Message::Decode(const uint8_t *buffer, const size_t size) {
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
	ASSERT (buffer != NULL);
		
	if (limit < 2)
		// Buffer too small;
		return 0;
	
	// Message type: 8 bits
	// Service Type: 8 bits

	buffer[0] = FS;

	buffer[1] = FS_Message::getServiceType();

	return 2;
};

size_t FSR_Message::Encode(uint8_t *buffer, size_t limit) {
	ASSERT (buffer != NULL);
		
	if (limit < 1 + 1 + 4 + 4 + 2) {
		// Buffer too small;
		ERROR("Buffer too small.");	
		return 0;
	}
	
	// Message type: 8 bits
	// Service Type: 8 bits
	// addr64: 64 bits
	//  - MSB (32 bit)
	//  - LSB (32 bit)
	// addr16: 16 bits

	buffer[0] = FSR;
	
	buffer[1] = FSR_Message::getServiceType();

	XBeeAddress64 addr64 = FSR_Message::getAddress64();

	convertUint32toArray(addr64.getMsb(), &buffer[1 + 1]);
	convertUint32toArray(addr64.getLsb(), &buffer[1 + 1 + 4]);

	XBeeAddress16 addr16 = FSR_Message::getAddress16();
	convertUint16toArray(addr16, &buffer[1 + 1 + 4 + 4]);

	return 1 + 1 + 4 + 4 + 2;
};

size_t DA_Message::Encode(uint8_t *buffer, size_t limit) {

	ASSERT (buffer != NULL);

	uint8_t size = DA_Message::getActionParameterSize();

	if (limit < 4 + size) {
		// Buffer too small;
		ERROR("Buffer too small.");	
		return 0;
	}
	
	// Message type: 8 bits
	// Service Type: 8 bits
	// Action Type: 8 bits
	// Parameter Size: 8 bits
	// Parameter: ?

	buffer[0] = DA;

	buffer[1] = DA_Message::getServiceType();

	buffer[2] = DA_Message::getActionType();

	buffer[3] = size;

	uint8_t *actionParameter = DA_Message::getActionParameter();
	for (int i = 0 ; i < size ; i++) {
		buffer[4 + i] = actionParameter[i];
	}

	return 4 + size;
};

size_t DAR_Message::Encode(uint8_t *buffer, size_t limit) {

	ASSERT (buffer != NULL);

	uint8_t size = DAR_Message::getActionResultSize();

	if (limit < 5 + size) {
		// Buffer too small;
		ERROR("Buffer too small.");	
		return 0;
	}
	
	// Message type: 8 bits
	// Service Type: 8 bits
	// Action Type: 8 bits
	// Action Status: 8 bits
	// Result Size: 8 bits
	// Result: ?

	buffer[0] = DAR;

	buffer[1] = DAR_Message::getServiceType();

	buffer[2] = DAR_Message::getActionType();

	buffer[3] = DAR_Message::getActionStatus();
	
	buffer[4] = size;

	uint8_t *actionResult = DAR_Message::getActionResult();
	for (int i = 0 ; i < size ; i++) {
		buffer[5 + i] = actionResult[i];
	}

	return 5 + size;

};