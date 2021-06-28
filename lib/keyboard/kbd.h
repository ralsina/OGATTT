#ifndef KBD__H
#define KBD__H

#include <stdint.h>

class Keyboard
{
public:
    Keyboard(){};
    void init(void);
    uint8_t get_key(void);

    // Time between 1st press and 1st repeat (half a second)
    unsigned long initial_delay = 500000;
    // Time between repeats (33Hz)
    unsigned long repeat_delay = 33333;

    long int last_emitted_time = 0;
    uint8_t last_emitted_keycode = 0;
    bool repeating = false;
};

// Test fake
class FakeKeyboard : public Keyboard
{
    uint8_t get_key(void) { return 255; };
};

#endif // KBD__H