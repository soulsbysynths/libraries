/* 
* OdyHpf.h
*
* Created: 29/09/2015 09:30:45
* Author: Paul Soulsby
*/


#ifndef __ODYHPF_H__
#define __ODYHPF_H__


class OdyHpf
{
//variables
public:
protected:
private:
	unsigned char fc_ = 0;
	int lpf_ = 0;
//functions
public:
	OdyHpf();
	~OdyHpf();
	void setFc(unsigned char newFc);
	unsigned char getFc(){return fc_;}	
	int processSample(int sample);
protected:
private:
	OdyHpf( const OdyHpf &c );
	OdyHpf& operator=( const OdyHpf &c );

}; //OdyHpf

#endif //__ODYHPF_H__
