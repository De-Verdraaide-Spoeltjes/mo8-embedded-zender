#ifndef SRC_DEFINES_H
#define SRC_DEFINES_H

#include <stdint.h>

typedef struct RSAData {
	uint64_t publicKey;
	uint64_t privateKey;
	uint64_t modulus;
} rsaData;

// The range for the random prime number generator - bigger primes means bigger RSA keys
#define RSA_PRIMES_MINIMUM 5
#define RSA_PRIMES_MAXIMUM 100

#endif
