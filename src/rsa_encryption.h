#ifndef SRC_RSA_ENCRYPTION_H
#define SRC_RSA_ENCRYPTION_H

#include "xstatus.h"
#include <stdbool.h>
#include "defines.h"

XStatus initEncryption(rsaData *RSAData);
void rsaInputInterrupt(void* baseAddr);

#endif