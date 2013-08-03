/*
 Service Discovery Protocol
 @author: Sebastien Soudan <sebastien.soudan@gmail.com>
*/

#include "SDP.h"

#define INFO(x) Serial.println(x)

RemoteServiceRecord *SDP::findRemoteService(ServiceType sid) {

	for (int i = 0 ; i < RSD_SIZE ; i++) {		
		if (rsd[i].sid == sid)
			return &rsd[i];
	}

	return NULL;
};

ServiceCallbackRecord *SDP::findServiceCallback(ServiceType sid, ActionType aid) {
	for (int i = 0 ; i < SCD_SIZE ; i++) {		
		if (scd[i].sid == sid && scd[i].aid == aid )
			return &scd[i];
	}

	return NULL;
}; 

LocalServiceRecord *SDP::findLocalService(ServiceType sid, ActionType aid) {

	for (int i = 0 ; i < LSD_SIZE ; i++) {		
		if (lsd[i].sid == sid && lsd[i].aid == aid )
			return &lsd[i];
	}

	return NULL;
};

LocalServiceRecord *SDP::findLocalService(ServiceType sid) {

	for (int i = 0 ; i < LSD_SIZE ; i++) {		
		if (lsd[i].sid == sid)
			return &lsd[i];
	}

	return NULL;
};

LocalServiceRecord *SDP::findEmptyLocalServiceRecord() {

	for (int i = 0 ; i < LSD_SIZE ; i++) {		
		if (lsd[i].sid == UNDEF_SERVICE)
			return &lsd[i];
	}

	return NULL;
};


ServiceCallbackRecord *SDP::findEmptyServiceCallbackRecord() {

	for (int i = 0 ; i < SCD_SIZE ; i++) {		
		if (scd[i].sid == UNDEF_SERVICE)
			return &scd[i];
	}

	return NULL;
};


RemoteServiceRecord *SDP::findEmptyRemoteServiceRecord() {

	for (int i = 0 ; i < RSD_SIZE ; i++) {		
		if (rsd[i].sid == UNDEF_SERVICE)
			return &rsd[i];
	}

	return NULL;
};


SDPState SDP::readPackets() {

	//XBeeResponse response = XBeeResponse();	

	xbee->readPacket();
    
    if (xbee->getResponse().isAvailable()) {
      // got something
      
      if (xbee->getResponse().getApiId() == ZB_RX_RESPONSE) {
        // got a zb rx packet
        
        // now fill our zb rx class
    	ZBRxResponse rx = ZBRxResponse();

        xbee->getResponse().getZBRxResponse(rx);
            
        // if (rx.getOption() == ZB_PACKET_ACKNOWLEDGED) {
        //     // the sender got an ACK
        //     flashLed(statusLed, 10, 10);
        // } else {
        //     // we got it (obviously) but sender didn't get an ACK
        //     flashLed(errorLed, 2, 20);
        // }
        INFO("You have got a message!");
        return processMessage(rx.getRemoteAddress64(), rx.getData(), rx.getDataLength());

      } else if (xbee->getResponse().getApiId() == MODEM_STATUS_RESPONSE) {
  		ModemStatusResponse msr = ModemStatusResponse();

        xbee->getResponse().getModemStatusResponse(msr);
        // the local XBee sends this response on certain events, like association/dissociation        
        if (msr.getStatus() == ASSOCIATED) {
          // yay this is great.  flash led
          return XBEE_ASSOCIATED;
        } else if (msr.getStatus() == DISASSOCIATED) {
          // this is awful.. flash led to show our discontent
          return XBEE_DISASSOCIATED;
        } else {
          // another status
          return XBEE_UNKNOWN_MODEM_STATUS;
        }
      } else {
        // not something we were expecting
        return XBEE_UNEXPECTED_MSG;
      }
    } else if (xbee->getResponse().isError()) {
      //nss.print("Error reading packet.  Error code: ");  
      //nss.println(xbee.getResponse().getErrorCode());
    	return XBEE_ERROR;
    }

    return XBEE_NO_DATA;
};


SDPState SDP::transmitPacket(XBeeAddress64 &addr64, uint8_t message[], size_t length) {

	
	ZBTxRequest zbTx = ZBTxRequest(addr64, &message[0], length);
	ZBTxStatusResponse txStatus = ZBTxStatusResponse();


	xbee->send(zbTx);

	// after sending a tx request, we expect a status response
	// wait up to half second for the status response
	if (xbee->readPacket(500)) {
		// got a response!

		// should be a znet tx status             
		if (xbee->getResponse().getApiId() == ZB_TX_STATUS_RESPONSE) {
			xbee->getResponse().getZBTxStatusResponse(txStatus);

			// get the delivery status, the fifth byte
			if (txStatus.getDeliveryStatus() == SUCCESS) {
				// success.  time to celebrate
				return XBEE_SUCCESS;
			} else {
				// the remote XBee did not receive our packet. is it powered on?
				return XBEE_NOT_DELIVERED;
			}
		}
	} else if (xbee->getResponse().isError()) {
		//nss.print("Error reading packet.  Error code: ");  
		//nss.println(xbee->getResponse().getErrorCode());
		return XBEE_ERROR;
	} else {
		// local XBee did not provide a timely TX Status Response -- should not happen
		return XBEE_ERROR;
	}

};

SDPState SDP::processMessage(XBeeAddress64 &addr64, const uint8_t *buffer, const size_t size) {

	Message* message = Message::Decode(buffer, size);

	if (message) {

		SDPState ret = message->process(static_cast<MessageProcessor*>(this), addr64);

		delete message;

		return ret;
	}

	return UNKNOWN_MESSAGE;

};

RegistrationStatus SDP::registerService(ServiceType sid, ActionType actionType, 
			uint8_t (*callback)(uint8_t *in, size_t inSize, uint8_t *out, size_t outLimit)) {

	ASSERT(sid != 0);
	ASSERT(actionType != 0);
	ASSERT(callback != NULL);

	LocalServiceRecord *seat = findEmptyLocalServiceRecord();

	if (seat == NULL) {
		ERROR("No available seat, failed to register service");
		return REGISTRATION_FAILED;
	}

	seat->sid = sid;
	seat->callback = callback;

}


SDPState SDP::processMessage(XBeeAddress64 &addr64, DA_Message *message) { 
#ifndef ARDUINO
	cout << "da_message" << std::endl; 
#endif		
	ServiceType sid = message->getServiceType();
	ActionType aid = message->getActionType();

	// Find the service locally, 
	LocalServiceRecord* record = SDP::findLocalService(sid, aid); 

	size_t size = 0;

	// FOUND?
	if (record && record->callback) {				
		// prepare the call
		uint8_t out[ACTION_BUFFER_SIZE];

		// call the callback,
		size = (*record->callback)(message->getActionParameter(), 
										  message->getActionParameterSize(), 
										  out, 
										  ACTION_BUFFER_SIZE);
		if (size > 0) {
			// send the result
			DAR_Message response = DAR_Message(sid, aid, DONE);
			uint8_t buffer[size + 5];

			// override size
			size = response.Encode(buffer, sizeof(buffer));

			if (size > 0) {
				// Transmit
				transmitPacket(addr64, buffer, size);
				INFO("All good, action executed and message sent");
				return DA_RECEIVED;
			} else {
				// Failed to build response
				ERROR("Failed to build the response");
				return DA_RECEIVED;
			}
		} 
		// else send an error
		ERROR("Execution failed");
	} 

	INFO("Callback not found");
	// Send ERROR message		
	DAR_Message response = 	DAR_Message(sid, aid, NOT_FOUND);
	uint8_t buffer[5];

	// override size
	size = response.Encode(buffer, sizeof(buffer));

	if (size > 0) {
		// Transmit
		transmitPacket(addr64, buffer, size);
		INFO("Sent action NOT_FOUND message");
		return DA_RECEIVED;
	} else {
		// Failed to build response
		ERROR("Failed to send NOT_FOUND message");
		return DA_RECEIVED;
	}
};

SDPState SDP::processMessage(XBeeAddress64 &addr64, const FS_Message *message) { 
#ifndef ARDUINO			
	cout << "fs_message" << std::endl; 
#endif

	// find the service if available and respond to the requester
	const ServiceType sid = message->getServiceType();

	LocalServiceRecord *service = findLocalService(sid);

	if (service) {
		FSR_Message message = FSR_Message(sid);
		message.setAddress64(this->getLocal64());
		message.setAddress16(this->getLocal16());

		uint8_t buffer[MESSAGE_BUFFER_SIZE];

		// override size
		size_t size = message.Encode(buffer, sizeof(buffer));
	
		if (size > 0) {
			transmitPacket(addr64, buffer, size);
		}

		return FS_RECEIVED;
	}

	// nothing

	return FS_RECEIVED;
};

// SDPState SDP::processMessage(const XBeeAddress64 &addr64, const Message *message) { 			
// #ifndef ARDUINO
// 	cout << "message" << std::endl; 
// #endif
// 	return UNKNOWN_STATE;
// };


SDPState SDP::processMessage(XBeeAddress64 &addr64, const FSR_Message *message) { 
#ifndef ARDUINO
	cout << "fsr_message" << std::endl; 
#endif

	// Update records with the service
	RemoteServiceRecord *slot = findEmptyRemoteServiceRecord();

	if (slot) {
		slot->sid = message->getServiceType();
		slot->address64 = message->getAddress64();
		slot->address16 = message->getAddress16();

		return FSR_RECEIVED;
	}

	ERROR("Failed to find an empty slot");

	return FSR_RECEIVED;
};


SDPState SDP::processMessage(XBeeAddress64 &addr64, const DAR_Message *message) { 
#ifndef ARDUINO
	cout << "dar_message" << std::endl; 
#endif

	ServiceType sid = message->getServiceType();
	ActionType aid = message->getActionType();

	// decode the result
	switch (message->getActionStatus()) {
		case DONE:
		{
			ServiceCallbackRecord *scd = findServiceCallback(sid, aid); 
			if (scd) {
				INFO("Callback found, calling it...");
				scd->callback(message->getActionResult(), message->getActionResultSize());
				return DAR_RECEIVED;
			} else {
				ERROR("Invalid callback");
				return DAR_RECEIVED;
			}
			break;
		}
			
		case NOT_FOUND:
		{
			// In case of NOT_FOUND, invalidate the service cache for this entry
			RemoteServiceRecord *remoteService = findRemoteService(sid);
			remoteService->sid = UNDEF_SERVICE;
			remoteService->address64 = XBeeAddress64();
			remoteService->address16 = 0;
			ERROR("Action not found, cleaning remote entry");
			return DAR_RECEIVED;
			break;
		}
		
		case NOT_DONE:
		{
			ERROR("Action execution failed");
			return DAR_RECEIVED;
			break;
		}
		
		default:
		{
			ERROR("Unexpected");
			return DAR_RECEIVED;
		}
	};

	return DAR_RECEIVED;
};


ActionStatus SDP::doAction(ServiceType sid, ActionType actionType, 
			uint8_t actionParameterSize,
			uint8_t actionParameter[],
			void (*callback)(uint8_t*, uint8_t)) {

	// Try to find the service locally
	LocalServiceRecord *localService = findLocalService(sid, actionType);

	// if found try the action
	if (localService) {
		uint8_t buffer[ACTION_BUFFER_SIZE];

		size_t size = localService->callback(&actionParameter[0], actionParameterSize, buffer, sizeof(buffer));

		if (size > 0) {

			// store the result somewhere
			if (callback) {
				callback(buffer, size);
				ERROR("Local action DONE");
				return DONE;
			} else {
				ERROR("Invalid callback for local action");
				return NOT_DONE;
			}
		}

		// action failed
		ERROR("Local action failed");
		return NOT_DONE;		
	}

	// else try to find the remote service (cached)
	RemoteServiceRecord *remoteService = findRemoteService(sid);

	// if found try the action
	if (remoteService) {

		// store the callback
		ServiceCallbackRecord *scd = findServiceCallback(sid, actionType); 
		if (!scd) {
			scd = findEmptyServiceCallbackRecord();	
			scd->sid = sid;
			scd->aid = actionType;
			scd->callback = callback;
		} else {			
			ERROR("No seat to store the callback");
			return NOT_DONE;
		}		

		// Send the request
		DA_Message message = DA_Message(sid, actionType);
		message.setActionParameterSize(actionParameterSize);
		message.setActionParameter(actionParameter);

		uint8_t buffer[MESSAGE_BUFFER_SIZE];

		size_t size = message.Encode(buffer, sizeof(buffer));	

		XBeeAddress64 addr64 = remoteService->address64;

		// send the request
		transmitPacket(addr64, buffer, size);

		INFO("Action requested");
		return REQUESTED;
	}

	// else request try to find the service
	uint8_t buffer[MESSAGE_BUFFER_SIZE];

	FS_Message message = FS_Message(sid);

	size_t size = message.Encode(buffer, sizeof(buffer));	

	// send the bcast
	XBeeAddress64 bcast64 = XBeeAddress64(0l, 0xffffl);

	transmitPacket(bcast64, buffer, size);

	INFO("Service requested");
	return SEARCHING;
};


