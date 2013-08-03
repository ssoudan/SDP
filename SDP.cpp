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

LocalServiceRecord *SDP::findLocalService(ServiceType sid, ActionType aid) {

	for (int i = 0 ; i < LSD_SIZE ; i++) {		
		if (lsd[i].sid == sid && lsd[i].aid == aid )
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

SDPState SDP::processMessage(const XBeeAddress64 &addr64, const uint8_t *buffer, const size_t size) {

	Message* message = Message::Decode(buffer, size);

	if (message) {

		SDPState ret = processMessage(addr64, message);

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