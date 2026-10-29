#include <unity.h>
#include <stdint.h>

struct VisitData {
    float durationSeconds;
    
    bool isValidVisit(float minDurationThreshold) const {
        return durationSeconds >= minDurationThreshold;
    }
};

void test_duration_seconds_calculation(void) {
    uint32_t startMillis = 1000;
    uint32_t endMillis = 11500; // 10.5 seconds later
    
    float duration = (endMillis - startMillis) / 1000.0f;
    
    TEST_ASSERT_EQUAL_FLOAT(10.5f, duration);
}

void test_duration_overflow_handling(void) {
    // Tests millis() rollover scenario math
    uint32_t startMillis = 4294967200; // Near uint32_t limit
    uint32_t endMillis = 1000;        // Rolled over
    
    uint32_t durationMillis = endMillis - startMillis;
    float durationSeconds = durationMillis / 1000.0f;
    
    TEST_ASSERT_TRUE(durationSeconds > 0.0f);
}

void test_minimum_visit_threshold_validation(void) {
    VisitData validVisit{ 12.0f };
    VisitData noiseTrigger{ 0.5f };

    TEST_ASSERT_TRUE(validVisit.isValidVisit(2.0f));
    TEST_ASSERT_FALSE(noiseTrigger.isValidVisit(2.0f));
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_duration_seconds_calculation);
    RUN_TEST(test_duration_overflow_handling);
    RUN_TEST(test_minimum_visit_threshold_validation);
    return UNITY_END();
}