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

void test_tab(void)
{
    // HT moves cursor_x to next multiple of 8

    Terminal term;
    term.cursor_x = 4;

    term.process(9);
    TEST_ASSERT_EQUAL(7, term.cursor_x);
    term.process(9);
    TEST_ASSERT_EQUAL(15, term.cursor_x);
    term.process(9);
    TEST_ASSERT_EQUAL(23, term.cursor_x);
    term.process(9);
    TEST_ASSERT_EQUAL(24, term.cursor_x);
    term.process(9);
    TEST_ASSERT_EQUAL(24, term.cursor_x);
}

void setup()
{
    UNITY_BEGIN();

    // Control characters that move the cursor
    RUN_TEST(test_backspace);
    RUN_TEST(test_tab);
}

void loop()
{
    UNITY_END();
}