#include <stdio.h>

#include "defines.h"
#include "platform.h"
#include "xparameters.h"
#include "xil_printf.h"
#include "xscugic.h"
#include "xgpio.h"

#include "interrupt_controller.h"
#include "rsa_encryption.h"
#include "rsa_key_receiver.h"

XScuGic InterruptController;
rsaData RSAData;

int main() {
    init_platform();
    print("Starting embedded application\n\r");

    XStatus status;

    status = initInterruptController(&InterruptController);
    if (status != XST_SUCCESS) {
    	print("Error initializing interrupt controller\n\r");
    	cleanup_platform();
        return 0;
    }

    status = initKeyReceiver(&RSAData);
    if (status != XST_SUCCESS) {
    	print("Error initializing key receiver\n\r");
    	cleanup_platform();
		return 0;
    }

    XGpio KeyReceiverGpio;
    status = setupGpioWithInterrupt(&InterruptController, &KeyReceiverGpio, XPAR_CONNECTION_EMBEDDED_TRANSMIT_KEYS_DEVICE_ID, XPAR_FABRIC_CONNECTION_EMBEDDED_TRANSMIT_KEYS_IP2INTC_IRPT_INTR, keyReceiverInterrupt);
    if (status != XST_SUCCESS) {
    	print("Error setting up key receiver GPIO with interrupt\n\r");
    	cleanup_platform();
        return 0;
    }

    status = initEncryption(&RSAData);
    if (status != XST_SUCCESS) {
    	print("Error initializing encryption\n\r");
    	cleanup_platform();
		return 0;
    }

    XGpio EncryptionInputGpio;
    status = setupGpioWithInterrupt(&InterruptController, &EncryptionInputGpio, XPAR_CONNECTION_EMBEDDED_RSA_INPUT_VALUE_DEVICE_ID, XPAR_FABRIC_CONNECTION_EMBEDDED_RSA_INPUT_VALUE_IP2INTC_IRPT_INTR, rsaInputInterrupt);
    if (status != XST_SUCCESS) {
    	print("Error setting up RSA input GPIO with interrupt\n\r");
    	cleanup_platform();
        return 0;
    }

//    TODO: Add status LED

    print("Embedded application initialized\n\r");

    while (1);
    cleanup_platform();
    return 0;
}