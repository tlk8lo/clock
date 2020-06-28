#ifndef DCF_H
#define DCF_H

#include <inttypes.h>

typedef __uint24 uint24_t;

extern uint8_t dcf_validate(uint8_t *frame);
extern uint24_t dcf_parse(uint8_t *frame);

#endif /* DCF_H */