/*
 Service Discovery Protocol - Message
 @author: Sebastien Soudan <sebastien.soudan@gmail.com>
*/

#ifndef MESSAGE_H_
 #define MESSAGE_H_

#include "sdp-config.h"
#include "enum.h"

#if defined(ARDUINO)
 	#include <XBee.h>
  	#include <stdint.h>
 	#include <string.h>
 	#include "new.h"
#ifndef size_t
 #define size_t uint8_t
#endif

#endif

#define FS 	1
#define FSR 2
#define DA 	3
#define DAR 4

using namespace ServiceDiscovery;

#define FS_MESSAGE_SIZE 3
#define FSR_MESSAGE_SIZE (1 + 1 + 1 + 4 + 4)
#define DA_MESSAGE_BASE_SIZE 5
#define DAR_MESSAGE_BASE_SIZE 6

/**
	Generic message class
*/
class Message {
public:
    virtual size_t Encode(uint8_t *buffer, size_t limit) = 0;
    static Message *Decode(const uint8_t *buffer, const size_t size);

	virtual ServiceDiscovery::SDPState process(class MessageProcessor *sdp, XBeeAddress64 &addr64) = 0;

	virtual ~Message() {};

};

/**
	Find Service Message
*/
class FS_Message : public Message {
private:
	ServiceType serviceType;	
	ServiceLocation serviceLocation;

public:

	FS_Message() {
		setServiceType(ServiceDiscovery::UNDEF_SERVICE);
		setServiceLocation(ServiceDiscovery::UNDEF_LOCATION);
	};

	FS_Message(ServiceType sid) {
		setServiceType(sid);
		setServiceLocation(ServiceDiscovery::UNDEF_LOCATION);
	};

	FS_Message(ServiceType sid, ServiceLocation sl) {
		setServiceType(sid);
		setServiceLocation(sl);
	};


    virtual size_t Encode(uint8_t *buffer, size_t limit);
    
    static FS_Message *Decode(const uint8_t *buffer, const size_t size);

	virtual ServiceDiscovery::SDPState process(class MessageProcessor *sdp, XBeeAddress64 &addr64);

    inline ServiceType getServiceType() const {return serviceType; };
    inline void setServiceType(const ServiceType serviceType) { this->serviceType = serviceType; };
    inline ServiceLocation getServiceLocation() const {return serviceLocation; };
    inline void setServiceLocation(const ServiceLocation serviceLocation) { this->serviceLocation = serviceLocation; };
};


/**
	Find Service Response Message
*/
class FSR_Message : public Message {
private:
	ServiceType serviceType;
	ServiceLocation serviceLocation;

	XBeeAddress64 address64;

public:
	FSR_Message() {
		setServiceType(ServiceDiscovery::UNDEF_SERVICE);		
		setServiceLocation(ServiceDiscovery::UNDEF_LOCATION);
	};

	FSR_Message(ServiceType serviceType) {
		setServiceType(serviceType);		
		setServiceLocation(ServiceDiscovery::UNDEF_LOCATION);
	};

	FSR_Message(ServiceType serviceType, ServiceLocation sl) {
		setServiceType(serviceType);		
		setServiceLocation(sl);
	};

    virtual size_t Encode(uint8_t *buffer, size_t limit);
    static FSR_Message *Decode(const uint8_t *buffer, const size_t size);

	virtual ServiceDiscovery::SDPState process(class MessageProcessor *sdp, XBeeAddress64 &addr64);

    inline ServiceType getServiceType() const { return serviceType; };
    inline void setServiceType(const ServiceType serviceType) { this->serviceType = serviceType; } ;

    inline ServiceLocation getServiceLocation() const { return serviceLocation; };
    inline void setServiceLocation(const ServiceLocation serviceLocation) { this->serviceLocation = serviceLocation; } ;

    inline XBeeAddress64 getAddress64() const { return address64; };
    inline void setAddress64(const XBeeAddress64 addr) { this->address64 = addr; };
};


/**
	Do Action Message
*/
class DA_Message : public Message {
private:
	ServiceType serviceType;
	ServiceLocation serviceLocation;
	ActionType actionType;
	uint8_t actionParameterSize;
	uint8_t* actionParameter;

public:
	//    virtual void DoSomething();
	DA_Message() {
		setServiceType(ServiceDiscovery::UNDEF_SERVICE);	
		setServiceLocation(ServiceDiscovery::UNDEF_LOCATION);	
		setActionType(ServiceDiscovery::UNDEF_ACTION);
		setActionParameterSize(0);		
		actionParameter = NULL;
	};

	DA_Message(ServiceType serviceType, ActionType actionType) {
		setServiceType(serviceType);	
		setServiceLocation(ServiceDiscovery::UNDEF_LOCATION);	
		setActionType(actionType);
		setActionParameterSize(0);		
		actionParameter = NULL;
	};

	DA_Message(ServiceType serviceType, ActionType actionType, ServiceLocation sl) {
		setServiceType(serviceType);	
		setServiceLocation(sl);	
		setActionType(actionType);
		setActionParameterSize(0);		
		actionParameter = NULL;
	};


    virtual size_t Encode(uint8_t *buffer, size_t limit);
    static DA_Message *Decode(const uint8_t *buffer, const size_t size);

	virtual ServiceDiscovery::SDPState process(class MessageProcessor *sdp, XBeeAddress64 &addr64);

    inline ServiceType getServiceType() const { return serviceType; };
    inline void setServiceType(const ServiceType serviceType) { this->serviceType = serviceType; };

    inline ServiceLocation getServiceLocation() const { return serviceLocation; };
    inline void setServiceLocation(const ServiceLocation serviceLocation) { this->serviceLocation = serviceLocation; };

	inline ActionType getActionType() const { return actionType; };
    inline void setActionType(const ActionType actionType) { this->actionType = actionType; };

    inline uint8_t getActionParameterSize() const { return actionParameterSize; };
    inline void setActionParameterSize(const uint8_t actionParameterSize)  { DA_Message::actionParameterSize = actionParameterSize; };

    inline uint8_t* getActionParameter() { return actionParameter; };
    inline void setActionParameter(const uint8_t* actionParameter) { 
		if (this->actionParameterSize <= 0) {
			return;
		}

		this->actionParameter = new uint8_t[actionParameterSize];
		memcpy(this->actionParameter, actionParameter, actionParameterSize);
	};

    ~DA_Message() { 
    	if (actionParameter != NULL) {
    		delete[] actionParameter;
    	}
    };
};


/**
	Do Action Response Message
*/
class DAR_Message : public Message {
private:
	ServiceType serviceType;
	ServiceLocation serviceLocation;
	ActionType actionType;
	ActionStatus actionStatus;
	uint8_t actionResultSize;
	uint8_t* actionResult;

public:
	DAR_Message() {
		setServiceType(ServiceDiscovery::UNDEF_SERVICE);	
		setServiceLocation(ServiceDiscovery::UNDEF_LOCATION);	
		setActionType(ServiceDiscovery::UNDEF_ACTION);
		setActionStatus(ServiceDiscovery::UNDEF_STATUS);
		setActionResultSize(0);		
		actionResult = NULL;
	};

	DAR_Message(ServiceType serviceType, ActionType actionType, ActionStatus actionStatus) {
		setServiceType(serviceType);	
		setServiceLocation(ServiceDiscovery::UNDEF_LOCATION);	
		setActionType(actionType);
		setActionStatus(actionStatus);
		setActionResultSize(0);		
		actionResult = NULL;
	};

	DAR_Message(ServiceType serviceType, ActionType actionType, ActionStatus actionStatus, ServiceLocation sl) {
		setServiceType(serviceType);	
		setServiceLocation(sl);	
		setActionType(actionType);
		setActionStatus(actionStatus);
		setActionResultSize(0);		
		actionResult = NULL;
	};
	
    virtual size_t Encode(uint8_t *buffer, size_t limit);
    static DAR_Message *Decode(const uint8_t *buffer, const size_t size);

	virtual ServiceDiscovery::SDPState process(class MessageProcessor *sdp, XBeeAddress64 &addr64);

    inline ServiceType getServiceType() const { return serviceType; };
    inline void setServiceType(const ServiceType serviceType) { this->serviceType = serviceType; };

    inline ServiceLocation getServiceLocation() const { return serviceLocation; };
    inline void setServiceLocation(const ServiceLocation serviceLocation) { this->serviceLocation = serviceLocation; };

	inline ActionType getActionType() const { return actionType; };
    inline void setActionType(const ActionType actionType) { this->actionType = actionType; };

	inline ActionStatus getActionStatus() const { return actionStatus; };
    inline void setActionStatus(const ActionStatus actionStatus) { this->actionStatus = actionStatus; };

	inline uint8_t getActionResultSize() const { return actionResultSize; };
    inline void setActionResultSize(const uint8_t actionResultSize) { this->actionResultSize = actionResultSize; };

	inline uint8_t* getActionResult() const { return actionResult; };
    inline void setActionResult(const uint8_t* actionResult) {
    	if (actionResultSize <= 0) {
			return;
		}

		this->actionResult = new uint8_t[actionResultSize];
		memcpy(this->actionResult, actionResult, actionResultSize);
	};

	~DAR_Message() { 
    	if (actionResult != NULL) {
    		delete[] actionResult;
    	}
    };
};



class MessageProcessor {
public:		
	virtual SDPState processMessage(XBeeAddress64 &addr64, const FSR_Message *message) = 0;
	virtual SDPState processMessage(XBeeAddress64 &addr64, const FS_Message *message) = 0;
	virtual SDPState processMessage(XBeeAddress64 &addr64, DA_Message *message) = 0;
	virtual SDPState processMessage(XBeeAddress64 &addr64, const DAR_Message *message) = 0;
};

#endif /* MESSAGE_H_ */
