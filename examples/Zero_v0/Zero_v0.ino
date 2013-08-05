// Service Discovery lib
#include <SDP.h>

// Xbee
XBee xbee = XBee();

// SDP
SDP sdp = SDP(&xbee);

// Remember to put the address of your XBee board here
XBeeAddress64 local64 = XBeeAddress64(0xbadc0de, 0xdeadbeef);

/**
    Service function to return epoch date on Jan, 1st 1970
 */
size_t getDate(uint8_t *in, size_t inLength, uint8_t *out, size_t outLimit) {
 
    Serial.println("getDate");

    if (outLimit >= 4) {

      uint32_t epoc = 0; 

      convertUint32toArray(epoc, out);
      return 4;
    }

    return 0;
}

void setup() {

  // Debugging output
  Serial.begin(9600);

  Serial.println("setup!");

  // Init xbee
  xbee.begin(Serial);
  
  sdp.setLocal64(local64);
  Serial.println("SDP init");

  // Service Discovery registration 
  RegistrationStatus res = sdp.registerService(ServiceDiscovery::SDP_RTC, ServiceDiscovery::GET_VALUE, &getDate);
  if (!res) { /* Registration has failed */ }

}

/**
 	Get packet read and processed.
	readPackets() returns a state giving some info about
	what has been done. But I removed it to save some place.	
 */
void processMessage() {
   /* SDPState state = */ sdp.readPackets();
}

/**
	Action callback.
	Executed when a reponse to a DO ACTION message has been
	sent, and the action has been successfully executed. 
 */
void actionCallback(uint8_t *data, size_t size) {	
	// do something with the data
}

void loop() {
  processMessage();

  ActionStatus as = sdp.doAction(ServiceDiscovery::TEMPERATURE, ServiceDiscovery::GET_VALUE, 0, NULL, &actionCallback);
  // flash some led according to the status in 'as'?

  // do a lot of other stuff here.

  delay(300);
}



