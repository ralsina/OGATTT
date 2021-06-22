#include <Arduino.h>
#include <unity.h>

void test_unit_testing(void) {
    TEST_ASSERT_EQUAL(1,1);
}

void setup()
{
    delay(2000);

    UNITY_BEGIN();

    pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
    RUN_TEST(test_unit_testing);

    UNITY_END();
}