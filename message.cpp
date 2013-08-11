/*
 Service Discovery Protocol - Message
 @author: Sebastien Soudan <sebastien.soudan@gmail.com>
*/

#include "message.h"
#include "util.h"

ServiceDiscovery::SDPState FS_Message::process(class MessageProcessor *sdp, XBeeAddress64 &addr64) {
	return sdp->processMessage(addr64, this);
}

ServiceDiscovery::SDPState FSR_Message::process(class MessageProcessor *sdp, XBeeAddress64 &addr64) {
	return sdp->processMessage(addr64, this);
}

ServiceDiscovery::SDPState DA_Message::process(class MessageProcessor *sdp, XBeeAddress64 &addr64) {
	return sdp->processMessage(addr64, this);
}

ServiceDiscovery::SDPState DAR_Message::process(class MessageProcessor *sdp, XBeeAddress64 &addr64) {
	return sdp->processMessage(addr64, this);
}

FS_Message *FS_Message::Decode(const uint8_t *buffer, const size_t size) {
	ASSERT (buffer != NULL);

	ASSERT (size >= FS_MESSAGE_SIZE);

	FS_Message *fs = new FS_Message();

	if (!fs) {
		ERROR("out of mem");
		return NULL;
	}

	// Skip first byte
	MOREINFO("FS-type", (uint8_t) buffer[1]);
	MOREINFO("FS-loc", (uint8_t) buffer[2]);

	fs->setServiceType((ServiceType) buffer[1]);
	fs->setServiceLocation((ServiceLocation) buffer[2]);
 
	return fs;
};

FSR_Message *FSR_Message::Decode(const uint8_t *buffer, const size_t size) {
	ASSERT (buffer != NULL);

	ASSERT (size >= FSR_MESSAGE_SIZE);

	FSR_Message *fsr = new FSR_Message();

	if (!fsr) {
		ERROR("out of mem");
		return NULL;
	}

	// Skip first byte

	ServiceType serviceType = (ServiceType) buffer[1];
	fsr->setServiceType(serviceType);

	ServiceLocation serviceLocation = (ServiceLocation) buffer[1 + 1];
	fsr->setServiceLocation(serviceLocation);
 
 	XBeeAddress64 addr64 = XBeeAddress64();
 	addr64.setMsb(convertArraytoUint32(&buffer[1 + 1 + 1]));
 	addr64.setLsb(convertArraytoUint32(&buffer[1 + 1 + 1 + 4]));
    fsr->setAddress64(addr64);

	return fsr;
};

DA_Message *DA_Message::Decode(const uint8_t *buffer, const size_t size) {
	ASSERT (buffer != NULL);

	ASSERT (size >= 5);

	DA_Message *da = new DA_Message();

	if (!da) {
		ERROR("out of mem");
		return NULL;
	}

	// Skip first byte

	da->setServiceType((ServiceType) buffer[1]);
	da->setServiceLocation((ServiceLocation) buffer[2]);
  	da->setActionType((ActionType) buffer[3]);

  	const uint8_t psize = buffer[4];
  	da->setActionParameterSize(psize);

	ASSERT (size >= 5 + psize);
	    
    da->setActionParameter(&buffer[5]);

	return da;
};

DAR_Message *DAR_Message::Decode(const uint8_t *buffer, const size_t size) {

	INFO("DARdec");
	ASSERT (buffer != NULL);

	ASSERT (size >= 6);

  	const uint8_t psize = buffer[5];

	INFO("psi");
	INFO((int) psize);

	DAR_Message *dar = new DAR_Message();

	if (!dar) {
		ERROR("out of mem");
		return NULL;
	}

	// Skip first byte
	
	dar->setServiceType((ServiceType) buffer[1]);
	dar->setServiceLocation((ServiceLocation) buffer[2]);
  	dar->setActionType((ActionType) buffer[3]);
	dar->setActionStatus((ActionStatus) buffer[4]);
  	dar->setActionResultSize(psize);

	ASSERT (size >= 6 + psize);
	    
    dar->setActionResult(&buffer[6]);

	return dar;
};

Message *Message::Decode(const uint8_t *buffer, const size_t size) {
    INFO("MSGdec");
    if (size < 1) 
    	return 0; /* or throw some exception */

	switch(buffer[0]) {
		case FS: 
		INFO("FS");
		return FS_Message::Decode(buffer, size);
		case FSR: 
		INFO("FSR");
		return FSR_Message::Decode(buffer, size);
		case DA: 
		INFO("DA");
		return DA_Message::Decode(buffer, size);
		case DAR: 
		INFO("DAR");
		return DAR_Message::Decode(buffer, size);
    default: return 0; // or throw an exception
	}
};

size_t FS_Message::Encode(uint8_t *buffer, size_t limit) {
	ASSERT (buffer != NULL);
		
	if (limit < FS_MESSAGE_SIZE)
		// Buffer too small;
		return 0;
	
	// Message type: 8 bits
	// Service Type: 8 bits
	// Service Location: 8 bits

	buffer[0] = FS;

	buffer[1] = FS_Message::getServiceType();
	buffer[2] = FS_Message::getServiceLocation();

	return FS_MESSAGE_SIZE;
};

size_t FSR_Message::Encode(uint8_t *buffer, size_t limit) {
	ASSERT (buffer != NULL);
		
	if (limit < FSR_MESSAGE_SIZE) {
		// Buffer too small;
		ERROR("Buffer too small.");	
		return 0;
	}
	
	// Message type: 8 bits
	// Service Type: 8 bits
	// Service Location: 8 bits
	// addr64: 64 bits
	//  - MSB (32 bit)
	//  - LSB (32 bit)

	buffer[0] = FSR;
	
	buffer[1] = FSR_Message::getServiceType();

	buffer[1 + 1] = FSR_Message::getServiceLocation();

	XBeeAddress64 addr64 = FSR_Message::getAddress64();

	convertUint32toArray(addr64.getMsb(), &buffer[1 + 1 + 1]);
	convertUint32toArray(addr64.getLsb(), &buffer[1 + 1 + 1 + 4]);

	return FSR_MESSAGE_SIZE;
};

size_t DA_Message::Encode(uint8_t *buffer, size_t limit) {

	ASSERT (buffer != NULL);

	uint8_t size = DA_Message::getActionParameterSize();

	if (limit < DA_MESSAGE_BASE_SIZE + size) {
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

	buffer[2] = DA_Message::getServiceLocation();

	buffer[3] = DA_Message::getActionType();

	buffer[4] = size;

	uint8_t *actionParameter = DA_Message::getActionParameter();
	for (int i = 0 ; i < size ; i++) {
		buffer[DA_MESSAGE_BASE_SIZE + i] = actionParameter[i];
	}

	return DA_MESSAGE_BASE_SIZE + size;
};

size_t DAR_Message::Encode(uint8_t *buffer, size_t limit) {

	ASSERT (buffer != NULL);

	uint8_t size = DAR_Message::getActionResultSize();

	if (limit < DAR_MESSAGE_BASE_SIZE + size) {
		// Buffer too small;
		ERROR("Buffer too small.");	
		return 0;
	}
	
	// Message type: 8 bits
	// Service Type: 8 bits
	// Service Location: 8 bits
	// Action Type: 8 bits
	// Action Status: 8 bits
	// Result Size: 8 bits
	// Result: ?

	buffer[0] = DAR;

	buffer[1] = DAR_Message::getServiceType();

	buffer[2] = DAR_Message::getServiceLocation();

	buffer[3] = DAR_Message::getActionType();

	buffer[4] = DAR_Message::getActionStatus();
	
	buffer[5] = size;

	uint8_t *actionResult = DAR_Message::getActionResult();
	for (int i = 0 ; i < size ; i++) {
		buffer[DAR_MESSAGE_BASE_SIZE + i] = actionResult[i];
	}

	return DAR_MESSAGE_BASE_SIZE + size;

};