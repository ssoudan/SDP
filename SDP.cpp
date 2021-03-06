/*
 Service Discovery Protocol
 @author: Sebastien Soudan <sebastien.soudan@gmail.com>
*/

#include "SDP.h"


RemoteServiceRecord *SDP::findRemoteService(ServiceType sid) {

	for (int i = 0 ; i < RSD_SIZE ; i++) {		
		if (rsd[i].sid == sid)
			return &rsd[i];
	}

	return NULL;
}; 

RemoteServiceRecord *SDP::findRemoteService(ServiceType sid, ServiceLocation sl) {

	for (int i = 0 ; i < RSD_SIZE ; i++) {		
		if (rsd[i].sid == sid && rsd[i].sl == sl)
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

ServiceCallbackRecord *SDP::findServiceCallback(ServiceType sid, ServiceLocation sl, ActionType aid) {
	for (int i = 0 ; i < SCD_SIZE ; i++) {		
		if (scd[i].sid == sid && scd[i].aid == aid && scd[i].sl == sl)
			return &scd[i];
	}

	return NULL;
}; 

LocalServiceRecord *SDP::findLocalService(ServiceType sid, ServiceLocation sl, ActionType aid) {

	for (int i = 0 ; i < LSD_SIZE ; i++) {		
		if (lsd[i].sid == sid && lsd[i].aid == aid && lsd[i].sl == sl)
			return &lsd[i];
	}

	return NULL;
};


LocalServiceRecord *SDP::findLocalService(ServiceType sid, ServiceLocation sl) {

	for (int i = 0 ; i < LSD_SIZE ; i++) {		
		if (lsd[i].sid == sid && lsd[i].sl == sl)
			return &lsd[i];
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
      	INFO("readPackets: isAvailable()");


      if (xbee->getResponse().getApiId() == ZB_RX_RESPONSE) {
      	INFO("ZB_RX_STATUS_RESPONSE");

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
        INFO("new msg!");
        INFO((int) rx.getDataLength());
        INFO("bytes");
        return processMessage(rx.getRemoteAddress64(), rx.getData(), rx.getDataLength());
	  } else if (xbee->getResponse().getApiId() == ZB_TX_STATUS_RESPONSE) {
	  	INFO("ZB_TX_STATUS_RESPONSE");
        return XBEE_UNEXPECTED_MSG;
      } else if (xbee->getResponse().getApiId() == MODEM_STATUS_RESPONSE) {
	  	INFO("MODEM_STATUS_RESPONSE");

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

	INFO("Packet supposedly sent");
	// after sending a tx request, we expect a status response
	// wait up to half second for the status response
	if (xbee->readPacket(1000)) {
		// got a response!
		INFO("Got a tx status report?");

		// should be a znet tx status             
		if (xbee->getResponse().getApiId() == ZB_TX_STATUS_RESPONSE) {
			xbee->getResponse().getZBTxStatusResponse(txStatus);

			INFO("ZB_TX_STATUS_RESPONSE");

			// get the delivery status, the fifth byte
			if (txStatus.getDeliveryStatus() == SUCCESS) {
				INFO("XBEE_SUCCESS");
				// success.  time to celebrate
				return XBEE_SUCCESS;
			} else {
				INFO("XBEE_NOT_DELIVERED");
				// the remote XBee did not receive our packet. is it powered on?
				return XBEE_NOT_DELIVERED;
			}	
		} else if (xbee->getResponse().isError()) {
			//nss.print("Error reading packet.  Error code: ");  
			//nss.println(xbee->getResponse().getErrorCode());
			MOREINFO("XBEE_ERROR", xbee->getResponse().getErrorCode());
			return XBEE_ERROR;
		} else {
			// local XBee did not provide a timely TX Status Response -- should not happen
			INFO("XBEE_ERROR");
			return XBEE_ERROR;
		}
	}

	return XBEE_ERROR;
};

SDPState SDP::processMessage(XBeeAddress64 &addr64, const uint8_t *buffer, const size_t size) {

	Message* message = Message::Decode(buffer, size);
	INFO("Msg decoded");
	if (message) {

		INFO("process msg");
		SDPState ret = message->process(static_cast<MessageProcessor*>(this), addr64);	
		delete message;
		INFO("proc&release");
		return ret;
	}
	ERROR("Decode fail");
	return UNKNOWN_MESSAGE;

};

RegistrationStatus SDP::registerService(ServiceType sid, ActionType actionType, 
			uint8_t (*callback)(uint8_t *in, size_t inSize, uint8_t *out, size_t outLimit)) {

	return SDP::registerService(sid, ServiceDiscovery::UNDEF_LOCATION, actionType, callback);
}


RegistrationStatus SDP::registerService(ServiceType sid, ServiceLocation sl, ActionType actionType, 
			uint8_t (*callback)(uint8_t *in, size_t inSize, uint8_t *out, size_t outLimit)) {

	ASSERT(sid != 0);
	ASSERT(actionType != 0);
	ASSERT(callback != NULL);

	LocalServiceRecord *seat = findEmptyLocalServiceRecord();

	if (seat == NULL) {
		ERROR("No seat, failed to register srv");
		return REGISTRATION_FAILED;
	}

	seat->sid = sid;
	seat->sl = sl;
	seat->aid = actionType;
	seat->callback = callback;

	return REGISTRATION_SUCCESS;
}



SDPState SDP::processMessage(XBeeAddress64 &addr64, DA_Message *message) { 
#if !defined(ARDUINO) && defined(DEBUG)
	cerr << "da_message" << std::endl; 
#endif		
	ServiceType sid = message->getServiceType();
	ServiceLocation sl = message->getServiceLocation();
	ActionType aid = message->getActionType();

	// Find the service locally, 
	LocalServiceRecord* record = SDP::findLocalService(sid, sl, aid); 
	DAR_Message response = DAR_Message(sid, aid, NOT_FOUND, sl);
	size_t size = 0;

	if (record) {
		INFO("Rec found");
	}

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
			response.setActionStatus(DONE);
			response.setActionResultSize(size);
			response.setActionResult(out);

			uint8_t buffer[size + DAR_MESSAGE_BASE_SIZE];

			// override size
			size = response.Encode(buffer, sizeof(buffer));

			if (size > 0) {
				INFO((int) size);
				// Transmit
				if (transmitPacket(addr64, buffer, size) == XBEE_SUCCESS)
					INFO("Action executed, mesg sent");
				else
					ERROR("Action executed, mesg failed"); 
				return DA_RECEIVED;
			} else {
				// Failed to build response
				ERROR("Failed to build the response");
				return DA_RECEIVED;
			}
		} 
		// else send an error
		response.setActionStatus(NOT_DONE);
		ERROR("Execution failed");
	} 

	INFO("Callback not found");
	// Send ERROR message		
	response.setActionStatus(NOT_FOUND);
	uint8_t buffer[DAR_MESSAGE_BASE_SIZE];

	// override size
	size = response.Encode(buffer, sizeof(buffer));

	if (size > 0) {
		// Transmit
		if (transmitPacket(addr64, buffer, size) == XBEE_SUCCESS)
			INFO("Sent action NOT_FOUND message");
		else 
			ERROR("Failed to send NOT_FOUND message");
		return DA_RECEIVED;
	} else {
		// Failed to build response
		ERROR("Failed to send NOT_FOUND message");
		return DA_RECEIVED;
	}
};

SDPState SDP::processMessage(XBeeAddress64 &addr64, const FS_Message *message) { 
#if !defined(ARDUINO) && defined(DEBUG)
	cerr << "fs_message" << std::endl; 
#endif

	// find the service if available and respond to the requester
	const ServiceType sid = message->getServiceType();
	const ServiceLocation sl = message->getServiceLocation();

	LocalServiceRecord *service = findLocalService(sid, sl);

	if (service) {
		FSR_Message message = FSR_Message(sid, sl);
		message.setAddress64(this->getLocal64());

		uint8_t buffer[FSR_MESSAGE_SIZE];

		// override size
		size_t size = message.Encode(buffer, sizeof(buffer));
	
		if (size > 0) {
			if (transmitPacket(addr64, buffer, size) == XBEE_SUCCESS)
				INFO("FSR sent");
			else 
				ERROR("FSR not sent");
		}

		return FS_RECEIVED;
	}

	// nothing

	return FS_RECEIVED;
};


SDPState SDP::processMessage(XBeeAddress64 &addr64, const FSR_Message *message) { 
#if !defined(ARDUINO) && defined(DEBUG)
	cerr << "fsr_message" << std::endl; 
#endif

	// Update records with the service
	RemoteServiceRecord *slot = findEmptyRemoteServiceRecord();

	if (slot) {
		INFO("Updating RSD");
		slot->sid = message->getServiceType();
		slot->sl = message->getServiceLocation();
		slot->address64 = message->getAddress64();

		return FSR_RECEIVED;
	}

	ERROR("Failed to find a slot - consider increase RSD_SIZE");

	return FSR_RECEIVED;
};


SDPState SDP::processMessage(XBeeAddress64 &addr64, const DAR_Message *message) { 
#if !defined(ARDUINO) && defined(DEBUG)
	cerr << "dar_message" << std::endl; 
#endif

	INFO("processMessage DAR");


	ServiceType sid = message->getServiceType();
	ServiceLocation sl = message->getServiceLocation();
	ActionType aid = message->getActionType();

	MOREINFO("DAR-type", (uint8_t) sid);
	MOREINFO("DAR-loc", (uint8_t) sl);
	MOREINFO("DAR-action", (uint8_t) aid);

	// decode the result
	switch (message->getActionStatus()) {
		case DONE:
		{
			ServiceCallbackRecord *scd = findServiceCallback(sid, sl, aid); 
			if (scd) {
				INFO("Callback found");
				scd->callback(message->getActionResult(), message->getActionResultSize());
			} else {
				ERROR("Callback not found");
			}
			break;
		}
			
		case NOT_FOUND:
		{
			// In case of NOT_FOUND, invalidate the service cache for this entry
			RemoteServiceRecord *remoteService = findRemoteService(sid, sl);
			remoteService->sid = UNDEF_SERVICE;
			remoteService->address64 = XBeeAddress64();
			//remoteService->address16 = 0;
			ERROR("Action not found, cleaned remote entry");
			break;
		}
		
		case NOT_DONE:
		{
			ERROR("Action failed");
			break;
		}
		
		default:
		{
			ERROR("Unexpected");
			break;
		}
	};
	INFO("processMsg DAR done");
	return DAR_RECEIVED;
};


ActionStatus SDP::doAction(ServiceType sid, 
			ServiceLocation sl,
			ActionType actionType, 			
			uint8_t actionParameterSize,
			uint8_t actionParameter[],
			void (*callback)(uint8_t*, uint8_t)) {

	// Try to find the service locally
	LocalServiceRecord *localService = findLocalService(sid, sl, actionType);

	// if found try the action
	if (localService) {
		uint8_t buffer[ACTION_BUFFER_SIZE];

		size_t size = localService->callback(&actionParameter[0], actionParameterSize, buffer, sizeof(buffer));

		if (size > 0) {

			// store the result somewhere
			if (callback) {
				callback(buffer, size);
				ERROR("LAction DONE");
				return DONE;
			} else {
				ERROR("Invalid callback LAction");
				return NOT_DONE;
			}
		}

		// action failed
		ERROR("LAction failed");
		return NOT_DONE;		
	}

	INFO("LAction not found");

	// else try to find the remote service (cached)
	RemoteServiceRecord *remoteService = findRemoteService(sid, sl);

	// if found try the action
	if (remoteService) {
		INFO("RAction found");
		// store the callback
		ServiceCallbackRecord *scd = findServiceCallback(sid, sl, actionType); 
		if (!scd) {
			scd = findEmptyServiceCallbackRecord();	

			if (!scd) {			
				ERROR("No seat for callback");
				return NOT_DONE;
			}	

			scd->sid = sid;
			scd->sl = sl;
			scd->aid = actionType;
			scd->callback = callback;
		} 

		// Send the request
		DA_Message message = DA_Message(sid, actionType);
		message.setServiceLocation(sl);
		message.setActionParameterSize(actionParameterSize);
		message.setActionParameter(actionParameter);

		uint8_t buffer[DA_MESSAGE_BASE_SIZE + actionParameterSize];

		size_t size = message.Encode(buffer, sizeof(buffer));	

		XBeeAddress64 addr64 = remoteService->address64;

		// send the request
		if (transmitPacket(addr64, buffer, size) == XBEE_SUCCESS) 
			INFO("Action req'ed");	
		else 
			ERROR("Action req. msg failed");	
		
		return REQUESTED;
	}

	INFO("Service not in cache");

	// else request try to find the service
	uint8_t buffer[FS_MESSAGE_SIZE];

	FS_Message message = FS_Message(sid, sl);

	size_t size = message.Encode(buffer, sizeof(buffer));	

	// send the bcast
	XBeeAddress64 bcast64 = XBeeAddress64(0l, 0xffffl);

	if (transmitPacket(bcast64, buffer, size) == XBEE_SUCCESS) 
		INFO("Service req'ed");
	
	return SEARCHING;
};


