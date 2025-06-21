#include "unity.h"

void setUp(void)   {}   /* Unity hooks */
void tearDown(void){}

int main(void)
{
    UNITY_BEGIN();
    TEST_ASSERT_EQUAL_INT(1, 1);   // placeholder
    return UNITY_END();
}