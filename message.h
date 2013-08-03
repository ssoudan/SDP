/*
 Service Discovery Protocol - Message
 @author: Sebastien Soudan <sebastien.soudan@gmail.com>
*/

#ifndef MESSAGE_H_
#define MESSAGE_H_

#include "enum.h"

#if defined(ARDUINO)
 	#include <XBee.h>
  	#include <stdint.h>
 	#include <string.h>
 	#include "new.h"
 	#define ASSERT(x) {}
 	//#include <stdlib.h>
	//#include <stdint.h>
#endif

#ifndef ARDUINO 
#define ERROR(x) do { printf(x); printf("\n"); } while (0)
#else
#define ERROR(x) // Nothing
#endif

#ifndef size_t
 #define size_t uint8_t
#endif

typedef uint16_t XBeeAddress16;

#ifndef XBee_h

 class XBeeAddress64 {
 private:
 	uint64_t addr;
 public:
 	XBeeAddress64() {
 		addr = 0;
 	};

 	inline uint32_t getMsb() const {
 		return (uint32_t) (addr >> 32);
 	};

	inline uint32_t getLsb() const {
 		return (uint32_t) (addr & 0xffffffff);
 	};

 	inline void setMsb(const uint32_t msb) {
 		addr = (addr & 0xffffffff) + ((uint64_t) msb << 32);
 	};

 	inline void setLsb(const uint32_t lsb) {
 		addr = (addr & 0xffffffff00000000ll) + lsb;
 	};

 };
 
#endif 

#define FS 	1
#define FSR 2
#define DA 	3
#define DAR 4

using namespace ServiceDiscovery;

/**
	Generic message class
*/
class Message {
//private:
public:
	//    virtual void DoSomething();
    virtual size_t Encode(uint8_t *buffer, size_t limit) = 0;
    static Message *Decode(const uint8_t *buffer, const size_t size);

	virtual ServiceDiscovery::SDPState process(class MessageProcessor *sdp, XBeeAddress64 &addr64) = 0;
};

/**
	Find Service Message
*/
class FS_Message : public Message {
private:
	ServiceType serviceType;

public:
	//    virtual void DoSomething();
	
	FS_Message() {
		setServiceType(ServiceDiscovery::UNDEF_SERVICE);
	};

	FS_Message(ServiceType sid) {
		setServiceType(sid);
	};


    virtual size_t Encode(uint8_t *buffer, size_t limit);
    
    static FS_Message *Decode(const uint8_t *buffer, const size_t size);

	virtual ServiceDiscovery::SDPState process(class MessageProcessor *sdp, XBeeAddress64 &addr64);

    inline ServiceType getServiceType() const {return serviceType; };
    inline void setServiceType(const ServiceType serviceType) { this->serviceType = serviceType; };
};


/**
	Find Service Response Message
*/
class FSR_Message : public Message {
private:
	ServiceType serviceType;

	XBeeAddress64 address64;
	XBeeAddress16 address16;

public:
	FSR_Message() {
		setServiceType(ServiceDiscovery::UNDEF_SERVICE);		
	};

	FSR_Message(ServiceType serviceType) {
		setServiceType(serviceType);		
	};

	//	virtual void DoSomething();

    virtual size_t Encode(uint8_t *buffer, size_t limit);
    static FSR_Message *Decode(const uint8_t *buffer, const size_t size);

	virtual ServiceDiscovery::SDPState process(class MessageProcessor *sdp, XBeeAddress64 &addr64);

    inline ServiceType getServiceType() const { return serviceType; };
    inline void setServiceType(const ServiceType serviceType) { this->serviceType = serviceType; } ;

    inline XBeeAddress64 getAddress64() const { return address64; };
    inline void setAddress64(const XBeeAddress64 addr) { this->address64 = addr; };

    inline XBeeAddress16 getAddress16() const { return address16; };
    inline void setAddress16(const XBeeAddress16 addr) { this->address16 = addr; };
};


/**
	Do Action Message
*/
class DA_Message : public Message {
private:
	ServiceType serviceType;
	ActionType actionType;
	uint8_t actionParameterSize;
	uint8_t* actionParameter;

public:
	//    virtual void DoSomething();
	DA_Message() {
		setServiceType(ServiceDiscovery::UNDEF_SERVICE);	
		setActionType(ServiceDiscovery::UNDEF_ACTION);
		setActionParameterSize(0);		
		setActionParameter(NULL);
	};

	DA_Message(ServiceType serviceType, ActionType actionType) {
		setServiceType(serviceType);	
		setActionType(actionType);
		setActionParameterSize(0);		
		setActionParameter(NULL);
	};

    virtual size_t Encode(uint8_t *buffer, size_t limit);
    static DA_Message *Decode(const uint8_t *buffer, const size_t size);

	virtual ServiceDiscovery::SDPState process(class MessageProcessor *sdp, XBeeAddress64 &addr64);

    inline ServiceType getServiceType() const { return serviceType; };
    inline void setServiceType(const ServiceType serviceType) { this->serviceType = serviceType; };

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
	ActionType actionType;
	ActionStatus actionStatus;
	uint8_t actionResultSize;
	uint8_t* actionResult;

public:
	//    virtual void DoSomething();
	DAR_Message() {
		setServiceType(ServiceDiscovery::UNDEF_SERVICE);	
		setActionType(ServiceDiscovery::UNDEF_ACTION);
		setActionStatus(ServiceDiscovery::UNDEF_STATUS);
		setActionResultSize(0);		
	};

	DAR_Message(ServiceType serviceType, ActionType actionType, ActionStatus actionStatus) {
		setServiceType(serviceType);	
		setActionType(actionType);
		setActionStatus(actionStatus);
		setActionResultSize(0);		
	};
	
    virtual size_t Encode(uint8_t *buffer, size_t limit);
    static DAR_Message *Decode(const uint8_t *buffer, const size_t size);

	virtual ServiceDiscovery::SDPState process(class MessageProcessor *sdp, XBeeAddress64 &addr64);

    inline ServiceType getServiceType() const { return serviceType; };
    inline void setServiceType(const ServiceType serviceType) { this->serviceType = serviceType; };

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
