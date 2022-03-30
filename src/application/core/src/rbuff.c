/* Includes */
#include "rbuff.h"

#include <string.h>

/* Defines */

/* Globals */

/* Locals */

/* Function Prototypes */

/* Functions */

/*
 * rbuff_init
 *
 */
int rbuff_init(rbuff *buffer, void *memory, uint32_t size)
{
    // Check if power of 2
    if(size && !(size&(size-1)))
    {
        buffer->m = memory;
        buffer->max = size;

        rbuff_clear(buffer);

        return 0;
    }

    return -1;
}

/*
 * rbuff_put_byte
 *
 */
int rbuff_put_byte(rbuff *buffer, uint8_t in)
{
    uint8_t *p;

    if(buffer->size < buffer->max)
    {
        p = (uint8_t *)buffer->m;

        p[buffer->in] = in;
        buffer->in++;
        buffer->in &= buffer->max-1;
        buffer->size++;

        return 0;
    }

    return -1;
}

/*
 * rbuff_put_short
 *
 */
int rbuff_put_short(rbuff *buffer, uint16_t in)
{
    uint16_t *p;

    if(buffer->size < buffer->max)
    {
        p = (uint16_t *)buffer->m;

        p[buffer->in] = in;
        buffer->in++;
        buffer->in &= buffer->max-1;
        buffer->size++;

        return 0;
    }

    return -1;
}

/*
 * rbuff_put_word
 *
 */
int rbuff_put_word(rbuff *buffer, uint32_t in)
{
    uint32_t *p;

    if(buffer->size < buffer->max)
    {
        p = (uint32_t *)buffer->m;

        p[buffer->in] = in;
        buffer->in++;
        buffer->in &= buffer->max-1;
        buffer->size++;

        return 0;
    }

    return -1;
}

/*
 * rbuff_get_byte
 *
 */
int rbuff_get_byte(rbuff *buffer, uint8_t *out)
{
    uint8_t *p;

    if(buffer->size > 0)
    {
        buffer->size--;

        p = (uint8_t *)buffer->m;

        *out = p[buffer->out];
        buffer->out++;
        buffer->out &= buffer->max-1;

        return 0;
    }

    return -1;
}

/*
 * rbuff_get_short
 *
 */
int rbuff_get_short(rbuff *buffer, uint16_t *out)
{
    uint16_t *p;

    if(buffer->size > 0)
    {
        buffer->size--;

        p = (uint16_t *)buffer->m;

        *out = p[buffer->out];
        buffer->out++;
        buffer->out &= buffer->max-1;
    
        return 0;
    }

    return -1;
}

/*
 * rbuff_get_word
 *
 */
int rbuff_get_word(rbuff *buffer, uint32_t *out)
{
    uint32_t *p;

    if(buffer->size > 0)
    {
        buffer->size--;

        p = (uint32_t *)buffer->m;

        *out = p[buffer->out];
        buffer->out++;
        buffer->out &= buffer->max-1;
    
        return 0;
    }

    return -1;
}

/*
 * rbuff_full
 *
 */
int rbuff_full(rbuff *buffer)
{
    return buffer->size == buffer->max;
}

/*
 * rbuff_clear
 *
 */
void rbuff_clear(rbuff *buffer)
{
    buffer->in = 0;
    buffer->out = 0;
    buffer->size = 0;
}

/*
 * rbuff_fillzero
 *
 */
void rbuff_fillzero(rbuff *buffer)
{
    rbuff_clear(buffer);
    buffer->size = buffer->max;
    memset(buffer->m, 0, buffer->max);
}
