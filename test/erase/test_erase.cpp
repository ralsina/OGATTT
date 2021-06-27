#include <Arduino.h>
#include "terminal.h"
#include <unity.h>

Terminal term;

// FIXME some tests have hardcoded screen sizes
void test_clear_whole_screen(void)
{
    term.init();
    // Fill the screen
    term.process_string("\033#8");
    // Clear the screen
    term.process_string("\x1B[2J");
    // See it's cleared
    for (int i = 0; i < SCREEN_COLS * SCREEN_ROWS; i++)
    {
        TEST_ASSERT_EQUAL(0, term.screen[i % SCREEN_COLS + 1][i / SCREEN_COLS + 1]);
    }
}

void test_clear_screen_from_beginning_to_cursor(void)
{
    term.init();
    // Fill the screen
    term.process_string("\033#8");
    term.cursor_x = 5;
    term.cursor_y = 5;
    // Clear the screen to the cursor
    term.process_string("\x1B[1J");
    for (int i = 0; i < SCREEN_COLS * SCREEN_ROWS - 1; i++)
    {
        if (i < 4 * SCREEN_COLS + 5)
        {
            TEST_ASSERT_EQUAL(0, term.screen[i % SCREEN_COLS + 1][i / SCREEN_COLS + 1]);
        }
        else
        {
            TEST_ASSERT_NOT_EQUAL(0, term.screen[i % SCREEN_COLS + 1][i / SCREEN_COLS + 1]);
        }
    }
}

void test_clear_screen_from_cursor_to_end(void)
{
    term.init();
    // Fill the screen
    term.process_string("\033#8");
    term.cursor_x = 5;
    term.cursor_y = 5;
    // Clear the screen to the cursor
    term.process_string("\x1B[0J");
    for (int i = 0; i < SCREEN_COLS * SCREEN_ROWS - 1; i++)
    {
        if (i < 4 * SCREEN_COLS + 4)
        {
            TEST_ASSERT_NOT_EQUAL(0, term.screen[i % SCREEN_COLS + 1][i / SCREEN_COLS + 1]);
        }
        else
        {
            TEST_ASSERT_EQUAL(0, term.screen[i % SCREEN_COLS + 1][i / SCREEN_COLS + 1]);
        }
    }
}

void test_clear_whole_line(void)
{
    term.init();
    term.cursor_y = 4;
    term.process_string("4 Hello World");
    term.cursor_y = 3;
    term.cursor_x = 1;
    term.process_string("3 Hello World");
    term.cursor_x = 5;
    // Clear the whole line 3
    term.process_string("\x1B[2K");

    // Line 3 is empty
    for (int x = 1; x <= SCREEN_COLS; x++)
    {
        TEST_ASSERT_EQUAL(0, term.screen[x][3]);
    }
    // And line 4 is not
    for (int x = 1; x <= 13; x++)
    {
        TEST_ASSERT_NOT_EQUAL(0, term.screen[x][4]);
    }
}

void test_clear_line_left_of_cursor(void)
{
    term.init();
    // Fill the screen
    term.process_string("\033#8");
    // Clear first 5 chars of line 4
    term.cursor_y = 4;
    term.cursor_x = 5;
    term.process_string("\x1B[1K");

    // First 5 chars cleared
    for (int x = 1; x <= 5; x++)
    {
        TEST_ASSERT_EQUAL(0, term.screen[x][4]);
    }
    // To the right, not cleared
    for (int x = 6; x <= SCREEN_COLS; x++)
    {
        TEST_ASSERT_NOT_EQUAL(0, term.screen[x][4]);
    }
    // Next row, not cleared
    for (int x = 1; x <= SCREEN_COLS; x++)
    {
        TEST_ASSERT_NOT_EQUAL(0, term.screen[x][5]);
    }
}

void test_clear_line_right_of_cursor(void)
{
    term.init();
    // Fill the screen
    term.process_string("\033#8");
    // Clear chars of line 4 from column 5
    term.cursor_y = 4;
    term.cursor_x = 5;
    term.process_string("\x1B[0K");

    // First 4 chars not cleared
    for (int x = 1; x < 4; x++)
    {
        TEST_ASSERT_NOT_EQUAL(0, term.screen[x][4]);
    }
    // To the right, cleared
    for (int x = 5; x <= SCREEN_COLS; x++)
    {
        TEST_ASSERT_EQUAL(0, term.screen[x][4]);
    }
    // Next row, not cleared
    for (int x = 1; x <= SCREEN_COLS; x++)
    {
        TEST_ASSERT_NOT_EQUAL(0, term.screen[x][5]);
    }
}

void setup()
{
    UNITY_BEGIN();

    // Screen clearing
    RUN_TEST(test_clear_whole_screen);
    RUN_TEST(test_clear_screen_from_cursor_to_end);
    RUN_TEST(test_clear_whole_line);
    RUN_TEST(test_clear_line_left_of_cursor);
    RUN_TEST(test_clear_line_right_of_cursor);
    RUN_TEST(test_clear_screen_from_beginning_to_cursor);
}   
 

void loop()
{

    UNITY_END();
}