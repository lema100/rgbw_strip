#ifndef BOOT_H
#define BOOT_H

#include <stdint.h>
#include <stdbool.h>

typedef void (*pFunction)(void);
__RAM_FUNC void BOOT_boot(uint32_t start_addr);

#endif
