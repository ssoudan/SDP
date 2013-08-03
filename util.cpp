/*
 Service Discovery Protocol - Util
 @author: Sebastien Soudan <sebastien.soudan@gmail.com>
*/
#include "util.h"

void convertUint32toArray(const uint32_t value, uint8_t byteArray[]) { 
	byteArray[0] = (uint8_t)((value >> 24) & 0xFF) ;
 	byteArray[1] = (uint8_t)((value >> 16) & 0xFF) ;
 	byteArray[2] = (uint8_t)((value >> 8) & 0XFF);
 	byteArray[3] = (uint8_t)((value & 0XFF));
}


void convertUint16toArray(const uint16_t value, uint8_t byteArray[]) { 
 	byteArray[0] = (uint8_t)((value >> 8) & 0XFF);
 	byteArray[1] = (uint8_t)((value & 0XFF));
}


uint16_t convertArraytoUint16(const uint8_t byteArray[]) { 

  uint16_t value;

  value = ( (byteArray[0] << 8) 
            + (byteArray[1] ) );

  return value;
};

uint32_t convertArraytoUint32(const uint8_t byteArray[]) { 

  uint32_t value;

  value = (((uint32_t) byteArray[0] << 24) 
  			+ ((uint32_t) byteArray[1] << 16) 
        + ((uint32_t) byteArray[2] << 8) 
        + ((uint32_t) byteArray[3] ));

  return value;
};