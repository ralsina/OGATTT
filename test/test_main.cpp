#include <Arduino.h>
#include "terminal.h"
#include <unity.h>

// FIXME some tests have hardcoded screen sizes

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

void test_print_follows_cursor(void)
{
    Terminal term;
    term.cursor_x = 10;
    term.cursor_y = 5;
    TEST_ASSERT_EQUAL(0, term.screen[10][5]);
    term.handle_print('a');
    TEST_ASSERT_EQUAL('a', term.screen[10][5]);
    TEST_ASSERT_EQUAL(11, term.cursor_x);
}

void test_process_printable(void)
{
    Terminal term;
    // Send characters to print, see they get printed
    for (uint8_t x = 0; x < SCREEN_COLS - 1; x++)
    {
        TEST_ASSERT_EQUAL(0, term.screen[x][0]);
        TEST_ASSERT_EQUAL(x, term.cursor_x);
        term.process('a' + x);
        TEST_ASSERT_EQUAL('a' + x, term.screen[x][0]);
        TEST_ASSERT_EQUAL(0, term.cursor_y);
    }
}

void test_overflow_wraps(void)
{
    // Printing more than SCREEN_COLS chars
    // wraps to next line
    Terminal term;
    for (int i = 0; i < SCREEN_COLS + 5; i++)
    {
        term.process('X');
    }
    TEST_ASSERT_EQUAL(5, term.cursor_x);
    TEST_ASSERT_EQUAL(1, term.cursor_y);
    TEST_ASSERT_EQUAL('X', term.screen[4][1]);
}

void test_scroll(void)
{
    Terminal term;
    for (int i = 0; i < SCREEN_ROWS; i++)
    {
        term.cursor_x = 0;
        term.cursor_y = i;
        term.handle_print('0' + i);
    }
    TEST_ASSERT_EQUAL_STRING_LEN("01234567", term.screen, 8);
    term.scroll(3);
    TEST_ASSERT_EQUAL_STRING_LEN("34567", term.screen, 8);
    term.scroll(0);
    TEST_ASSERT_EQUAL_STRING_LEN("34567", term.screen, 8);
    term.scroll(1);
    TEST_ASSERT_EQUAL_STRING_LEN("4567", term.screen, 8);
    term.scroll(100);
    TEST_ASSERT_EQUAL_STRING_LEN("", term.screen, 8);
}

void test_fill_screen(void)
{
    Terminal term;
    // Screen filled like this:
    // 111111111
    // 222222222
    // ...
    // 777777777
    // 000000000

    for (int i = 0; i < SCREEN_COLS * SCREEN_ROWS - 1; i++)
    {

        int y = i / SCREEN_COLS;
        char c = '0' + y;
        term.process(c);
    }
    // Compares per column
    for (int i = 0; i < SCREEN_COLS - 1; i++)
    {
        TEST_ASSERT_EQUAL_STRING_LEN("012345670", term.screen[i], 8);
    }
    TEST_ASSERT_EQUAL_STRING_LEN("0123456", term.screen[SCREEN_COLS - 1], 8);
}

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

    // Basic print support
    RUN_TEST(test_screen_buffer_exists);
    RUN_TEST(test_print);
    RUN_TEST(test_print_follows_cursor);
    RUN_TEST(test_process_printable);

    // Screen handling
    RUN_TEST(test_scroll);
    RUN_TEST(test_fill_screen);
    RUN_TEST(test_overflow_wraps);

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