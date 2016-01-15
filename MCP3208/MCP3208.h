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


#ifndef _MCP3208_H
#define _MCP3208_H

#if (ARDUINO >= 100) 
# include <Arduino.h>
#else
# include <WProgram.h>
#endif

#ifdef __PIC32MX__
#include <DSPI.h>
#else
#include <SPI.h>
#endif

class MCP3208 {
    private:
        // Private functions and variables here.  They can only be accessed
        // by functions within the class.
        uint8_t _cs;
#ifdef __PIC32MX__
        DSPI *_spi;
#endif
        uint8_t spiTransfer(uint8_t);

    public:
        // Public functions and variables.  These can be accessed from
        // outside the class.
#ifdef __PIC32MX__
        MCP3208() : _spi(new DSPI0), _cs(10) {}
        MCP3208(uint8_t cs) : _spi(new DSPI0), _cs(cs) {}
        MCP3208(DSPI *dspi) : _spi(dspi), _cs(10) {}
        MCP3208(DSPI *dspi, uint8_t cs) : _spi(dspi), _cs(cs) {}
#else
        MCP3208() : _cs(10) {}
        MCP3208(uint8_t cs) : _cs(cs) {}
#endif
    
        void begin();
        uint16_t analogRead(uint8_t pin);
        int16_t analogReadDif(uint8_t pin);
};
#endif
