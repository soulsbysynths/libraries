/*
 * Copyright (c) 2014, Majenko Technologies
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 * 
 *  1. Redistributions of source code must retain the above copyright notice, 
 *     this list of conditions and the following disclaimer.
 * 
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 * 
 *  3. Neither the name of Majenko Technologies nor the names of its contributors may be used
 *     to endorse or promote products derived from this software without 
 *     specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include <MCP3208.h>

void MCP3208::begin() {
    pinMode(_cs, OUTPUT);
    digitalWrite(_cs, HIGH);
#ifdef __PIC32MX__
    _spi->begin();
#else
    SPI.begin();
#endif
}

uint8_t MCP3208::spiTransfer(uint8_t i) {
    uint8_t d;
#ifdef __PIC32MX__
    d = _spi->transfer(i);
#else
    d = SPI.transfer(i);
#endif
    return d;
}

uint16_t MCP3208::analogRead(uint8_t pin) {
    uint8_t addr = 0b01100000 | ((pin & 0b111) << 2);
    digitalWrite(_cs, LOW);
    (void) spiTransfer(addr);
    uint8_t b1 = spiTransfer(0);
    uint8_t b2 = spiTransfer(0);
    digitalWrite(_cs, HIGH);

    return (b1 << 4) | (b2 >> 4);
}

int16_t MCP3208::analogReadDif(uint8_t pin) {
    uint8_t diff;
    uint8_t b1, b2;
    uint8_t addr = 0b01000000 | ((pin & 0b11) << 3);
    digitalWrite(_cs, LOW);
    (void) spiTransfer(addr);
    b1 = spiTransfer(0);
    b2 = spiTransfer(0);
    digitalWrite(_cs, HIGH);

    diff = (b1 << 4) | (b2 >> 4);
    if (diff > 0) {
        return diff;
    }
    addr = 0b01000100 | ((pin & 0b11) << 3);
    digitalWrite(_cs, LOW);
    (void) spiTransfer(addr);
    b1 = spiTransfer(0);
    b2 = spiTransfer(0);
    digitalWrite(_cs, HIGH);
    diff = (b1 << 4) | (b2 >> 4);
    return -diff;
}
