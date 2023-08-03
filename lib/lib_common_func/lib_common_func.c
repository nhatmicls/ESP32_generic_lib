#include <stdio.h>
#include "lib_common_func.h"

void NOP(void)
{
#ifdef DEBUG_STAGE
    printf("No operation\n");
#endif
    return;
}