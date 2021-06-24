#include <Arduino.h>
#include "terminal.h"
#include <unity.h>

void test_backspace(void)
{
    // BS moves the cursor to the left one character position,
    // unless it is at the left margin, in which case no action occurs.

    Terminal term;
    term.cursor_x = 4;
    term.cursor_y = 5;

    term.process(8);
    term.process(8);

    TEST_ASSERT_EQUAL(2, term.cursor_x);
    TEST_ASSERT_EQUAL(5, term.cursor_y);

    term.process(8);
    term.process(8);
    TEST_ASSERT_EQUAL(0, term.cursor_x);
    TEST_ASSERT_EQUAL(5, term.cursor_y);

    term.process(8);
    term.process(8);
    TEST_ASSERT_EQUAL(0, term.cursor_x);
    TEST_ASSERT_EQUAL(5, term.cursor_y);
}

void setup()
{
    UNITY_BEGIN();

    // Basic print support
    RUN_TEST(test_backspace);
}

void loop()
{
    UNITY_END();
}