#include <stdio.h>

#include "defines.h"
#include "platform.h"
#include "xparameters.h"
#include "xil_printf.h"
#include "xscugic.h"
#include "xgpio.h"
#include "xtime_l.h"

#include "interrupt_controller.h"
#include "rsa_encryption.h"
#include "rsa_key_receiver.h"

#define STATUS_LED_DEVICE_ID    XPAR_CONNECTION_EMBEDDED_STATUS_LED_DEVICE_ID
XGpio leds;

XScuGic InterruptController;
rsaData RSAData;

void statusLED();

int main() {
    init_platform();

    XGpio_Initialize(&leds, STATUS_LED_DEVICE_ID);
	XGpio_SetDataDirection(&leds, 1, 0x0);
    XGpio_DiscreteWrite(&leds, 1, 0x3);

    #ifdef DEBUG
        RSAData.publicKey = 0x03;
        RSAData.modulus = 0xF23A877;
    #endif

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

    XGpio_DiscreteWrite(&leds, 1, 0x2);

    print("Embedded application initialized\n\r");

    while (1) statusLED();
    cleanup_platform();
    return 0;
}

void statusLED() {
    XTime tNow;
    static XTime tOld;
	static bool state;
    XTime_GetTime(&tNow);
    if (tOld + 65000000 < tNow) {
        // Toggle status LED
        XGpio_DiscreteWrite(&leds, 1, state << 2);
        state = !state;
        tOld = tNow;
    }
}
