#include "interrupt_controller.h"

#include "xil_printf.h"
#include "xil_exception.h"
#include "xscugic.h"
#include "xgpio.h"

#include "defines.h"

XStatus initInterruptController(XScuGic *interruptController) {
    XScuGic_Config *IntcConfig;
    XStatus status;

    IntcConfig = XScuGic_LookupConfig(XPAR_PS7_SCUGIC_0_DEVICE_ID);
    if (NULL == IntcConfig) {
        return XST_FAILURE;
    }

    status = XScuGic_CfgInitialize(interruptController, IntcConfig, IntcConfig->CpuBaseAddress);
    if (status != XST_SUCCESS) {
        return XST_FAILURE;
    }

    Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT, (Xil_ExceptionHandler)XScuGic_InterruptHandler, interruptController);
    Xil_ExceptionEnable();

    #ifdef DEBUG
        xil_printf("Interrupt controller initialized\n\r");
    #endif

    return XST_SUCCESS;
}

XStatus setupGpioWithInterrupt(XScuGic *interruptController, XGpio *gpio, u16 gpioDeviceId, u32 interruptId, Xil_InterruptHandler handler) {
    XStatus status;

    status = XGpio_Initialize(gpio, gpioDeviceId);
    if (status != XST_SUCCESS) {
        return XST_FAILURE;
    }

    XGpio_SetDataDirection(gpio, 1, 0xFFFFFFFF);

    #ifdef DEBUG
        xil_printf("GPIO initialized\n\r");
    #endif

    status = XScuGic_Connect(interruptController, interruptId, (Xil_ExceptionHandler)handler, (void *)gpio);
    if (status != XST_SUCCESS) {
        return XST_FAILURE;
    }

    #ifdef DEBUG
        xil_printf("Interrupt connected\n\r");
    #endif

    XGpio_InterruptEnable(gpio, 1);
    XGpio_InterruptGlobalEnable(gpio);

    #ifdef DEBUG
        xil_printf("GPIO interrupt enabled\n\r");
    #endif

    XScuGic_Enable(interruptController, interruptId);

    #ifdef DEBUG
        xil_printf("Interrupt controller enabled\n\r");
    #endif

    return XST_SUCCESS;
}