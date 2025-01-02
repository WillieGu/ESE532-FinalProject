// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2020.2 (64-bit)
// Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
// ==============================================================
#ifndef XLZW_ENCODE_HW_H
#define XLZW_ENCODE_HW_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/
#ifndef __linux__
#include "xil_types.h"
#include "xil_assert.h"
#include "xstatus.h"
#include "xil_io.h"
#else
#include <stdint.h>
#include <assert.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stddef.h>
#endif
#include "xlzw_encode_hw_hw.h"

/**************************** Type Definitions ******************************/
#ifdef __linux__
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
#else
typedef struct {
    u16 DeviceId;
    u32 Control_BaseAddress;
} XLzw_encode_hw_Config;
#endif

typedef struct {
    u64 Control_BaseAddress;
    u32 IsReady;
} XLzw_encode_hw;

typedef u32 word_type;

/***************** Macros (Inline Functions) Definitions *********************/
#ifndef __linux__
#define XLzw_encode_hw_WriteReg(BaseAddress, RegOffset, Data) \
    Xil_Out32((BaseAddress) + (RegOffset), (u32)(Data))
#define XLzw_encode_hw_ReadReg(BaseAddress, RegOffset) \
    Xil_In32((BaseAddress) + (RegOffset))
#else
#define XLzw_encode_hw_WriteReg(BaseAddress, RegOffset, Data) \
    *(volatile u32*)((BaseAddress) + (RegOffset)) = (u32)(Data)
#define XLzw_encode_hw_ReadReg(BaseAddress, RegOffset) \
    *(volatile u32*)((BaseAddress) + (RegOffset))

#define Xil_AssertVoid(expr)    assert(expr)
#define Xil_AssertNonvoid(expr) assert(expr)

#define XST_SUCCESS             0
#define XST_DEVICE_NOT_FOUND    2
#define XST_OPEN_DEVICE_FAILED  3
#define XIL_COMPONENT_IS_READY  1
#endif

/************************** Function Prototypes *****************************/
#ifndef __linux__
int XLzw_encode_hw_Initialize(XLzw_encode_hw *InstancePtr, u16 DeviceId);
XLzw_encode_hw_Config* XLzw_encode_hw_LookupConfig(u16 DeviceId);
int XLzw_encode_hw_CfgInitialize(XLzw_encode_hw *InstancePtr, XLzw_encode_hw_Config *ConfigPtr);
#else
int XLzw_encode_hw_Initialize(XLzw_encode_hw *InstancePtr, const char* InstanceName);
int XLzw_encode_hw_Release(XLzw_encode_hw *InstancePtr);
#endif

void XLzw_encode_hw_Start(XLzw_encode_hw *InstancePtr);
u32 XLzw_encode_hw_IsDone(XLzw_encode_hw *InstancePtr);
u32 XLzw_encode_hw_IsIdle(XLzw_encode_hw *InstancePtr);
u32 XLzw_encode_hw_IsReady(XLzw_encode_hw *InstancePtr);
void XLzw_encode_hw_Continue(XLzw_encode_hw *InstancePtr);
void XLzw_encode_hw_EnableAutoRestart(XLzw_encode_hw *InstancePtr);
void XLzw_encode_hw_DisableAutoRestart(XLzw_encode_hw *InstancePtr);

void XLzw_encode_hw_Set_input_data(XLzw_encode_hw *InstancePtr, u64 Data);
u64 XLzw_encode_hw_Get_input_data(XLzw_encode_hw *InstancePtr);
void XLzw_encode_hw_Set_input_size(XLzw_encode_hw *InstancePtr, u64 Data);
u64 XLzw_encode_hw_Get_input_size(XLzw_encode_hw *InstancePtr);
void XLzw_encode_hw_Set_compressed_data(XLzw_encode_hw *InstancePtr, u64 Data);
u64 XLzw_encode_hw_Get_compressed_data(XLzw_encode_hw *InstancePtr);
u64 XLzw_encode_hw_Get_compressed_size(XLzw_encode_hw *InstancePtr);

void XLzw_encode_hw_InterruptGlobalEnable(XLzw_encode_hw *InstancePtr);
void XLzw_encode_hw_InterruptGlobalDisable(XLzw_encode_hw *InstancePtr);
void XLzw_encode_hw_InterruptEnable(XLzw_encode_hw *InstancePtr, u32 Mask);
void XLzw_encode_hw_InterruptDisable(XLzw_encode_hw *InstancePtr, u32 Mask);
void XLzw_encode_hw_InterruptClear(XLzw_encode_hw *InstancePtr, u32 Mask);
u32 XLzw_encode_hw_InterruptGetEnabled(XLzw_encode_hw *InstancePtr);
u32 XLzw_encode_hw_InterruptGetStatus(XLzw_encode_hw *InstancePtr);

#ifdef __cplusplus
}
#endif

#endif
