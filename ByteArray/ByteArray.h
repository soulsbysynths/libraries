/*
 * ByteArray.h
 *
 * Created: 13/01/2017 13:56:22
 *  Author: paulsoulsby
 */ 


#ifndef BYTEARRAY_H_
#define BYTEARRAY_H_

inline
void convIntToByteArray(int i, unsigned char * uc)
{
    uc[0] = (unsigned int)i >> 8;
    uc[1] = (unsigned int)i & 0xFF;	
}

inline 
int convByteArrayToInt(unsigned char * uc)
{
    unsigned int i = uc[0] << 8;
    i |= uc[1];
	return (int)i;
}

inline
void convUintToByteArray(unsigned int ui, unsigned char * uc)
{
    uc[0] = ui >> 8;
    uc[1] = ui & 0xFF;
}

inline
unsigned int convByteArrayToUint(unsigned char * uc)
{
    unsigned int ui = uc[0] << 8;
    ui |= uc[1];
    return ui;
}

#endif /* BYTEARRAY_H_ */