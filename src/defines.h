#ifndef SRC_DEFINES_H
#define SRC_DEFINES_H

#include <stdint.h>

// Uncomment to enable debug output
#define DEBUG

// Data structure for RSA keys
typedef struct RSAData {
	uint64_t publicKey;
	uint64_t privateKey;
	uint32_t modulus;
} rsaData;

#endif
