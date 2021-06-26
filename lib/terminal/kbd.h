#ifndef KBD__H
#define KBD__H

#include <stdint.h>

class Keyboard
{
public:
    Keyboard(){};
    void init(void);
    const char *get_key(void);

    int initial_delay = 500; // Time between 1st press and 1st repeat
    int repeat_delay = 33;   // Time between repeats
};

#endif // KBD__H