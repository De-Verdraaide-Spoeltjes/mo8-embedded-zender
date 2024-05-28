#ifndef SRC_DEFINES_H
#define SRC_DEFINES_H

#include <stdint.h>

// Uncomment to enable debug output
#define DEBUG

// The range for the random prime number generator - bigger primes means bigger RSA keys
#define RSA_PRIMES_MINIMUM 200
#define RSA_PRIMES_MAXIMUM 10000

// Data structure for RSA keys
typedef struct RSAData {
	uint64_t publicKey;
	uint32_t modulus;
} rsaData;

#endif