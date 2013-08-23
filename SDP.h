/*
 Service Discovery Protocol
 @author: Sebastien Soudan <sebastien.soudan@gmail.com>
*/

#ifndef SDP_H_
 #define SDP_H_

#include "enum.h"

#include "sdp-config.h"

#include "message.h"
#include "util.h"

using namespace std;

typedef struct {
	ServiceType sid;
	ServiceLocation sl;
	XBeeAddress64 address64;
	//XBeeAddress16 address16;	
} RemoteServiceRecord;

typedef struct {
	ServiceType sid;
	ServiceLocation sl;
	ActionType aid;
	void (*callback)(uint8_t*, uint8_t);	
} ServiceCallbackRecord;

typedef struct {
	ServiceType sid;
	ServiceLocation sl;
	ActionType aid;
	uint8_t (*callback)(uint8_t *in, size_t inSize, uint8_t *out, size_t outLimit);	
} LocalServiceRecord;

using namespace ServiceDiscovery;

class SDP : public MessageProcessor {
	RemoteServiceRecord rsd[RSD_SIZE];
	ServiceCallbackRecord scd[SCD_SIZE];
	LocalServiceRecord lsd[LSD_SIZE];
	XBeeAddress64 local64;
	XBee *xbee;

	public: 
		SDP() {
			for (int i = 0 ; i < RSD_SIZE ; i++) {
				RemoteServiceRecord record = { UNDEF_SERVICE, UNDEF_LOCATION, XBeeAddress64()};
				rsd[i] = record;
			}

			for (int i = 0 ; i < SCD_SIZE ; i++) {
				ServiceCallbackRecord record = { UNDEF_SERVICE, UNDEF_LOCATION, UNDEF_ACTION, NULL};
				scd[i] = record;
			}

			for (int i = 0 ; i < LSD_SIZE ; i++) {
				LocalServiceRecord record = { UNDEF_SERVICE, UNDEF_LOCATION, UNDEF_ACTION, 0};
				lsd[i] = record;
			}
		};
		SDP(XBee *xbee) {
			SDP();
			this->xbee = xbee;
		};

		ActionStatus doAction(ServiceType sid, ServiceLocation sl, ActionType actionType, 
			uint8_t actionParameterSize,
			uint8_t actionParameter[],
			void (*callback)(uint8_t*, uint8_t));

		RegistrationStatus registerService(ServiceType sid, ActionType actionType, 
			uint8_t (*callback)(uint8_t *in, size_t inSize, uint8_t *out, size_t outLimit));

		RegistrationStatus registerService(ServiceType sid, ServiceLocation sl, ActionType actionType, 
			uint8_t (*callback)(uint8_t *in, size_t inSize, uint8_t *out, size_t outLimit));
		
		SDPState readPackets();

		inline XBeeAddress64 getLocal64() const { return local64; };
		inline void setLocal64(const XBeeAddress64 local64) { this->local64 = local64; };
		inline XBee* getXBee() { return xbee; };
		inline void setXBee(XBee *xbee) { this->xbee = xbee; };

		virtual SDPState processMessage(XBeeAddress64 &addr64, const FSR_Message *message);
		virtual SDPState processMessage(XBeeAddress64 &addr64, const FS_Message *message);
		virtual SDPState processMessage(XBeeAddress64 &addr64, DA_Message *message);
		virtual SDPState processMessage(XBeeAddress64 &addr64, const DAR_Message *message);

	private:
		SDPState transmitPacket(XBeeAddress64 &addr64, uint8_t message[], size_t length);

		RemoteServiceRecord *findRemoteService(ServiceType sid); 		
		RemoteServiceRecord *findRemoteService(ServiceType sid, ServiceLocation sl);
		LocalServiceRecord *findLocalService(ServiceType sid, ActionType aid); 
		LocalServiceRecord *findLocalService(ServiceType sid);
		LocalServiceRecord *findLocalService(ServiceType sid, ServiceLocation sl);
		LocalServiceRecord *findLocalService(ServiceType sid, ServiceLocation sl, ActionType aid);

		ServiceCallbackRecord *findServiceCallback(ServiceType sid, ActionType aid); 
		ServiceCallbackRecord *findServiceCallback(ServiceType sid, ServiceLocation sl, ActionType aid);
		ServiceCallbackRecord *findEmptyServiceCallbackRecord();
		RemoteServiceRecord *findEmptyRemoteServiceRecord();
		LocalServiceRecord *findEmptyLocalServiceRecord();

		XBeeAddress64 findService64(ServiceType sid); 

		SDPState processMessage(XBeeAddress64 &addr64, const uint8_t *buffer, const size_t size);
		//SDPState processMessage(const XBeeAddress64 &addr64, const Message *message);		
};



#endif
