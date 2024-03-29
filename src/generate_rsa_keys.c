#include "generate_rsa_keys.h"
#include "defines.h"

#include <stdint.h>
#include <stdbool.h>
#include "xstatus.h"

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

XStatus generateRSAKeys() {
	seedPrimes();

	return XST_SUCCESS;
}
