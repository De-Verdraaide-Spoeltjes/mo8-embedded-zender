#include "rsa_key_receiver.h"

#include <stdbool.h>
#include "xstatus.h"
#include "xuartps.h"
#include "xtime_l.h"
#include "xgpio.h"

#include "defines.h"

#define UART_DEVICE_ID XPAR_PS7_UART_1_DEVICE_ID

XUartPs uart;		/* Instance of the UART Device */

rsaData *RSAdata;

bool UartSendAvailable();
void UartSendBytes(u8 *data, int length);
bool UartRecvAvailable();
void UartRecvBytes(u8 *data, int length);
void runKeyReceiver();

XStatus initKeyReceiver(rsaData *RSAData) {
	RSAdata = RSAData;

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

void keyReceiverInterrupt(void* gpio) {
	// Disable interrupts
	XGpio_InterruptDisable((XGpio *)gpio, 1);

	// Ignore additional button presses
	if ((XGpio_InterruptGetStatus((XGpio *)gpio) & 1) != 1) {
		return;
	}

	// Check if the input value is high
	bool risingEdge = XGpio_DiscreteRead((XGpio *)gpio, 1);

	// Execute key receiver if rising edge
	if (risingEdge) {
		runKeyReceiver();
	}

	// Clear interrupt
	XGpio_InterruptClear((XGpio *)gpio, 1);

	// Re-enable interrupts
	XGpio_InterruptEnable((XGpio *)gpio, 1);
}

// TODO: Add timeouts to UART

void runKeyReceiver() {
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
			xil_printf("Error: Received unknown command, expected 0x01 and received 0x%x\n\r", resp);
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
			xil_printf("Error: Received unknown command, expected 0x02 and received 0x%x\n\r", resp);
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
	RSAdata->publicKey = *((uint64_t *) keyData);

	// Wait for room to send data
	while (!UartSendAvailable());

	// Acknowledge received key
	resp = 0x03;
	UartSendBytes(&resp, 1);

	#ifdef DEBUG
		xil_printf("Received RSA key: ");
		xil_printf("0x%x%x\n\r", (uint32_t) (RSAdata->publicKey >> 32), (uint32_t) RSAdata->publicKey);
	#endif

	// Wait for response
	while (!UartRecvAvailable());

	// Receive RSA modulus data
	u8 modulusData[4];
	UartRecvBytes(modulusData, 4);

	// Store received RSA modulus data
	RSAdata->modulus = *((uint32_t *) modulusData);

	// Wait for room to send data
	while (!UartSendAvailable());

	// Acknowledge received modulus
	resp = 0x04;
	UartSendBytes(&resp, 1);

	#ifdef DEBUG
		xil_printf("Received RSA modulus: 0x%x\n\r", RSAdata->modulus);
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
