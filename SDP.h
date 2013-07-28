/*
 Service Discovery Protocol
 @author: Sebastien Soudan <sebastien.soudan@gmail.com>
*/

#ifndef SDP_H_
#define SDP_H_

//#include "Arduino.h"
#include "message.h"

#ifndef ARDUINO
#include <iostream>
using namespace std;
#endif

#define LSD_SIZE 20

typedef struct {
	ServiceType sid;
	XBeeAddress64 address64;
	XBeeAddress16 address16;	
} Record;

class SDP {
	Record lsd[LSD_SIZE];
	uint8_t lsdCount;

	public: 
		SDP() {
			lsdCount = 0;
	
			for (int i = 0 ; i < LSD_SIZE ; i++) {
				Record record = { UNDEF_SERVICE, XBeeAddress64(), 0};
				lsd[i] = record;
			}
		};

		//SDP(Xbee xbee);
		ActionStatus doAction(ServiceType sid, ActionType actionType, 
			uint8_t actionParameterSize,
			uint8_t actionParameter[]);

		inline void processMessage(const Message *message) { 
#ifndef ARDUINO
			cout << "message" << std::endl; 
#endif
		};
		inline void processMessage(const FSR_Message *message) { 
#ifndef ARDUINO
			cout << "fsr_message" << std::endl; 
#endif
		};
		inline void processMessage(const FS_Message *message) { 
#ifndef ARDUINO			
			cout << "fs_message" << std::endl; 
#endif
		};
		inline void processMessage(const DA_Message *message) { 
#ifndef ARDUINO
			cout << "da_message" << std::endl; 
#endif		
		};
		inline void processMessage(const DAR_Message *message) { 
#ifndef ARDUINO
			cout << "dar_message" << std::endl; 
#endif
		};
	
	private:
		Record *findService(ServiceType sid); 
		XBeeAddress64 findService64(ServiceType sid); 
		XBeeAddress16 findService16(ServiceType sid); 

};

#endif