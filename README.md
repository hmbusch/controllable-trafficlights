# controllable-trafficlights

In my workplace, neat little USB traffic lights appeared in several places some time ago. They serve as universal indicators for whatever someone wants to monitor, e.g. the status of a build or the health of a server. The offer an easy situation summary with just a single glance.

I thought about getting one as well but the premade ones are rather pricey (50+ euros) and they are, well, premade. So where is the fun in that? So I decided to make my own. The traffic light itself is based on the Arduino microcontroller and uses RGB LEDs for displaying the colors (I use the of them with each color wired together). Incidentally, this also allows me to display even more colors than just red, yellow and green.

## circuit

The circuit is pretty simple, it just uses N-MOSFETS (I use 2N7000 ones) to sink the LED current and switch each color with one pin. So you basically have on of these layouts for each color:

                           VCC
                            +
                            |
                           .-.
                           | |
                           | |
                           '-'
                            |
                            V -> LED
                            -
                            |
                         ||-+
                         ||<-  2N7000
             PIN ---o----||-+
                    |       |
                   .-.      |
                   | |      |
                   | |      |
                   '-'      |
                    |       |
                    o-------'
                    |
                   ===
                   GND
(created by AACircuit v1.28.6 beta 04/19/05 www.tech-chat.de)

I additionally put a speaker on pin 8 (with an appropriately sized resistor to limit the current to something below 20mA) so that a status change can also be heard without looking at the lights.
