#include "CppUTest/TestHarness.h"

extern "C" {
  #include "scheduler.h"
  #include "events.h"
}

static int test_val;

TEST_GROUP(test_scheduler) 
{
    
    void setup()
    {
        test_val = 1;
        scheduler_init();
    }

    void teardown()
    {

    }
};

void event_handler_0(enum events e, int ticks)
{
    test_val = 100;
}

void event_handler_1(enum events e, int ticks)
{
    test_val = 101;
}

void event_handler_2(enum events e, int ticks)
{
    test_val = 102;
}

TEST(test_scheduler, test_scheduler_basic)
{
    event_subscribe(EVENT_SYSTICK, event_handler_0);
    event_publish(EVENT_SYSTICK);
    scheduler_check_events();
    LONGS_EQUAL(100, test_val);
}


