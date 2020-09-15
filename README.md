# Dimmer-on-Arduino
That`s a dimmer with timer, screen, and different options.
/******************************************************************
 * All what you need for this dimmer are:                         *
 *                                                                *
 * TFT Display ST7789                                             *
 * RTC module DS1302                                              *
 * AC Dimmer https://arduino.ua/prod2958-dimmer-220v-dlya-arduino *
 * 3 Buttons                                                      *
 *                                                                *
 * The schematic of connection:                                   *
 *                                                                *
 * 1)TFT Display ST7789:                                          *
 *    GND - GND                                                   *
 *    VCC - 5V                                                    *
 *    SCL - 13 pin                                                *
 *    SDA - 11 pin                                                *
 *    RES - 9 pin                                                 *
 *    DC - 8 pin                                                  *
 * 2)RTC module DS1302:                                           *                                                               
 *    GND - GND                                                   *
 *    VCC - 5V                                                    *
 *    RST - 7                                                     *
 *    DAT - 10                                                    *
 *    CLK -12                                                     *
 * 3)AC Dimmer:                                                   *       
 *    GND - GND                                                   *
 *    VCC - 5V                                                    *
 *    m1 - 3                                                      *
 *    m2 - 2                                                      *
 * 4)3 Buttons                                                    *
 *   The first one to the 4 pin (Left button)                     *                                       
 *   The second one to the 5 pin (OK button)                      *
 *   The last one to the 6 pin (Right button)                     *
 *                                                                *
 ******************************************************************/
