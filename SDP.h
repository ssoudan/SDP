/*
 Service Discovery Protocol
*/

#ifndef SDP_H_
#define SDP_H_

//#include "Arduino.h"
#include "message.h"
#include <iostream>
using namespace std;

class SDP {
	public: 
		//SDP(Xbee xbee);
		ActionStatus doAction(ServiceType sid, ActionType actionType, 
			uint8_t actionParameterSize,
			uint8_t actionParameter[]);

		inline void processMessage(const Message *message) { cout << "message" << std::endl; };
		inline void processMessage(const FSR_Message *message) { cout << "fsr_message" << std::endl; };
		inline void processMessage(const FS_Message *message) { cout << "fs_message" << std::endl; };
		inline void processMessage(const DA_Message *message) { cout << "da_message" << std::endl; };
		inline void processMessage(const DAR_Message *message) { cout << "dar_message" << std::endl; };
	
	private:
		XBeeAddress64 findService64(ServiceType sid); 
		XBeeAddress16 findService16(ServiceType sid); 

};

#endif