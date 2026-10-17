#ifndef PE_GTE_COMMANDS_H
#define PE_GTE_COMMANDS_H
#include <stdint.h>
/* Native arithmetic only. Unknown operation returns -1 without a state write. */
int PE_GTE_ExecuteNative(uint32_t word);
#endif
