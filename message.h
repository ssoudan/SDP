/*
 Service Discovery Protocol - Message
 @author: Sebastien Soudan <sebastien.soudan@gmail.com>
*/

#ifndef MESSAGE_H_
#define MESSAGE_H_

//#include <XBee.h>


#ifndef ARDUINO
	#include <iostream> 
#else 
 	#include <iostream> 
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

#ifndef XBeeAddress16
 typedef uint16_t XBeeAddress16;
#endif

#ifndef XBeeAddress64
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
 		addr = (addr & 0xffffffff00000000) + lsb;
 	};

 };
 
#endif 

#define FS 	1
#define FSR 2
#define DA 	3
#define DAR 4

enum ServiceType { UNDEF_SERVICE = 0, RTC = 1, TEMPERATURE = 2 };

enum ActionType { UNDEF_ACTION = 0, GET_VALUE = 1, SET_VALUE = 2 };

enum ActionStatus { UNDEF_STATUS = 0, DONE = 1, NOT_FOUND = 2, NOT_DONE = 3 };

/**
	Generic message class
*/
class Message {
//private:
public:
	//    virtual void DoSomething();
    virtual size_t Encode(uint8_t *buffer, size_t limit) = 0;
    static Message *Decode(uint8_t *buffer, size_t size);
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
		setServiceType(UNDEF_SERVICE);
	};

    virtual size_t Encode(uint8_t *buffer, size_t limit);
    
    static FS_Message *Decode(uint8_t *buffer, size_t size);

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
		setServiceType(UNDEF_SERVICE);		
	};

	FSR_Message(ServiceType serviceType) {
		setServiceType(serviceType);		
	};

	//	virtual void DoSomething();

    virtual size_t Encode(uint8_t *buffer, size_t limit);
    static FSR_Message *Decode(uint8_t *buffer, size_t size);

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
		setServiceType(UNDEF_SERVICE);	
		setActionType(UNDEF_ACTION);
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
    static DA_Message *Decode(uint8_t *buffer, size_t size);

    inline ServiceType getServiceType() const { return serviceType; };
    inline void setServiceType(const ServiceType serviceType) { this->serviceType = serviceType; };

	inline ActionType getActionType() const { return actionType; };
    inline void setActionType(const ActionType actionType) { this->actionType = actionType; };

    inline uint8_t getActionParameterSize() const { return actionParameterSize; };
    inline void setActionParameterSize(const uint8_t actionParameterSize)  { DA_Message::actionParameterSize = actionParameterSize; };

    inline uint8_t* getActionParameter() { return actionParameter; };
    inline void setActionParameter(uint8_t* actionParameter) { 
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
		setServiceType(UNDEF_SERVICE);	
		setActionType(UNDEF_ACTION);
		setActionStatus(UNDEF_STATUS);
		setActionResultSize(0);		
	};

	DAR_Message(ServiceType serviceType, ActionType actionType, ActionStatus actionStatus) {
		setServiceType(serviceType);	
		setActionType(actionType);
		setActionStatus(actionStatus);
		setActionResultSize(0);		
	};

    virtual size_t Encode(uint8_t *buffer, size_t limit);
    static DAR_Message *Decode(uint8_t *buffer, size_t size);

    inline ServiceType getServiceType() const { return serviceType; };
    inline void setServiceType(const ServiceType serviceType) { this->serviceType = serviceType; };

	inline ActionType getActionType() const { return actionType; };
    inline void setActionType(const ActionType actionType) { this->actionType = actionType; };

	inline ActionStatus getActionStatus() const { return actionStatus; };
    inline void setActionStatus(const ActionStatus actionStatus) { this->actionStatus = actionStatus; };

	inline uint8_t getActionResultSize() const { return actionResultSize; };
    inline void setActionResultSize(const uint8_t actionResultSize) { this->actionResultSize = actionResultSize; };

	inline uint8_t* getActionResult() const { return actionResult; };
    inline void setActionResult(uint8_t* actionResult) {
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


#endif /* MESSAGE_H_ */
