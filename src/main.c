#include <stdio.h>

#include "defines.h"
#include "platform.h"
#include "rsa_encryption.h"
#include "rsa_key_receiver.h"
#include "xil_printf.h"

rsaData *RSAData;

int main() {
    init_platform();
    print("Starting embedded application\n\r");

    XStatus status;

    status = initKeyReceiver();
    if (status != XST_SUCCESS) {
    	print("Error initializing key receiver\n\r");
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

    while (1);

    cleanup_platform();
    return 0;
}
