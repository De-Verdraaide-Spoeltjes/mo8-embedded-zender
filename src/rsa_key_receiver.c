#include "rsa_key_receiver.h"

#include <stdbool.h>
#include "xstatus.h"
#include "xuartps.h"
#include "xtime_l.h"

#include "defines.h"

#define UART_DEVICE_ID XPAR_PS7_UART_1_DEVICE_ID

XUartPs uart;		/* Instance of the UART Device */

bool UartSendAvailable();
void UartSendBytes(u8 *data, int length);
bool UartRecvAvailable();
void UartRecvBytes(u8 *data, int length);

XStatus initKeyReceiver() {
	XUartPs_Config *Config;
	int status;

	Config = XUartPs_LookupConfig(UART_DEVICE_ID);
	if (NULL == Config) {
		return XST_FAILURE;
	}

	status = XUartPs_CfgInitialize(&uart, Config, Config->BaseAddress);
	if (status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	XUartPs_SetBaudRate(&uart, 9600);

	xil_printf("UART initialized\n\r");

	return XST_SUCCESS;
}

// TODO: Add timeouts to UART

void runKeyReceiver(rsaData *RSAData) {
	#ifdef DEBUG
		xil_printf("Requesting RSA key\n\r");
	#endif
	
	u8 data, resp;

	// Wait for room to send data
	while (!UartSendAvailable());

	// Request RSA key command
	data = 0x01;
	UartSendBytes(&data, 1);

	// Wait for response
	while (!UartRecvAvailable());

	// Read response
	UartRecvBytes(&resp, 1);
	
	// Check if response is correct
	if (resp != 0x01) {
		#ifdef DEBUG
			xil_printf("Error: Received unknown command\n\r");
		#endif
		return;
	}

	#ifdef DEBUG
		xil_printf("Waiting for RSA key transmission\n\r");
	#endif

	// Wait for response
	while (!UartRecvAvailable());

	// Read RSA key command
	UartRecvBytes(&data, 1);

	// Check if command is correct
	if (data != 0x02) {
		#ifdef DEBUG
			xil_printf("Error: Received unknown command\n\r");
		#endif
		return;
	}

	#ifdef DEBUG
		xil_printf("Receiving RSA key\n\r");
	#endif

	// Wait for room to send data
	while (!UartSendAvailable());

	// Receive RSA key command
	resp = 0x02;
	UartSendBytes(&resp, 1);

	// Wait for response
	while (!UartRecvAvailable());

	// Receive RSA key data
	u8 keyData[8];
	UartRecvBytes(keyData, 8);
	
	// Store received RSA key data
	RSAData->publicKey = *((uint64_t *) keyData);

	// Wait for room to send data
	while (!UartSendAvailable());

	// Acknowledge received key
	resp = 0x03;
	UartSendBytes(&resp, 1);

	#ifdef DEBUG
		xil_printf("Received RSA key: ");
		xil_printf("0x%x%x\n\r", (uint32_t) (RSAData->publicKey >> 32), (uint32_t) RSAData->publicKey);
	#endif

	// Wait for response
	while (!UartRecvAvailable());

	// Receive RSA modulus data
	u8 modulusData[4];
	UartRecvBytes(modulusData, 4);

	// Store received RSA modulus data
	RSAData->modulus = *((uint32_t *) modulusData);

	// Wait for room to send data
	while (!UartSendAvailable());

	// Acknowledge received modulus
	resp = 0x04;
	UartSendBytes(&resp, 1);

	#ifdef DEBUG
		xil_printf("Received RSA modulus: 0x%x\n\r", RSAData->modulus);
	#endif
}

// Check if UART is ready to send data
bool UartSendAvailable() {
	u32 status = XUartPs_ReadReg(uart.Config.BaseAddress, XUARTPS_SR_OFFSET);
	return (status & XUARTPS_SR_TXFULL) == 0;
}

// Send data via UART
void UartSendBytes(u8 *data, int length) {
	int bytesSent = 0;
	while (bytesSent < length) {
		bytesSent += XUartPs_Send(&uart, data + bytesSent, 1);
	}
}

// Check if UART is ready to receive data
bool UartRecvAvailable() {
	u32 status = XUartPs_ReadReg(uart.Config.BaseAddress, XUARTPS_SR_OFFSET);
	return (status & XUARTPS_SR_RXEMPTY) == 0;
}

// Receive data from UART
void UartRecvBytes(u8 *data, int length) {
	int bytesReceived = 0;
	while (bytesReceived < length) {
		bytesReceived += XUartPs_Recv(&uart, data + length - bytesReceived -1, 1);
	}
}
