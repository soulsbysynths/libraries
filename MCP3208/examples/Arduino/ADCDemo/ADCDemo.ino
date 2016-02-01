#include <MCP3208.h>
#include <SPI.h>
MCP3208 adc(10);

void setup() {
	adc.begin();
	Serial.begin(9600);
}

void loop() {
	char temp[10];
	static int ctr = 0;
	if (ctr == 0) {
		Serial.println();
		Serial.println("              Single Ended               |    Differential");
		Serial.println("    0    1    2    3    4    5    6    7 |    0    1    2    3");
		Serial.println("--------------------------------------------------------------");
	}
	ctr++;
	if (ctr == 10) {
		ctr = 0;
	}
	for (int i = 0; i < 8; i++) {
		sprintf(temp, "%5d", adc.analogRead(i));
		Serial.print(temp);
	}
	Serial.print(" |");
	for (int i = 0; i < 4; i++) {
		sprintf(temp, "%5d", adc.analogReadDif(i));
		Serial.print(temp);
	}
	Serial.println();
	delay(100);
}
