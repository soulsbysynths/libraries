//AteOscEngineProgmem.h  Progmem for Audio engine for Atmegatron,
//Copyright (C) 2015  Paul Soulsby info@soulsbysynths.com,
//,
//This program is free software: you can redistribute it and/or modify,
//it under the terms of the GNU General Public License as published by,
//the Free Software Foundation, either version 3 of the License, or,
//(at your option) any later version.,
//,
//This program is distributed in the hope that it will be useful,,
//but WITHOUT ANY WARRANTY; without even the implied warranty of,
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the,
//GNU General Public License for more details.,
//,
//You should have received a copy of the GNU General Public License,
//along with this program.  If not, see <http://www.gnu.org/licenses/>.,

#ifndef ATEOSCENGINEPROGMEM_H_
#define ATEOSCENGINEPROGMEM_H_
#include <avr/pgmspace.h>

static const unsigned int CV_VALS_PER_OCT = 819;

static const unsigned int ENV_ADR_INC[16] PROGMEM = {32767,4095,1424,682,372,217,133,86,57,38,26,19,13,9,7,5};
static const int ENV_S_LEVEL[16] PROGMEM = {0,2048,4352,6400,8704,10752,13056,15104,17408,19456,21760,23808,26112,28160,30464,32767};

static const unsigned int PORTA_SPEED[16] PROGMEM = {0,5,14,27,46,77,123,195,306,477,740,1145,1768,2728,4207,6484};

static const unsigned int CV_TO_FREQ[CV_VALS_PER_OCT] PROGMEM = {
	16744,
	16758,
	16772,
	16786,
	16800,
	16815,
	16829,
	16843,
	16857,
	16872,
	16886,
	16900,
	16914,
	16929,
	16943,
	16957,
	16972,
	16986,
	17001,
	17015,
	17029,
	17044,
	17058,
	17073,
	17087,
	17101,
	17116,
	17130,
	17145,
	17159,
	17174,
	17189,
	17203,
	17218,
	17232,
	17247,
	17261,
	17276,
	17291,
	17305,
	17320,
	17335,
	17349,
	17364,
	17379,
	17393,
	17408,
	17423,
	17438,
	17452,
	17467,
	17482,
	17497,
	17512,
	17526,
	17541,
	17556,
	17571,
	17586,
	17601,
	17616,
	17630,
	17645,
	17660,
	17675,
	17690,
	17705,
	17720,
	17735,
	17750,
	17765,
	17780,
	17795,
	17810,
	17825,
	17841,
	17856,
	17871,
	17886,
	17901,
	17916,
	17931,
	17947,
	17962,
	17977,
	17992,
	18007,
	18023,
	18038,
	18053,
	18068,
	18084,
	18099,
	18114,
	18130,
	18145,
	18160,
	18176,
	18191,
	18207,
	18222,
	18237,
	18253,
	18268,
	18284,
	18299,
	18315,
	18330,
	18346,
	18361,
	18377,
	18392,
	18408,
	18424,
	18439,
	18455,
	18470,
	18486,
	18502,
	18517,
	18533,
	18549,
	18564,
	18580,
	18596,
	18612,
	18627,
	18643,
	18659,
	18675,
	18690,
	18706,
	18722,
	18738,
	18754,
	18770,
	18786,
	18801,
	18817,
	18833,
	18849,
	18865,
	18881,
	18897,
	18913,
	18929,
	18945,
	18961,
	18977,
	18993,
	19009,
	19026,
	19042,
	19058,
	19074,
	19090,
	19106,
	19122,
	19139,
	19155,
	19171,
	19187,
	19203,
	19220,
	19236,
	19252,
	19269,
	19285,
	19301,
	19318,
	19334,
	19350,
	19367,
	19383,
	19399,
	19416,
	19432,
	19449,
	19465,
	19482,
	19498,
	19515,
	19531,
	19548,
	19564,
	19581,
	19597,
	19614,
	19631,
	19647,
	19664,
	19680,
	19697,
	19714,
	19730,
	19747,
	19764,
	19781,
	19797,
	19814,
	19831,
	19848,
	19865,
	19881,
	19898,
	19915,
	19932,
	19949,
	19966,
	19983,
	19999,
	20016,
	20033,
	20050,
	20067,
	20084,
	20101,
	20118,
	20135,
	20152,
	20169,
	20186,
	20204,
	20221,
	20238,
	20255,
	20272,
	20289,
	20306,
	20324,
	20341,
	20358,
	20375,
	20392,
	20410,
	20427,
	20444,
	20462,
	20479,
	20496,
	20514,
	20531,
	20548,
	20566,
	20583,
	20601,
	20618,
	20635,
	20653,
	20670,
	20688,
	20705,
	20723,
	20740,
	20758,
	20776,
	20793,
	20811,
	20828,
	20846,
	20864,
	20881,
	20899,
	20917,
	20934,
	20952,
	20970,
	20988,
	21005,
	21023,
	21041,
	21059,
	21077,
	21094,
	21112,
	21130,
	21148,
	21166,
	21184,
	21202,
	21220,
	21238,
	21256,
	21274,
	21292,
	21310,
	21328,
	21346,
	21364,
	21382,
	21400,
	21418,
	21436,
	21454,
	21473,
	21491,
	21509,
	21527,
	21545,
	21564,
	21582,
	21600,
	21618,
	21637,
	21655,
	21673,
	21692,
	21710,
	21729,
	21747,
	21765,
	21784,
	21802,
	21821,
	21839,
	21858,
	21876,
	21895,
	21913,
	21932,
	21950,
	21969,
	21987,
	22006,
	22025,
	22043,
	22062,
	22081,
	22099,
	22118,
	22137,
	22156,
	22174,
	22193,
	22212,
	22231,
	22249,
	22268,
	22287,
	22306,
	22325,
	22344,
	22363,
	22382,
	22401,
	22420,
	22439,
	22458,
	22477,
	22496,
	22515,
	22534,
	22553,
	22572,
	22591,
	22610,
	22629,
	22648,
	22668,
	22687,
	22706,
	22725,
	22744,
	22764,
	22783,
	22802,
	22821,
	22841,
	22860,
	22879,
	22899,
	22918,
	22938,
	22957,
	22976,
	22996,
	23015,
	23035,
	23054,
	23074,
	23093,
	23113,
	23133,
	23152,
	23172,
	23191,
	23211,
	23231,
	23250,
	23270,
	23290,
	23309,
	23329,
	23349,
	23369,
	23388,
	23408,
	23428,
	23448,
	23468,
	23488,
	23507,
	23527,
	23547,
	23567,
	23587,
	23607,
	23627,
	23647,
	23667,
	23687,
	23707,
	23727,
	23747,
	23767,
	23788,
	23808,
	23828,
	23848,
	23868,
	23888,
	23909,
	23929,
	23949,
	23969,
	23990,
	24010,
	24030,
	24051,
	24071,
	24091,
	24112,
	24132,
	24153,
	24173,
	24194,
	24214,
	24235,
	24255,
	24276,
	24296,
	24317,
	24337,
	24358,
	24379,
	24399,
	24420,
	24440,
	24461,
	24482,
	24503,
	24523,
	24544,
	24565,
	24586,
	24606,
	24627,
	24648,
	24669,
	24690,
	24711,
	24732,
	24753,
	24774,
	24795,
	24816,
	24837,
	24858,
	24879,
	24900,
	24921,
	24942,
	24963,
	24984,
	25005,
	25026,
	25048,
	25069,
	25090,
	25111,
	25133,
	25154,
	25175,
	25196,
	25218,
	25239,
	25260,
	25282,
	25303,
	25325,
	25346,
	25368,
	25389,
	25411,
	25432,
	25454,
	25475,
	25497,
	25518,
	25540,
	25561,
	25583,
	25605,
	25626,
	25648,
	25670,
	25692,
	25713,
	25735,
	25757,
	25779,
	25800,
	25822,
	25844,
	25866,
	25888,
	25910,
	25932,
	25954,
	25976,
	25998,
	26020,
	26042,
	26064,
	26086,
	26108,
	26130,
	26152,
	26174,
	26196,
	26219,
	26241,
	26263,
	26285,
	26308,
	26330,
	26352,
	26374,
	26397,
	26419,
	26441,
	26464,
	26486,
	26509,
	26531,
	26554,
	26576,
	26598,
	26621,
	26644,
	26666,
	26689,
	26711,
	26734,
	26757,
	26779,
	26802,
	26825,
	26847,
	26870,
	26893,
	26915,
	26938,
	26961,
	26984,
	27007,
	27030,
	27052,
	27075,
	27098,
	27121,
	27144,
	27167,
	27190,
	27213,
	27236,
	27259,
	27282,
	27305,
	27329,
	27352,
	27375,
	27398,
	27421,
	27444,
	27468,
	27491,
	27514,
	27537,
	27561,
	27584,
	27607,
	27631,
	27654,
	27678,
	27701,
	27724,
	27748,
	27771,
	27795,
	27818,
	27842,
	27866,
	27889,
	27913,
	27936,
	27960,
	27984,
	28007,
	28031,
	28055,
	28079,
	28102,
	28126,
	28150,
	28174,
	28198,
	28222,
	28245,
	28269,
	28293,
	28317,
	28341,
	28365,
	28389,
	28413,
	28437,
	28461,
	28485,
	28510,
	28534,
	28558,
	28582,
	28606,
	28630,
	28655,
	28679,
	28703,
	28727,
	28752,
	28776,
	28800,
	28825,
	28849,
	28874,
	28898,
	28923,
	28947,
	28972,
	28996,
	29021,
	29045,
	29070,
	29094,
	29119,
	29144,
	29168,
	29193,
	29218,
	29242,
	29267,
	29292,
	29317,
	29342,
	29366,
	29391,
	29416,
	29441,
	29466,
	29491,
	29516,
	29541,
	29566,
	29591,
	29616,
	29641,
	29666,
	29691,
	29716,
	29742,
	29767,
	29792,
	29817,
	29842,
	29868,
	29893,
	29918,
	29944,
	29969,
	29994,
	30020,
	30045,
	30071,
	30096,
	30121,
	30147,
	30172,
	30198,
	30224,
	30249,
	30275,
	30300,
	30326,
	30352,
	30377,
	30403,
	30429,
	30455,
	30480,
	30506,
	30532,
	30558,
	30584,
	30610,
	30636,
	30661,
	30687,
	30713,
	30739,
	30765,
	30791,
	30818,
	30844,
	30870,
	30896,
	30922,
	30948,
	30974,
	31001,
	31027,
	31053,
	31079,
	31106,
	31132,
	31158,
	31185,
	31211,
	31238,
	31264,
	31290,
	31317,
	31343,
	31370,
	31397,
	31423,
	31450,
	31476,
	31503,
	31530,
	31556,
	31583,
	31610,
	31637,
	31663,
	31690,
	31717,
	31744,
	31771,
	31798,
	31825,
	31851,
	31878,
	31905,
	31932,
	31959,
	31986,
	32014,
	32041,
	32068,
	32095,
	32122,
	32149,
	32177,
	32204,
	32231,
	32258,
	32286,
	32313,
	32340,
	32368,
	32395,
	32422,
	32450,
	32477,
	32505,
	32532,
	32560,
	32587,
	32615,
	32643,
	32670,
	32698,
	32726,
	32753,
	32781,
	32809,
	32837,
	32864,
	32892,
	32920,
	32948,
	32976,
	33004,
	33032,
	33060,
	33088,
	33116,
	33144,
	33172,
	33200,
	33228,
	33256,
	33284,
	33312,
	33341,
	33369,
	33397,
	33425,
	33454
};
#endif /* ATEOSCENGINEPROGMEM_H_ */