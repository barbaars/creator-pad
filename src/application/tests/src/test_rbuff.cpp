#include "CppUTest/TestHarness.h"

extern "C" {
  #include "rbuff.h"
}

TEST_GROUP(test_rbuff) 
{
    rbuff rb;
    
    void setup()
    {
        
    }

    void teardown()
    {

    }
};

TEST(test_rbuff, test_rbuff_basic_byte)
{
    uint8_t mem[32];

    rbuff_init(&rb, mem, 32);

    rbuff_put_byte(&rb, 101);
    rbuff_put_byte(&rb, 102);
    rbuff_put_byte(&rb, 103);

    uint8_t out;
    rbuff_get_byte(&rb, &out);
    LONGS_EQUAL(101, out);

    rbuff_get_byte(&rb, &out);
    LONGS_EQUAL(102, out);

    rbuff_get_byte(&rb, &out);
    LONGS_EQUAL(103, out);
}

TEST(test_rbuff, test_rbuff_basic_short)
{
    uint16_t mem[32];

    rbuff_init(&rb, mem, 32);
    
    rbuff_put_short(&rb, 1001);
    rbuff_put_short(&rb, 1002);
    rbuff_put_short(&rb, 1003);

    uint16_t out;
    rbuff_get_short(&rb, &out);
    LONGS_EQUAL(1001, out);

    rbuff_get_short(&rb, &out);
    LONGS_EQUAL(1002, out);

    rbuff_get_short(&rb, &out);
    LONGS_EQUAL(1003, out);
}

TEST(test_rbuff, test_rbuff_basic_word)
{
    uint32_t mem[32];

    rbuff_init(&rb, mem, 32);
    
    rbuff_put_word(&rb, 90001);
    rbuff_put_word(&rb, 90002);
    rbuff_put_word(&rb, 90003);

    uint32_t out;
    rbuff_get_word(&rb, &out);
    LONGS_EQUAL(90001, out);

    rbuff_get_word(&rb, &out);
    LONGS_EQUAL(90002, out);

    rbuff_get_word(&rb, &out);
    LONGS_EQUAL(90003, out);
}