#include <unity.h>
#include <time.h>

// Helper logic under test (matches TimeManager logic)
float convertTimeToDecimal(const struct tm* timeinfo) {
    if (!timeinfo) return 0.0f;
    return timeinfo->tm_hour + (timeinfo->tm_min / 60.0f) + (timeinfo->tm_sec / 3600.0f);
}

void test_convert_noon_time(void) {
    struct tm t;
    t.tm_hour = 12;
    t.tm_min = 0;
    t.tm_sec = 0;
    TEST_ASSERT_EQUAL_FLOAT(12.0f, convertTimeToDecimal(&t));
}

void test_convert_half_past_two(void) {
    struct tm t;
    t.tm_hour = 14;
    t.tm_min = 30;
    t.tm_sec = 0;
    TEST_ASSERT_EQUAL_FLOAT(14.5f, convertTimeToDecimal(&t));
}

void test_convert_quarter_to_midnight(void) {
    struct tm t;
    t.tm_hour = 23;
    t.tm_min = 45;
    t.tm_sec = 0;
    TEST_ASSERT_EQUAL_FLOAT(23.75f, convertTimeToDecimal(&t));
}

void test_convert_null_pointer_handling(void) {
    TEST_ASSERT_EQUAL_FLOAT(0.0f, convertTimeToDecimal(NULL));
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_convert_noon_time);
    RUN_TEST(test_convert_half_past_two);
    RUN_TEST(test_convert_quarter_to_midnight);
    RUN_TEST(test_convert_null_pointer_handling);
    return UNITY_END();
}