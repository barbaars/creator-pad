#ifndef _RBUFF_H_
#define _RBUFF_H_

#include <stdint.h>

typedef struct rbuff
{
  void *m;
  uint32_t max;
  uint32_t in;
  uint32_t out;
  uint32_t size;
} rbuff;

int rbuff_init(rbuff *buffer, void *memory, uint32_t size);

int rbuff_put_byte(rbuff *buffer, uint8_t in);
int rbuff_put_short(rbuff *buffer, uint16_t in);
int rbuff_put_word(rbuff *buffer, uint32_t in);

int rbuff_get_byte(rbuff *buffer, uint8_t *out);
int rbuff_get_short(rbuff *buffer, uint16_t *out);
int rbuff_get_word(rbuff *buffer, uint32_t *out);

int rbuff_full(rbuff *buffer);
void rbuff_clear(rbuff *buffer);
void rbuff_fillzero(rbuff *buffer);

#endif /* _RBUFF_H_ */
