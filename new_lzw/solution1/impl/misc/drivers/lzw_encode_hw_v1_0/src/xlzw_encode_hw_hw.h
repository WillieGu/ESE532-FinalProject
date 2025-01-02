// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2020.2 (64-bit)
// Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
// ==============================================================
// control
// 0x00 : Control signals
//        bit 0  - ap_start (Read/Write/COH)
//        bit 1  - ap_done (Read)
//        bit 2  - ap_idle (Read)
//        bit 3  - ap_ready (Read)
//        bit 4  - ap_continue (Read/Write/SC)
//        bit 7  - auto_restart (Read/Write)
//        others - reserved
// 0x04 : Global Interrupt Enable Register
//        bit 0  - Global Interrupt Enable (Read/Write)
//        others - reserved
// 0x08 : IP Interrupt Enable Register (Read/Write)
//        bit 0  - enable ap_done interrupt (Read/Write)
//        bit 1  - enable ap_ready interrupt (Read/Write)
//        others - reserved
// 0x0c : IP Interrupt Status Register (Read/TOW)
//        bit 0  - ap_done (COR/TOW)
//        bit 1  - ap_ready (COR/TOW)
//        others - reserved
// 0x10 : Data signal of input_data
//        bit 31~0 - input_data[31:0] (Read/Write)
// 0x14 : Data signal of input_data
//        bit 31~0 - input_data[63:32] (Read/Write)
// 0x18 : reserved
// 0x1c : Data signal of input_size
//        bit 31~0 - input_size[31:0] (Read/Write)
// 0x20 : Data signal of input_size
//        bit 31~0 - input_size[63:32] (Read/Write)
// 0x24 : reserved
// 0x28 : Data signal of compressed_data
//        bit 31~0 - compressed_data[31:0] (Read/Write)
// 0x2c : Data signal of compressed_data
//        bit 31~0 - compressed_data[63:32] (Read/Write)
// 0x30 : reserved
// 0x34 : Data signal of compressed_size
//        bit 31~0 - compressed_size[31:0] (Read)
// 0x38 : Data signal of compressed_size
//        bit 31~0 - compressed_size[63:32] (Read)
// 0x3c : reserved
// (SC = Self Clear, COR = Clear on Read, TOW = Toggle on Write, COH = Clear on Handshake)

#define XLZW_ENCODE_HW_CONTROL_ADDR_AP_CTRL              0x00
#define XLZW_ENCODE_HW_CONTROL_ADDR_GIE                  0x04
#define XLZW_ENCODE_HW_CONTROL_ADDR_IER                  0x08
#define XLZW_ENCODE_HW_CONTROL_ADDR_ISR                  0x0c
#define XLZW_ENCODE_HW_CONTROL_ADDR_INPUT_DATA_DATA      0x10
#define XLZW_ENCODE_HW_CONTROL_BITS_INPUT_DATA_DATA      64
#define XLZW_ENCODE_HW_CONTROL_ADDR_INPUT_SIZE_DATA      0x1c
#define XLZW_ENCODE_HW_CONTROL_BITS_INPUT_SIZE_DATA      64
#define XLZW_ENCODE_HW_CONTROL_ADDR_COMPRESSED_DATA_DATA 0x28
#define XLZW_ENCODE_HW_CONTROL_BITS_COMPRESSED_DATA_DATA 64
#define XLZW_ENCODE_HW_CONTROL_ADDR_COMPRESSED_SIZE_DATA 0x34
#define XLZW_ENCODE_HW_CONTROL_BITS_COMPRESSED_SIZE_DATA 64
