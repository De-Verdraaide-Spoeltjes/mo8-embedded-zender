#ifndef SRC_DEFINES_H
#define SRC_DEFINES_H

#include <stdint.h>

typedef struct RSAData {
	uint64_t publicKey;
	uint64_t privateKey;
	uint32_t modulus;
} rsaData;

#endif
