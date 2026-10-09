#ifndef EINKDISPLAYFONTS_h
#define EINKDISPLAYFONTS_h

#ifdef USE_EINK

#ifdef ARDUINO
#include <Arduino.h>
#elif __MBED__
#define PROGMEM
#endif

/**
 * Monospaced Plain 30
 */
extern const uint8_t Monospaced_plain_30[] PROGMEM;
#ifdef OLED_UA
extern const uint8_t Monospaced_plain_30_UA[] PROGMEM;
#endif

#endif // USE_EINK

#endif
