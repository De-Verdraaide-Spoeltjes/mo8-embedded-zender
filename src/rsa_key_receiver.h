#ifndef SRC_RSA_KEY_RECEIVER_H
#define SRC_RSA_KEY_RECEIVER_H

#include "xstatus.h"
#include "defines.h"

XStatus initKeyReceiver(rsaData *RSAData);
void keyReceiverInterrupt(void* baseAddr);

#endif
