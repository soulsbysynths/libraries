MCP3208
=======

Arduino library for MCP3208 8 channel, 12 bit, ADC chip

Usage
-----

The library provides a constructor which takes a chip select pin number.
To use the library you must first create a new MCP3208 object with the
pin number.  On the chipKIT boards you also get the option of passing a
DSPI object to use a different SPI bus than the default:

Arduino or chipKIT:

    MCP3208 myADC(10);

chipKIT only:

    DSPI1 mySPI;
    MCP3208 myADC(&mySPI, 10);

You can also miss out all parameters and the default chip select pin
of 10 will be used.

Reading the ADC values can be done in two ways.  The chip supports both
normal linear analog reading (as the on-board ADC on Arduino and chipKIT
boards provides), and also differential reading; that is the difference
between the voltages applied to two adjacent channels.

To do a normal straight reading use the analogRead() member function of
the MCP3208 object.  For instance, to read from channel 3:

    int myValue = myADC.analogRead(3);

For differential reading channels are numbered 0 to 3 instead of 0 to 7
and form the single-ended pairs 0+1, 2+3, 4+5 and 6+7. The analogReadDif()
member function is used to read a differential value, so to read the
voltage difference between inputs 4 and 5, you would use:

    int myValue = myADC.analogReadDif(2);

Differential reading is done in either one or two stages depending on the
voltages applied.  If the voltage difference is positive then the channels
are read simultaneously and the value returned.  However, if the difference
is negative the first reading will return an invalid value, so the channel
assignments are then swapped and a new reading taken.  The negative of this
new value is then returned, thus giving you the full positive and negative 
difference ranges.

For more detail on how the chip works you should consult the datasheet:

http://ww1.microchip.com/downloads/en/DeviceDoc/21298c.pdf
