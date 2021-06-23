#include <Arduino.h>
#include "terminal.h"
#include <unity.h>

void test_screen_buffer_exists(void)
{
    Terminal term;
    TEST_ASSERT_EQUAL(0, term.screen[0][0]);
}

void test_print(void)
{
    Terminal term;
    // Send characters to print, see they get printed
    for (uint8_t x = 0; x < SCREEN_COLS - 1; x++)
    {
        TEST_ASSERT_EQUAL(0, term.screen[x][0]);
        TEST_ASSERT_EQUAL(x, term.cursor_x);
        term.handle_print('a' + x);
        TEST_ASSERT_EQUAL('a' + x, term.screen[x][0]);
        TEST_ASSERT_EQUAL(0, term.cursor_y);
    }
}

void setup()
{
    UNITY_BEGIN();

    RUN_TEST(test_screen_buffer_exists);
    RUN_TEST(test_print);
}

void loop()
{

    UNITY_END();
}