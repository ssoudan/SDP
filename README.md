	   _____ ____  ____ 
	  / ___// __ \/ __ \
	  \__ \/ / / / /_/ /
	 ___/ / /_/ / ____/ 
	/____/_____/_/      ...
		Service Discovery Protocol for Embedded devices.


Sebastien Soudan <sebastien.soudan@gmail.com> 2013

APLv2

A Service Discovery Protocol for Arduino-Xbee based sensor networks.

===============
Principle
===

* One board register an action it can offer on a service
* Another board want this action to be executed but have 
  no clue on who is offering this service
* This second guy, send a broadcast message to find the 
  service: (FS message)
* The first guy, or maybe another one, or both respond 
  with a 'find service' response message (FSR) giving 
  their addresses.
* Next time first want to execute the action, he can 
  just send directly a 'do action' message (DA) in 
  unicast with the parameter payload and with get a 
  response (or not) from the guy targeted by the message.
  Upon the reception of this 'do action' response message
  (DAR), the callback he has provided with the doAction, 
  will be executed on the result of the execution of the
  remote method that has been packed in the DAR message
  together with the status in case this one is DONE.

Note that there are some 'corner' cases that are not yet supported by this lib:
* the address of the first guy that replied to the FS will not be overriden by the 
  one of the second guy (provider as well) which will get a seat in the 
  Remote Service Directory but never be called because the search method sucks.
* In case the first guy disappear, there is no timeout mechanism to remove the entry.
  Though in case of an error during the execution of an action (NOT_DONE status), 
  the entry is removed.

===============
To Do: 
===
* extract XBee from the stuff
* add timeout mechanism
* refine service/action model (single callback for an entire service?)
* add location concept to the service
* improve the way the xmit status is checked
* use it

===============
Getting started
===

* Get Arduino-Makefile [https://github.com/sudar/Arduino-Makefile] from Sudar.

My tree looks like something like:

	.../Arduino-Makefile/...
	
	.../libraries/AdafruitLCD
	.../libraries/AdafruitMPL115A2
	.../libraries/AdafruitRGBLCDShieldLibraryMaster
	.../libraries/RTClib
	.../libraries/SDP 						<- the lib we are talking about
	.../libraries/SDP/examples/Zero_v0/ 	<- the example thereafter
	.../libraries/SDP/examples/Zero_v0/Makefile 	<- [ the makefile next 
 	.../libraries/TFTLCD							   [ bullet point is about
	.../libraries/TKLCD
	.../libraries/TinkerKit
	.../libraries/USB_Host_Shield_20
	.../libraries/XBee						<- one the lib dependencies
	
	
	.../Uno_v0/Uno_v0.ino

* Create your own project Makefile similar to this one:

```
	#
	# Service Discovery Protocol - Makefile
	# @author: Sebastien Soudan <sebastien.soudan@gmail.com>
	#
	ARDMK_DIR=../../../../Arduino-Makefile/
	ARDUINO_DIR=/Applications/Arduino-1.0.5.app/Contents/Resources/Java

	# if you want some debug info (beware of the total size in RAM):
	#CPPFLAGS=-DDEBUG 

	BOARD_TAG = uno 

	MONITOR_PORT = /dev/cu.usb*

	ARDUINO_LIBS = AdafruitRGBLCDShieldLibraryMaster \
				   AdafruitMPL115A2 \
				   RTClib \
				   SDP \
				   XBee \
				   Wire \
				   Wire/utility

	include $(ARDMK_DIR)/arduino-mk/Arduino.mk
```

* Create your own code but here is an example:
```
	#include <SDP.h> // Service Discovery lib
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
	  RegistrationStatus res = sdp.registerService(ServiceDiscovery::SDP_RTC, 
	  											 ServiceDiscovery::GET_VALUE, 
	  											 &getDate);
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

	  ActionStatus as = sdp.doAction(ServiceDiscovery::TEMPERATURE, 
									 ServiceDiscovery::GET_VALUE, 
									 0, 
									 NULL, 
									 &actionCallback);
	  // flash some led according to the status in 'as'?

	  // do a lot of other stuff here.
	  delay(300);
	}
```

	                    

