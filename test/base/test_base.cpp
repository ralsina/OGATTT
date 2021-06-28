#include <Arduino.h>
#include "terminal.h"
#include "kbd.h"
#include "screen.h"
#include <unity.h>

// FIXME some tests have hardcoded screen sizes

Terminal term;
FakeKeyboard kbd;

#ifndef NATIVE
#include "ssd1306_screen.h"
SSD1306Screen scr;
#else
Screen scr;
#endif

void test_screen_buffer_exists(void)
{
    term.init(kbd, scr);
    TEST_ASSERT_EQUAL(0, scr._screen[0][0]);
}

void test_print(void)
{
    term.init(kbd, scr);
    // Send characters to print, see they get printed
    for (uint8_t x = 1; x < SCREEN_COLS; x++)
    {
        TEST_ASSERT_EQUAL(0, scr._screen[x][1]);
        TEST_ASSERT_EQUAL(x, term.cursor_x);
        term.handle_print('a' + x);
        TEST_ASSERT_EQUAL('a' + x, scr._screen[x][1]);
        TEST_ASSERT_EQUAL(1, term.cursor_y);
    }
}

void test_print_follows_cursor(void)
{
    term.init(kbd, scr);
    term.cursor_x = 10;
    term.cursor_y = 5;
    TEST_ASSERT_EQUAL(0, scr._screen[10][5]);
    term.handle_print('a');
    TEST_ASSERT_EQUAL('a', scr._screen[10][5]);
    TEST_ASSERT_EQUAL(11, term.cursor_x);
}

void test_process_printable(void)
{
    term.init(kbd, scr);
    // Send characters to print, see they get printed
    for (uint8_t x = 1; x < SCREEN_COLS; x++)
    {
        TEST_ASSERT_EQUAL(0, scr._screen[x][1]);
        TEST_ASSERT_EQUAL(x, term.cursor_x);
        term.process('a' + x);
        TEST_ASSERT_EQUAL('a' + x, scr._screen[x][1]);
        TEST_ASSERT_EQUAL(1, term.cursor_y);
    }
}

void test_overflow_wraps(void)
{
    // Printing more than SCREEN_COLS chars
    // wraps to next line
    term.init(kbd, scr);
    for (int i = 0; i < SCREEN_COLS + 5; i++)
    {
        term.process('X');
    }
    TEST_ASSERT_EQUAL(6, term.cursor_x);
    TEST_ASSERT_EQUAL(2, term.cursor_y);
    TEST_ASSERT_EQUAL('X', scr._screen[4][1]);
}

void test_scroll(void)
{
    term.init(kbd, scr);
    for (int i = 1; i <= SCREEN_ROWS; i++)
    {
        term.cursor_x = 1;
        term.cursor_y = i;
        term.handle_print('0' + i);
    }
    TEST_ASSERT_EQUAL_STRING_LEN("12345678", scr._screen[1] + 1, 8);
    scr.scroll(3);
    TEST_ASSERT_EQUAL_STRING_LEN("45678", scr._screen[1] + 1, 8);
    scr.scroll(0);
    TEST_ASSERT_EQUAL_STRING_LEN("45678", scr._screen[1] + 1, 8);
    scr.scroll(1);
    TEST_ASSERT_EQUAL_STRING_LEN("5678", scr._screen[1] + 1, 8);
    scr.scroll(100);
    TEST_ASSERT_EQUAL_STRING_LEN("", scr._screen[1] + 1, 8);
}

void test_fill_screen(void)
{
    term.init(kbd, scr);
    scr.clear();
    // Screen filled like this (missing one char in the lower left
    // corner to avoid scrolling):
    // 000000000
    // 111111111
    // 222222222
    // ...
    // 77777777

    for (int i = 0; i < SCREEN_COLS * SCREEN_ROWS - 1; i++)
    {

        int y = i / SCREEN_COLS;
        char c = '0' + y;
        term.process(c);
    }
    // Compares per column
    for (int i = 1; i < SCREEN_COLS; i++)
    {
        TEST_ASSERT_EQUAL_STRING_LEN("01234567", scr._screen[i] + 1, 8);
    }
    TEST_ASSERT_EQUAL_STRING_LEN("0123456", scr._screen[SCREEN_COLS] + 1, 7);
}

void test_clear(void)
{
    term.init(kbd, scr);
    term.process_string("\x1b#8"); // Fills screen with E
    // Compares per column
    for (int i = 1; i <= SCREEN_COLS; i++)
    {
        TEST_ASSERT_EQUAL_STRING_LEN("EEEEEEEE", scr._screen[i] + 1, 8);
    }
}

void test_nlm(void)
{
    term.init(kbd, scr);
    TEST_ASSERT_EQUAL(false, term.lnm);
    term.process_string("\033[20h");
    TEST_ASSERT_EQUAL(true, term.lnm);
    term.process_string("\033[20l");
    TEST_ASSERT_EQUAL(false, term.lnm);
}

void setup()
{
    UNITY_BEGIN();

    // Basic print support
    RUN_TEST(test_screen_buffer_exists);
    RUN_TEST(test_print);
    RUN_TEST(test_print_follows_cursor);
    RUN_TEST(test_process_printable);

    // Screen handling
    RUN_TEST(test_scroll);
    RUN_TEST(test_overflow_wraps);
    RUN_TEST(test_clear);
    RUN_TEST(test_fill_screen);
}

void loop()
{
    UNITY_END();
}