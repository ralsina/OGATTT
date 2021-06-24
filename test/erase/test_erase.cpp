#include <Arduino.h>
#include "terminal.h"
#include <unity.h>

// FIXME some tests have hardcoded screen sizes
void test_clear_whole_screen(void)
{
    Terminal term;
    // Fill the screen
    for (int i = 0; i < SCREEN_COLS * SCREEN_ROWS - 1; i++)
    {
        uint8_t c = '0' + i / SCREEN_COLS;
        term.process(c);
        TEST_ASSERT_EQUAL(c, term.screen[i % SCREEN_COLS][i / SCREEN_COLS]);
    }
    // Clear the screen
    term.process_string("\x1B[2J");
    // See it's cleared
    for (int i = 0; i < SCREEN_COLS * SCREEN_ROWS; i++)
    {
        TEST_ASSERT_EQUAL(0, term.screen[i % SCREEN_COLS][i / SCREEN_COLS]);
    }
}

void test_clear_screen_from_beginning_to_cursor(void)
{
    Terminal term;
    // Fill the screen
    for (int i = 0; i < SCREEN_COLS * SCREEN_ROWS - 1; i++)
    {
        uint8_t c = '0' + i / SCREEN_COLS;
        term.process(c);
        TEST_ASSERT_EQUAL(c, term.screen[i % SCREEN_COLS][i / SCREEN_COLS]);
    }
    term.cursor_x = 5;
    term.cursor_y = 5;
    // Clear the screen to the cursor
    term.process_string("\x1B[1J");
    for (int i = 0; i < SCREEN_COLS * SCREEN_ROWS - 1; i++)
    {
        if (i < 5 * SCREEN_COLS + 5)
        {
            TEST_ASSERT_EQUAL(0, term.screen[i % SCREEN_COLS][i / SCREEN_COLS]);
        }
        else
        {
            TEST_ASSERT_NOT_EQUAL(0, term.screen[i % SCREEN_COLS][i / SCREEN_COLS]);
        }
    }
}

void test_clear_screen_from_cursor_to_end(void)
{
    Terminal term;
    // Fill the screen
    for (int i = 0; i < SCREEN_COLS * SCREEN_ROWS - 1; i++)
    {
        uint8_t c = '0' + i / SCREEN_COLS;
        term.process(c);
        TEST_ASSERT_EQUAL(c, term.screen[i % SCREEN_COLS][i / SCREEN_COLS]);
    }
    term.cursor_x = 5;
    term.cursor_y = 5;
    // Clear the screen to the cursor
    term.process_string("\x1B[0J");
    for (int i = 0; i < SCREEN_COLS * SCREEN_ROWS - 1; i++)
    {
        if (i < 5 * SCREEN_COLS + 5)
        {
            TEST_ASSERT_NOT_EQUAL(0, term.screen[i % SCREEN_COLS][i / SCREEN_COLS]);
        }
        else
        {
            TEST_ASSERT_EQUAL(0, term.screen[i % SCREEN_COLS][i / SCREEN_COLS]);
        }
    }
}

void test_clear_whole_line(void)
{
    Terminal term;
    term.cursor_y = 4;
    term.process_string("4 Hello World");
    term.cursor_y = 3;
    term.cursor_x = 0;
    term.process_string("3 Hello World");
    term.cursor_x = 5;
    // Clear the whole line 3
    term.process_string("\x1B[2K");

    for (int x = 0; x < SCREEN_COLS; x++)
    {
        TEST_ASSERT_EQUAL(0, term.screen[x][3]);
    }
    for (int x = 0; x < 13; x++)
    {
        TEST_ASSERT_NOT_EQUAL(0, term.screen[x][4]);
    }
}

void test_clear_line_left_of_cursor(void)
{
    Terminal term;
    term.cursor_y = 4;
    term.process_string("1234567890");
    term.cursor_y = 5;
    term.cursor_x = 0;
    term.process_string("1234567890");
    // Clear first 5 chars of line 4
    term.cursor_y = 4;
    term.cursor_x = 5;
    term.process_string("\x1B[1K");

    // First 5 chars cleared
    for (int x = 0; x < 5; x++)
    {
        TEST_ASSERT_EQUAL(0, term.screen[x][4]);
    }
    // To the right, not cleared
    for (int x = 5; x < 10; x++)
    {
        TEST_ASSERT_NOT_EQUAL(0, term.screen[x][4]);
    }
    // Next row, not cleared
    for (int x = 0; x < 10; x++)
    {
        TEST_ASSERT_NOT_EQUAL(0, term.screen[x][5]);
    }
}

void test_clear_line_right_of_cursor(void)
{
    Terminal term;
    term.cursor_y = 4;
    term.process_string("1234567890");
    term.cursor_y = 5;
    term.cursor_x = 0;
    term.process_string("1234567890");
    // Clear first 5 chars of line 4
    term.cursor_y = 4;
    term.cursor_x = 5;
    term.process_string("\x1B[0K");

    // First 5 chars not cleared
    for (int x = 0; x < 5; x++)
    {
        TEST_ASSERT_NOT_EQUAL(0, term.screen[x][4]);
    }
    // To the right, cleared
    for (int x = 5; x < SCREEN_COLS; x++)
    {
        TEST_ASSERT_EQUAL(0, term.screen[x][4]);
    }
    // Next row, not cleared
    for (int x = 0; x < 10; x++)
    {
        TEST_ASSERT_NOT_EQUAL(0, term.screen[x][5]);
    }
}

void setup()
{
    UNITY_BEGIN();

    // Screen clearing
    RUN_TEST(test_clear_whole_screen);
    RUN_TEST(test_clear_screen_from_beginning_to_cursor);
    RUN_TEST(test_clear_screen_from_cursor_to_end);
    RUN_TEST(test_clear_whole_line);
    RUN_TEST(test_clear_line_left_of_cursor);
    RUN_TEST(test_clear_line_right_of_cursor);
}

void loop()
{

    UNITY_END();
}