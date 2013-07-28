/*
 Service Discovery Protocol - Util
 @author: Sebastien Soudan <sebastien.soudan@gmail.com>
*/
#include <stdint.h>

void convertUint32toArray(const uint32_t value, uint8_t byteArray[]);

void convertUint16toArray(const uint16_t value, uint8_t byteArray[]);

uint16_t convertArraytoUint16(const uint8_t byteArray[]);

uint32_t convertArraytoUint32(const uint8_t byteArray[]);