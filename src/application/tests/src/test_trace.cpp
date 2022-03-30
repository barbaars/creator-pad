#include "CppUTest/TestHarness.h"

extern "C" {
  #include "trace.h"
}

TEST_GROUP(test_trace) 
{    
    trace trace_0;

    void setup()
    {
        
    }

    void teardown()
    {

    }
};

TEST(test_trace, test_trace_basic)
{
    const int trigger_level = 120;

    trace_init(&trace_0, 1337, trigger_level);

    uint16_t sample=0;

    int add = 0;
    for (int i=0; i<TRACE_SIZE-TRACE_DELAY; i+=add)
    {
        if (sample > trigger_level && add == 0)
            add = 1;
            
        int result = trace_put(&trace_0, sample++);
        LONGS_EQUAL(0, result);
    }

    {
        int result = trace_put(&trace_0, sample++);
        LONGS_EQUAL(1, result);
    }

    {
        int result = trace_put(&trace_0, sample++);
        LONGS_EQUAL(-1, result);
    }
}
