/*
 Service Discovery Protocol - Message
 @author: Sebastien Soudan <sebastien.soudan@gmail.com>
*/

#ifndef ENUM_H_
#define ENUM_H_

#include "sdp-config.h"

namespace ServiceDiscovery {

typedef enum { UNDEF_LOCATION = 0, TERRACE = 1, CELLAR = 2, ROOM_1 = 3} ServiceLocation;

typedef enum { UNDEF_SERVICE = 0, SDP_RTC = 1, TEMPERATURE = 2, PRESSURE = 3, FAN = 4, HUMIDITY = 5 } ServiceType;

typedef enum { UNDEF_ACTION = 0, GET_VALUE = 1, SET_VALUE = 2 , GET_STATE = 3, START = 4, STOP = 5, AUTO = 6} ActionType;

typedef enum { UNDEF_STATUS = 0, DONE = 1, NOT_FOUND = 2, NOT_DONE = 3, REQUESTED = 4, SEARCHING = 5, ACTION_ERROR = 6 } ActionStatus;

typedef enum {UNKNOWN_STATE = 0, UNKNOWN_MESSAGE, FSR_RECEIVED, DA_RECEIVED, DAR_RECEIVED, FS_RECEIVED, 
	XBEE_ASSOCIATED, XBEE_DISASSOCIATED, XBEE_UNKNOWN_MODEM_STATUS, XBEE_UNEXPECTED_MSG, XBEE_ERROR, XBEE_SUCCESS, XBEE_NOT_DELIVERED, XBEE_NO_DATA } SDPState;

typedef enum {REGISTRATION_FAILED = 0, REGISTRATION_SUCCESS = 1} RegistrationStatus;

}

#endif
