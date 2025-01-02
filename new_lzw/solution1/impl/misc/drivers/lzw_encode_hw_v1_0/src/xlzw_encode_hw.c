// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2020.2 (64-bit)
// Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
// ==============================================================
/***************************** Include Files *********************************/
#include "xlzw_encode_hw.h"

/************************** Function Implementation *************************/
#ifndef __linux__
int XLzw_encode_hw_CfgInitialize(XLzw_encode_hw *InstancePtr, XLzw_encode_hw_Config *ConfigPtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(ConfigPtr != NULL);

    InstancePtr->Control_BaseAddress = ConfigPtr->Control_BaseAddress;
    InstancePtr->IsReady = XIL_COMPONENT_IS_READY;

    return XST_SUCCESS;
}
#endif

void XLzw_encode_hw_Start(XLzw_encode_hw *InstancePtr) {
    u32 Data;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLzw_encode_hw_ReadReg(InstancePtr->Control_BaseAddress, XLZW_ENCODE_HW_CONTROL_ADDR_AP_CTRL) & 0x80;
    XLzw_encode_hw_WriteReg(InstancePtr->Control_BaseAddress, XLZW_ENCODE_HW_CONTROL_ADDR_AP_CTRL, Data | 0x01);
}

u32 XLzw_encode_hw_IsDone(XLzw_encode_hw *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLzw_encode_hw_ReadReg(InstancePtr->Control_BaseAddress, XLZW_ENCODE_HW_CONTROL_ADDR_AP_CTRL);
    return (Data >> 1) & 0x1;
}

u32 XLzw_encode_hw_IsIdle(XLzw_encode_hw *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLzw_encode_hw_ReadReg(InstancePtr->Control_BaseAddress, XLZW_ENCODE_HW_CONTROL_ADDR_AP_CTRL);
    return (Data >> 2) & 0x1;
}

u32 XLzw_encode_hw_IsReady(XLzw_encode_hw *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLzw_encode_hw_ReadReg(InstancePtr->Control_BaseAddress, XLZW_ENCODE_HW_CONTROL_ADDR_AP_CTRL);
    // check ap_start to see if the pcore is ready for next input
    return !(Data & 0x1);
}

void XLzw_encode_hw_Continue(XLzw_encode_hw *InstancePtr) {
    u32 Data;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLzw_encode_hw_ReadReg(InstancePtr->Control_BaseAddress, XLZW_ENCODE_HW_CONTROL_ADDR_AP_CTRL) & 0x80;
    XLzw_encode_hw_WriteReg(InstancePtr->Control_BaseAddress, XLZW_ENCODE_HW_CONTROL_ADDR_AP_CTRL, Data | 0x10);
}

void XLzw_encode_hw_EnableAutoRestart(XLzw_encode_hw *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLzw_encode_hw_WriteReg(InstancePtr->Control_BaseAddress, XLZW_ENCODE_HW_CONTROL_ADDR_AP_CTRL, 0x80);
}

void XLzw_encode_hw_DisableAutoRestart(XLzw_encode_hw *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLzw_encode_hw_WriteReg(InstancePtr->Control_BaseAddress, XLZW_ENCODE_HW_CONTROL_ADDR_AP_CTRL, 0);
}

void XLzw_encode_hw_Set_input_data(XLzw_encode_hw *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLzw_encode_hw_WriteReg(InstancePtr->Control_BaseAddress, XLZW_ENCODE_HW_CONTROL_ADDR_INPUT_DATA_DATA, (u32)(Data));
    XLzw_encode_hw_WriteReg(InstancePtr->Control_BaseAddress, XLZW_ENCODE_HW_CONTROL_ADDR_INPUT_DATA_DATA + 4, (u32)(Data >> 32));
}

u64 XLzw_encode_hw_Get_input_data(XLzw_encode_hw *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLzw_encode_hw_ReadReg(InstancePtr->Control_BaseAddress, XLZW_ENCODE_HW_CONTROL_ADDR_INPUT_DATA_DATA);
    Data += (u64)XLzw_encode_hw_ReadReg(InstancePtr->Control_BaseAddress, XLZW_ENCODE_HW_CONTROL_ADDR_INPUT_DATA_DATA + 4) << 32;
    return Data;
}

void XLzw_encode_hw_Set_input_size(XLzw_encode_hw *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLzw_encode_hw_WriteReg(InstancePtr->Control_BaseAddress, XLZW_ENCODE_HW_CONTROL_ADDR_INPUT_SIZE_DATA, (u32)(Data));
    XLzw_encode_hw_WriteReg(InstancePtr->Control_BaseAddress, XLZW_ENCODE_HW_CONTROL_ADDR_INPUT_SIZE_DATA + 4, (u32)(Data >> 32));
}

u64 XLzw_encode_hw_Get_input_size(XLzw_encode_hw *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLzw_encode_hw_ReadReg(InstancePtr->Control_BaseAddress, XLZW_ENCODE_HW_CONTROL_ADDR_INPUT_SIZE_DATA);
    Data += (u64)XLzw_encode_hw_ReadReg(InstancePtr->Control_BaseAddress, XLZW_ENCODE_HW_CONTROL_ADDR_INPUT_SIZE_DATA + 4) << 32;
    return Data;
}

void XLzw_encode_hw_Set_compressed_data(XLzw_encode_hw *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLzw_encode_hw_WriteReg(InstancePtr->Control_BaseAddress, XLZW_ENCODE_HW_CONTROL_ADDR_COMPRESSED_DATA_DATA, (u32)(Data));
    XLzw_encode_hw_WriteReg(InstancePtr->Control_BaseAddress, XLZW_ENCODE_HW_CONTROL_ADDR_COMPRESSED_DATA_DATA + 4, (u32)(Data >> 32));
}

u64 XLzw_encode_hw_Get_compressed_data(XLzw_encode_hw *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLzw_encode_hw_ReadReg(InstancePtr->Control_BaseAddress, XLZW_ENCODE_HW_CONTROL_ADDR_COMPRESSED_DATA_DATA);
    Data += (u64)XLzw_encode_hw_ReadReg(InstancePtr->Control_BaseAddress, XLZW_ENCODE_HW_CONTROL_ADDR_COMPRESSED_DATA_DATA + 4) << 32;
    return Data;
}

u64 XLzw_encode_hw_Get_compressed_size(XLzw_encode_hw *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLzw_encode_hw_ReadReg(InstancePtr->Control_BaseAddress, XLZW_ENCODE_HW_CONTROL_ADDR_COMPRESSED_SIZE_DATA);
    Data += (u64)XLzw_encode_hw_ReadReg(InstancePtr->Control_BaseAddress, XLZW_ENCODE_HW_CONTROL_ADDR_COMPRESSED_SIZE_DATA + 4) << 32;
    return Data;
}

void XLzw_encode_hw_InterruptGlobalEnable(XLzw_encode_hw *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLzw_encode_hw_WriteReg(InstancePtr->Control_BaseAddress, XLZW_ENCODE_HW_CONTROL_ADDR_GIE, 1);
}

void XLzw_encode_hw_InterruptGlobalDisable(XLzw_encode_hw *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLzw_encode_hw_WriteReg(InstancePtr->Control_BaseAddress, XLZW_ENCODE_HW_CONTROL_ADDR_GIE, 0);
}

void XLzw_encode_hw_InterruptEnable(XLzw_encode_hw *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XLzw_encode_hw_ReadReg(InstancePtr->Control_BaseAddress, XLZW_ENCODE_HW_CONTROL_ADDR_IER);
    XLzw_encode_hw_WriteReg(InstancePtr->Control_BaseAddress, XLZW_ENCODE_HW_CONTROL_ADDR_IER, Register | Mask);
}

void XLzw_encode_hw_InterruptDisable(XLzw_encode_hw *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XLzw_encode_hw_ReadReg(InstancePtr->Control_BaseAddress, XLZW_ENCODE_HW_CONTROL_ADDR_IER);
    XLzw_encode_hw_WriteReg(InstancePtr->Control_BaseAddress, XLZW_ENCODE_HW_CONTROL_ADDR_IER, Register & (~Mask));
}

void XLzw_encode_hw_InterruptClear(XLzw_encode_hw *InstancePtr, u32 Mask) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLzw_encode_hw_WriteReg(InstancePtr->Control_BaseAddress, XLZW_ENCODE_HW_CONTROL_ADDR_ISR, Mask);
}

u32 XLzw_encode_hw_InterruptGetEnabled(XLzw_encode_hw *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XLzw_encode_hw_ReadReg(InstancePtr->Control_BaseAddress, XLZW_ENCODE_HW_CONTROL_ADDR_IER);
}

u32 XLzw_encode_hw_InterruptGetStatus(XLzw_encode_hw *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XLzw_encode_hw_ReadReg(InstancePtr->Control_BaseAddress, XLZW_ENCODE_HW_CONTROL_ADDR_ISR);
}

