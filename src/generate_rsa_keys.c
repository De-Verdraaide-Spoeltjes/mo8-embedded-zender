#include "generate_rsa_keys.h"
#include "defines.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "xstatus.h"
#include "xil_printf.h"

uint32_t primes[];
uint32_t primeCount;

void seedPrimes() {
	bool isPrime[RSA_PRIMES_MAXIMUM + 1];

	for (int i = 1; i <= RSA_PRIMES_MAXIMUM; i++) {
		isPrime[i] = true;
	}

	isPrime[0] = false;
	isPrime[1] = false;
	isPrime[2] = false;

	for (int i = 2; i <= RSA_PRIMES_MAXIMUM; i++) {
		int j = 2 * (i);
		while (j <= RSA_PRIMES_MAXIMUM) {
			isPrime[j] = false;
			j += i;
		}
	}

	primeCount = 0;
	for (int i = 0; i <= RSA_PRIMES_MAXIMUM; i++) {
		if (isPrime[i]) {
			primes[primeCount] = i;
			primeCount++;
		}
	}
}

int generateRandomPrime() {
	bool found = false;
	int randomPrime = 0;
	while (!found) {
		randomPrime = primes[rand() % primeCount];
		found = randomPrime > RSA_PRIMES_MINIMUM;
	}

	xil_printf("Random prime: %d\n\r", randomPrime);
	return 0;
}

XStatus generateRSAKeys(uint64_t seed) {
	// Generate a list of prime numbers
	seedPrimes();

	// Seed the random number generator
	srand(seed);

	// Generate two random prime numbers
	int p = generateRandomPrime();
	int q = generateRandomPrime();

	return XST_SUCCESS;
}
