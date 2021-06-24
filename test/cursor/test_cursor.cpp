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

void test_lf(void)
{
    // LF VT and FF All should do the same
    // FIXME: this doesn't take "New Line Mode" into account yet
    Terminal term;
    term.cursor_x = 4;
    term.cursor_y = 2;
    term.process(10);
    term.process(11);
    term.process(12);
    TEST_ASSERT_EQUAL(5, term.cursor_y);

    // FIXME
    // When scrolling down too much, the screen should scroll up
    // and cursor_y be bound to the screen
    term.cursor_y = SCREEN_ROWS - 1;
    term.process(10);
    term.process(10);
    term.process(10);
    // TEST_ASSERT_EQUAL(SCREEN_ROWS - 1, term.cursor_y);
}

void test_CUB(void)
{
    // Cursor Backwards
    Terminal term;
    term.cursor_x = 10;
    term.process_string("\x1B[3D"); // 3 back
    TEST_ASSERT_EQUAL(7, term.cursor_x);
    term.process_string("\x1B[D"); // 0 back means 1 back
    TEST_ASSERT_EQUAL(6, term.cursor_x);
    term.process_string("\x1B[1D"); // 1 back
    TEST_ASSERT_EQUAL(5, term.cursor_x);
    // Doesn't go left of 0
    term.process_string("\x1B[9D"); // 9 back
    TEST_ASSERT_EQUAL(0, term.cursor_x);
}

void test_CUF(void)
{
    // Cursor Forward
    Terminal term;
    term.cursor_x = 10;
    term.process_string("\x1B[3C"); // 3 right
    TEST_ASSERT_EQUAL(13, term.cursor_x);
    term.process_string("\x1B[C"); // 0 right means 1 right
    TEST_ASSERT_EQUAL(14, term.cursor_x);
    term.process_string("\x1B[1C"); // 1 right
    TEST_ASSERT_EQUAL(15, term.cursor_x);
    // Doesn't go right on SCREEN_COLS - 1
    term.process_string("\x1B[99C"); // 9 right
    TEST_ASSERT_EQUAL(SCREEN_COLS - 1, term.cursor_x);
}

void test_CUD(void)
{
    // Cursor Down
    Terminal term;
    term.cursor_y = 2;
    term.process_string("\x1B[3B"); // 3 down
    TEST_ASSERT_EQUAL(5, term.cursor_y);
    term.process_string("\x1B[B"); // 0 down means 1 down
    TEST_ASSERT_EQUAL(6, term.cursor_y);
    term.process_string("\x1B[1B"); // 1 down
    TEST_ASSERT_EQUAL(7, term.cursor_y);
    // Doesn't go more than SCREEN_ROWS - 1
    term.process_string("\x1B[9B"); // 9 down
    TEST_ASSERT_EQUAL(SCREEN_ROWS -1, term.cursor_y);
}

void test_CUU(void)
{
    // Cursor Up
    Terminal term;
    term.cursor_y = 6;
    term.process_string("\x1B[3A"); // 3 up
    TEST_ASSERT_EQUAL(3, term.cursor_y);
    term.process_string("\x1B[A"); // 0 up means 1 up
    TEST_ASSERT_EQUAL(2, term.cursor_y);
    term.process_string("\x1B[1A"); // 1 up
    TEST_ASSERT_EQUAL(1, term.cursor_y);
    // Doesn't go less than 0
    term.process_string("\x1B[9A"); // 9 down
    TEST_ASSERT_EQUAL(0, term.cursor_y);
}

void test_CUP(void)
{
    // Cursor Up
    Terminal term;
    term.cursor_x = 5;
    term.cursor_y = 5;
    
    // No params, sets x, y to 0
    term.process_string("\x1B[H"); // Nothing means 0, 0
    TEST_ASSERT_EQUAL(0, term.cursor_x);
    TEST_ASSERT_EQUAL(0, term.cursor_y);

    // Two params, sets x, y
    term.process_string("\x1B[3;4H"); // x=4, y=3
    TEST_ASSERT_EQUAL(4, term.cursor_x);
    TEST_ASSERT_EQUAL(3, term.cursor_y);

    // Single param sets y
    term.process_string("\x1B[5;H"); // x=0, y=5
    TEST_ASSERT_EQUAL(0, term.cursor_x);
    TEST_ASSERT_EQUAL(5, term.cursor_y);

    // This is still a single param, so just setting y
    term.process_string("\x1B[;5H"); 
    TEST_ASSERT_EQUAL(0, term.cursor_x);
    TEST_ASSERT_EQUAL(5, term.cursor_y);
}

void test_DECSC_DECRC(void)
{
    Terminal term;
    term.cursor_x = 3;
    term.cursor_y = 4;
    term.process_string("\0337");
    TEST_ASSERT_EQUAL(3, term.saved_cursor_x);
    TEST_ASSERT_EQUAL(4, term.saved_cursor_y);
    term.cursor_x = 20;
    term.cursor_y = 2;
    term.process_string("\0338");
    TEST_ASSERT_EQUAL(3, term.cursor_x);
    TEST_ASSERT_EQUAL(4, term.cursor_y);
}

void test_IND(void)
{
    Terminal term;
    // Fill screen with Es
    term.process_string("\033#8");
    // repeat IND SCREEN_ROWS times
    for (int i=0; i<SCREEN_ROWS; i++)
    {
        TEST_ASSERT_EQUAL(i, term.cursor_y);
        term.process_string("\033D");
        TEST_ASSERT_EQUAL(i+1, term.cursor_y);
    }
    // Cursor should be in last row
    // Last row should be blank because we scrolled
    TEST_ASSERT_EQUAL(0, term.screen[0][SCREEN_ROWS-1]);
}

void setup()
{
    UNITY_BEGIN();

    // Control characters that move the cursor
    RUN_TEST(test_backspace);
    RUN_TEST(test_tab);
    RUN_TEST(test_lf);
    RUN_TEST(test_CUB);
    RUN_TEST(test_CUD);
    RUN_TEST(test_CUF);
    RUN_TEST(test_CUP);
    RUN_TEST(test_DECSC_DECRC);
    RUN_TEST(test_IND);
}

void loop()
{
    UNITY_END();
}