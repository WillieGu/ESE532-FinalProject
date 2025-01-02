############################################################
## This file is generated automatically by Vitis HLS.
## Please DO NOT edit it.
## Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
############################################################
open_project new_lzw
set_top lzw_encode_hw
add_files Server/lzw_hw.h
add_files Server/lzw_hw.cpp
add_files Server/lzw.h
add_files Server/lzw.cpp
add_files -tb Server/lzw_testbench.cpp -cflags "-Wno-unknown-pragmas" -csimflags "-Wno-unknown-pragmas"
open_solution "solution1" -flow_target vitis
set_part {xczu3eg-sbva484-1-i}
create_clock -period 6.67 -name default
config_compile -pipeline_loops 0
config_interface -m_axi_alignment_byte_size 64 -m_axi_latency 64 -m_axi_max_widen_bitwidth 512 -m_axi_offset slave
config_rtl -register_reset_num 3
config_export -format xo -output /mnt/castor/seas_home/t/truongng/ese532_group_project/lzw_encode_hw.xo -rtl verilog
source "./new_lzw/solution1/directives.tcl"
csim_design
csynth_design
cosim_design
export_design -rtl verilog -format xo -output /mnt/castor/seas_home/t/truongng/ese532_group_project/lzw_encode_hw.xo
