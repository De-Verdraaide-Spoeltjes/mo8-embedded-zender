#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"

#include "defines.h"
#include "generate_rsa_keys.h"
#include "transmit_rsa_keys.h"
#include "rsa_encryption.h"

struct rsaData *RSAData;

int main()
{
    init_platform();
    print("Starting embedded application\n\r");

    XStatus status;

    status = generateRSAKeys(557);
    if (status != XST_SUCCESS) {
		print("Error generating encryption keys\n\r");
		cleanup_platform();
		return 0;
	}

    status = initKeyTransmission();
    if (status != XST_SUCCESS) {
    	print("Error initializing key transmission\n\r");
    	cleanup_platform();
		return 0;
    }

    status = initEncryption();
    if (status != XST_SUCCESS) {
    	print("Error initializing encryption\n\r");
    	cleanup_platform();
		return 0;
    }

//    TODO: Add status LED

    print("Embedded application initialized\n\r");

    while(1);

    cleanup_platform();
    return 0;
}
