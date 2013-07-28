/*
 Service Discovery Protocol
 @author: Sebastien Soudan <sebastien.soudan@gmail.com>
*/

#include "SDP.h"


Record *SDP::findService(ServiceType sid) {

	for (int i = 0 ; i < LSD_SIZE ; i++) {		
		if (lsd[i].sid == sid)
			return &lsd[i];
	}

	return NULL;
};