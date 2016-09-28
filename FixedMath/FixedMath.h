/*
* FixedMath.h
*
* Created: 04/05/2016 11:55:18
*  Author: paulsoulsby
*/


#ifndef FIXEDMATH_H_
#define FIXEDMATH_H_

#include <avr/pgmspace.h>
//10 bit version.
static const int SINE[0x101] PROGMEM = {0,25,50,75,101,126,151,176,201,226,251,276,301,326,351,376,401,426,451,476,501,526,551,576,601,626,651,675,700,725,750,774,799,824,848,873,897,922,946,971,995,1020,1044,1068,1092,1117,1141,1165,1189,1213,1237,1261,1285,1309,1332,1356,1380,1404,1427,1451,1474,1498,1521,1544,1567,1591,1614,1637,1660,1683,1706,1729,1751,1774,1797,1819,1842,1864,1886,1909,1931,1953,1975,1997,2019,2041,2062,2084,2106,2127,2149,2170,2191,2213,2234,2255,2276,2296,2317,2338,2359,2379,2399,2420,2440,2460,2480,2500,2520,2540,2559,2579,2598,2618,2637,2656,2675,2694,2713,2732,2751,2769,2788,2806,2824,2843,2861,2878,2896,2914,2932,2949,2967,2984,3001,3018,3035,3052,3068,3085,3102,3118,3134,3150,3166,3182,3198,3214,3229,3244,3260,3275,3290,3305,3320,3334,3349,3363,3378,3392,3406,3420,3433,3447,3461,3474,3487,3500,3513,3526,3539,3551,3564,3576,3588,3600,3612,3624,3636,3647,3659,3670,3681,3692,3703,3713,3724,3734,3745,3755,3765,3775,3784,3794,3803,3812,3822,3831,3839,3848,3857,3865,3873,3881,3889,3897,3905,3912,3920,3927,3934,3941,3948,3954,3961,3967,3973,3979,3985,3991,3996,4002,4007,4012,4017,4022,4027,4031,4036,4040,4044,4048,4052,4055,4059,4062,4065,4068,4071,4074,4076,4079,4081,4083,4085,4087,4088,4090,4091,4092,4093,4094,4095,4095,4096,4096,4096};
static const int TAN[0x101] PROGMEM = {0,25,50,75,101,126,151,176,201,226,252,277,302,327,353,378,403,429,454,480,505,531,556,582,608,633,659,685,711,737,763,789,815,841,867,893,920,946,973,999,1026,1053,1080,1107,1134,1161,1188,1215,1243,1270,1298,1325,1353,1381,1409,1437,1466,1494,1523,1551,1580,1609,1638,1667,1697,1726,1756,1786,1816,1846,1876,1907,1937,1968,1999,2030,2062,2093,2125,2157,2189,2222,2254,2287,2320,2354,2387,2421,2455,2489,2524,2559,2594,2629,2665,2701,2737,2773,2810,2847,2885,2922,2961,2999,3038,3077,3116,3156,3197,3237,3278,3320,3362,3404,3446,3490,3533,3577,3622,3667,3712,3758,3805,3852,3900,3948,3997,4046,4096,4147,4198,4250,4302,4355,4409,4464,4519,4575,4632,4690,4748,4808,4868,4929,4991,5054,5118,5183,5249,5315,5383,5453,5523,5594,5667,5741,5816,5892,5970,6049,6130,6212,6296,6381,6468,6557,6647,6740,6834,6930,7028,7128,7230,7335,7442,7551,7663,7777,7895,8015,8137,8263,8392,8525,8660,8800,8943,9089,9240,9396,9555,9720,9889,10063,10242,10427,10618,10816,11019,11230,11448,11673,11906,12148,12399,12659,12929,13210,13503,13807,14124,14455,14801,15162,15540,15937,16352,16789,17247,17731,18240,18779,19348,19951,20592,21273,22000,22775,23606,24497,25457,26492,27613,28831,30158,31612,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767};

static const unsigned int FIXEDMATH_SCALE = 1024;
static const unsigned char FIXEDMATH_BS = 10;

inline
int fsin(unsigned int x)  //0-1023
{
	unsigned int i = (unsigned int)(x & 0xFF);
	if (x < 0x100)
	{
		return (int)pgm_read_word(&(SINE[i]));
	}
	else if (x < 0x200)
	{
		i = (unsigned int)(0x100 - i);
		return (int)pgm_read_word(&(SINE[i]));
	}
	else if (x < 0x300)
	{
		return (int)-pgm_read_word(&(SINE[i]));
	}
	else
	{
		i = (unsigned int)(0x100 - i);
		return ( int)-pgm_read_word(&(SINE[i]));
	}
}
inline
int fcos(unsigned int x)
{
	x += 0x100;
	x &= 0x3FF;
	return fsin(x);
}
inline
int ftan(unsigned int x)
{
	unsigned int i = (unsigned int)(x & 0xFF);
	if (x < 0x100 || (x >= 0x200 && x < 0x300))
	{
		return pgm_read_word(&(TAN[i]));
	}
	else
	{
		i = (unsigned int)(0x100 - i);
		return ( int)-pgm_read_word(&(TAN[i]));
	}
}
#endif /* FIXEDMATH_H_ */