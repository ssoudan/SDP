/*
 Service Discovery Protocol
 @author: Sebastien Soudan <sebastien.soudan@gmail.com>
*/

#ifndef SDP_H_
#define SDP_H_

#include <XBee.h>
//#include "Arduino.h"
#include "message.h"
#include "util.h"

#ifndef ARDUINO
#include <iostream>
using namespace std;
#endif

#define RSD_SIZE 20
#define LSD_SIZE 20
#define ACTION_BUFFER_SIZE 20

typedef struct {
	ServiceType sid;
	XBeeAddress64 address64;
	XBeeAddress16 address16;	
} RemoteServiceRecord;

typedef struct {
	ServiceType sid;
	ActionType aid;
	uint8_t (*callback)(uint8_t *in, size_t inSize, uint8_t *out, size_t outLimit);	
} LocalServiceRecord;

enum SDPState {UNKNOWN_STATE = 0, UNKNOWN_MESSAGE, FSR_RECEIVED, DA_RECEIVED, DAR_RECEIVED, FS_RECEIVED, 
	XBEE_ASSOCIATED, XBEE_DISASSOCIATED, XBEE_UNKNOWN_MODEM_STATUS, XBEE_UNEXPECTED_MSG, XBEE_ERROR, XBEE_SUCCESS, XBEE_NOT_DELIVERED };

enum RegistrationStatus {REGISTRATION_FAILED = 0, REGISTRATION_SUCCESS = 1};

class SDP {
	RemoteServiceRecord rsd[RSD_SIZE];
	LocalServiceRecord lsd[LSD_SIZE];
	XBee *xbee;

	public: 
		SDP(XBee *xbee) {
			this->xbee = xbee;
	
			for (int i = 0 ; i < RSD_SIZE ; i++) {
				RemoteServiceRecord record = { UNDEF_SERVICE, XBeeAddress64(), 0};
				rsd[i] = record;
			}

			for (int i = 0 ; i < LSD_SIZE ; i++) {
				LocalServiceRecord record = { UNDEF_SERVICE, UNDEF_ACTION, 0};
				lsd[i] = record;
			}
		};

		ActionStatus doAction(ServiceType sid, ActionType actionType, 
			uint8_t actionParameterSize,
			uint8_t actionParameter[]);

		RegistrationStatus registerService(ServiceType sid, ActionType actionType, 
			uint8_t (*callback)(uint8_t *in, size_t inSize, uint8_t *out, size_t outLimit));
		
		SDPState readPackets();
		SDPState transmitPacket(XBeeAddress64 &addr64, uint8_t message[], size_t length);

	private:
		RemoteServiceRecord *findRemoteService(ServiceType sid); 		
		LocalServiceRecord *findLocalService(ServiceType sid, ActionType aid); 
		RemoteServiceRecord *findEmptyRemoteServiceRecord();
		LocalServiceRecord *findEmptyLocalServiceRecord();

		XBeeAddress64 findService64(ServiceType sid); 
		XBeeAddress16 findService16(ServiceType sid); 

		SDPState processMessage(const XBeeAddress64 &addr64, const uint8_t *buffer, const size_t size);

		inline SDPState processMessage(const XBeeAddress64 &addr64, const Message *message) { 			
#ifndef ARDUINO
			cout << "message" << std::endl; 
#endif
			return UNKNOWN_STATE;
		};
		inline SDPState processMessage(const XBeeAddress64 &addr64, const FSR_Message *message) { 
#ifndef ARDUINO
			cout << "fsr_message" << std::endl; 
#endif
	
			// Update records with the service

			return FSR_RECEIVED;
		};
		inline SDPState processMessage(const XBeeAddress64 &addr64, const FS_Message *message) { 
#ifndef ARDUINO			
			cout << "fs_message" << std::endl; 
#endif

			// TODO find the service if available and respond to the requester

			return FS_RECEIVED;
		};
		inline SDPState processMessage(XBeeAddress64 &addr64, DA_Message *message) { 
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
					DAR_Message response = 	DAR_Message(sid, aid, DONE);
					uint8_t buffer[size + 5];

					// override size
					size = response.Encode(buffer, sizeof(buffer));

					if (size > 0) {
						// Transmit
						transmitPacket(addr64, buffer, size);

						return DA_RECEIVED;
					} else {
						// Failed to build response
						return DA_RECEIVED;
					}

					return DA_RECEIVED;
				} 
				// else send an error			    
			} 

			// Send ERROR message		
			DAR_Message response = 	DAR_Message(sid, aid, NOT_FOUND);
			uint8_t buffer[5];

			// override size
			size = response.Encode(buffer, sizeof(buffer));

			if (size > 0) {
				// Transmit
				// TODO
				transmitPacket(addr64, buffer, size);

				return DA_RECEIVED;
			} else {
				// Failed to build response
				return DA_RECEIVED;
			}

			// delete what has to be deleted
			// TODO 

			return DA_RECEIVED;			
		};
		inline SDPState processMessage(const XBeeAddress64 &addr64, const DAR_Message *message) { 
#ifndef ARDUINO
			cout << "dar_message" << std::endl; 
#endif

			// TODO decode the result, return it to the local action requester

			return DAR_RECEIVED;
		};



};

#endif