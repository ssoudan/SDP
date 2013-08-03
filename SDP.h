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
#define SCD_SIZE 40
#define LSD_SIZE 20
#define ACTION_BUFFER_SIZE 20
#define MESSAGE_BUFFER_SIZE 30

typedef struct {
	ServiceType sid;
	XBeeAddress64 address64;
	XBeeAddress16 address16;	
} RemoteServiceRecord;

typedef struct {
	ServiceType sid;
	ActionType aid;
	void (*callback)(uint8_t*, uint8_t);	
} ServiceCallbackRecord;

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
	ServiceCallbackRecord scd[SCD_SIZE];
	LocalServiceRecord lsd[LSD_SIZE];
	XBeeAddress64 local64;
	XBeeAddress16 local16;
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

			for (int i = 0 ; i < SCD_SIZE ; i++) {
				ServiceCallbackRecord record = { UNDEF_SERVICE, UNDEF_ACTION, NULL};
				scd[i] = record;
			}
		};

		ActionStatus doAction(ServiceType sid, ActionType actionType, 
			uint8_t actionParameterSize,
			uint8_t actionParameter[],
			void (*callback)(uint8_t*, uint8_t));

		RegistrationStatus registerService(ServiceType sid, ActionType actionType, 
			uint8_t (*callback)(uint8_t *in, size_t inSize, uint8_t *out, size_t outLimit));
		
		SDPState readPackets();

		inline XBeeAddress64 getLocal64() const { return local64; };
		inline void setLocal64(const XBeeAddress64 local64) { this->local64 = local64; };
		inline XBeeAddress16 getLocal16() const { return local16; };
		inline void setLocal16(const XBeeAddress16 local16) { this->local16 = local16; };

	private:
		SDPState transmitPacket(XBeeAddress64 &addr64, uint8_t message[], size_t length);

		RemoteServiceRecord *findRemoteService(ServiceType sid); 		
		LocalServiceRecord *findLocalService(ServiceType sid, ActionType aid); 
		LocalServiceRecord *findLocalService(ServiceType sid);
		ServiceCallbackRecord *findServiceCallback(ServiceType sid, ActionType aid); 
		ServiceCallbackRecord *findEmptyServiceCallbackRecord();
		RemoteServiceRecord *findEmptyRemoteServiceRecord();
		LocalServiceRecord *findEmptyLocalServiceRecord();

		XBeeAddress64 findService64(ServiceType sid); 
		XBeeAddress16 findService16(ServiceType sid); 

		SDPState processMessage(const XBeeAddress64 &addr64, const uint8_t *buffer, const size_t size);

		SDPState processMessage(const XBeeAddress64 &addr64, const Message *message);
		SDPState processMessage(const XBeeAddress64 &addr64, const FSR_Message *message);
		SDPState processMessage(XBeeAddress64 &addr64, const FS_Message *message);
		SDPState processMessage(XBeeAddress64 &addr64, DA_Message *message);
		SDPState processMessage(const XBeeAddress64 &addr64, const DAR_Message *message);

};

#endif
