/*
 Service Discovery Protocol - Stream class for local use
 @author: Sebastien Soudan <sebastien.soudan@gmail.com>
*/

#ifndef STREAM_H_
#define STREAM_H_

#include <stdint.h>

class Stream {
public:
	virtual ~Stream() {};

	virtual uint8_t read() = 0;
	virtual bool available() = 0;
	virtual void flush() = 0;
	virtual void write(uint8_t val)  = 0;
};

#endif