/* 
* CircularBuffer.h
*
* Created: 13/11/2016 14:08:48
* Author: paulsoulsby
*/


#ifndef __CIRCULARBUFFER_H__
#define __CIRCULARBUFFER_H__

#include <string.h>

class CircularBuffer
{
//variables
public:
protected:
private:
	unsigned char * buffer_ = NULL;
	unsigned char mask_ = 255;
	unsigned int size_ = 256;
	unsigned char readPos_ = mask_;
	unsigned char writePos_ = 0;
//functions
public:
	CircularBuffer() {}
	CircularBuffer(unsigned int bufferSize)
	{
		size_ = bufferSize;
		mask_ = bufferSize - 1;  //powers of 2 only
		readPos_ = mask_;
		buffer_ = new unsigned char[bufferSize]; 
	}
	~CircularBuffer(){}
	inline 
	unsigned int getSize()
	{
		return size_;
	}
	inline 
	unsigned char getMask()
	{
		return mask_;
	}
	inline 
	bool isAvailable()
	{
		unsigned char nextPos = (readPos_+1) & mask_;
		if(nextPos==writePos_)
		{
			return false;
		}
		else
		{
			readPos_ = nextPos;
		}
	}
	inline
	unsigned char read()
	{
		return buffer_[readPos_];
	}
	inline
	bool isFull()
	{
		unsigned char nextPos = (writePos_+1) & mask_;
		if(nextPos==readPos_)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	inline 
	void write(unsigned char data)
	{
		buffer_[writePos_] = data;
		writePos_++;
		writePos_ &= mask_;
	}
protected:
private:
	//CircularBuffer( const CircularBuffer &c );
	//CircularBuffer& operator=( const CircularBuffer &c );

}; //CircularBuffer

#endif //__CIRCULARBUFFER_H__
