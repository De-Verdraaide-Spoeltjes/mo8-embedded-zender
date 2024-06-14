#include "rsa_encryption.h"
#include "xstatus.h"
#include "xgpio.h"
#include "defines.h"
#include <stdlib.h>

rsaData *RSA_Data; 																		// Data structure for RSA keys

uint32_t *prime_values; 																// Array for prime values

XGpio dataOut;  																		// GPIO for data out
XGpio Data_Rdy; 																		// GPIO for data ready

void rsaProtocol(uint8_t inputValue); 													// Function for the RSA protocol

void checkData(){ 
xil_printf("Data Ready: %x \n\r", XGpio_DiscreteRead(&Data_Rdy, 1)); 					// Print the data ready value
}

XStatus initEncryption(rsaData *RSAData) {
	RSA_Data = RSAData; 																// Set the RSA data structure
	
	XGpio_Initialize(&dataOut, XPAR_CONNECTION_EMBEDDED_RSA_ENCRYPTED_CHAR_DEVICE_ID);  // Initialize the GPIO driver
	XGpio_Initialize(&Data_Rdy, XPAR_CONNECTION_EMBEDDED_RSA_DATA_READY_DEVICE_ID);		// Initialize the GPIO driver
	
	// Set the direction of the GPIO ports
	XGpio_SetDataDirection(&dataOut, 1, 0xFFFFFFFF); 									// Set as output
	XGpio_SetDataDirection(&Data_Rdy, 1, 0xFFFFFFFF); 									// Set as output

	return XST_SUCCESS;
}

void rsaInputInterrupt(void* gpio) { 													// Interrupt handler for the RSA input GPIO
	
	XGpio_InterruptDisable((XGpio *)gpio, 1); 											// Disable interrupts	

	// Ignore additional button presses
	if ((XGpio_InterruptGetStatus((XGpio *)gpio) & 1) != 1) {
		return;
	}

	uint8_t inputValue = XGpio_DiscreteRead((XGpio *)gpio, 1); 							// Read the input value from the GPIO port
	rsaProtocol(inputValue); 															// Start the RSA protocol with the input value

	// Clear interrupt
	XGpio_InterruptClear((XGpio *)gpio, 1);

	// Re-enable interrupts
	XGpio_InterruptEnable((XGpio *)gpio, 1);

}

// Starts the RSA protocol with the given input value.
// @param inputValue The input value to be encrypted.
void rsaProtocol(uint8_t inputValue) { 
	xil_printf("Start RSA Protocol \n\r"); 												// Start the RSA protocol
	xil_printf("Input Value: %0x \n\r", inputValue); 									// Print the input value

	XGpio_DiscreteWrite(&Data_Rdy, 1, 0); 												// Set data ready to 0 (not ready) 

	if (inputValue == 0xA || inputValue == 0xB || inputValue == 0xF){ 					// Check if the input value is a valid value
		xil_printf("No match with data \n\r"); 											// Print that there is no match with the data
		return;
	} 

	uint64_t encryptedData = 1; 														// Initialize the encrypted data
	for (int i = 0; i < RSA_Data->publicKey; i++) {										// Encrypt the data
		encryptedData = encryptedData * (inputValue + 48);								// InputValue + 48 door ASCII waarde (0 = 48)
		encryptedData = encryptedData % RSA_Data->modulus;  							// Modulus the data with the modulus value of the RSA data structure
	}

	XGpio_DiscreteWrite(&dataOut, 1, encryptedData); 									// Write the encrypted data to the output GPIO port 
	XGpio_DiscreteWrite(&Data_Rdy, 1, 1); 												// Set data ready to 1 (ready)

	xil_printf("Encrypted Data: %d",(unsigned int)((encryptedData & 0xFFFFFFFF00000000) >> 32)); // Print the encrypted data (first part)
	xil_printf("%d\r\n",(unsigned int)(encryptedData & 0x00000000FFFFFFFF)); 					 // Print the encrypted data (continued)
}
