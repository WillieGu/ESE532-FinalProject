# #######################################################################################
# .PHONY: help
# help:
# 	@echo "Makefile Usage:"
# 	@echo ""
# 	@echo "  make all"
# 	@echo "      Command to build client, encoder and decoder."
# 	@echo ""
# 	@echo "  make client"
# 	@echo "      Command to build client."
# 	@echo ""
# 	@echo "  make encoder"
# 	@echo "      Command to build encoder."
# 	@echo ""
# 	@echo "  make decoder"
# 	@echo "      Command to build decoder."
# 	@echo ""
# 	@echo "  make clean"
# 	@echo "      Command to remove the generated files."
# 	@echo ""
# #######################################################################################

# # Compiler tools
# HOST_CXX ?= aarch64-linux-gnu-g++
# RM = rm -f
# RMDIR = rm -rf

# VITIS_PLATFORM = u96v2_sbc_base
# VITIS_PLATFORM_DIR = ${PLATFORM_REPO_PATHS}
# VITIS_PLATFORM_PATH = $(VITIS_PLATFORM_DIR)/u96v2_sbc_base.xpfm

# # Host compiler global settings
# CXXFLAGS += -march=armv8-a+simd -mtune=cortex-a53 -std=c++11 -DVITIS_PLATFORM=$(VITIS_PLATFORM) -D__USE_XOPEN2K8 -I$(XILINX_VIVADO)/include/ -I$(VITIS_PLATFORM_DIR)/sw/u96v2_sbc_base/PetaLinux/sysroot/aarch64-xilinx-linux/usr/include/xrt/ -O3 -g -Wall -c -fmessage-length=0 --sysroot=$(VITIS_PLATFORM_DIR)/sw/u96v2_sbc_base/PetaLinux/sysroot/aarch64-xilinx-linux -I./server
# LDFLAGS += -lxilinxopencl -lpthread -lrt -ldl -lcrypt -lstdc++ \
# -L$(VITIS_PLATFORM_DIR)/sw/u96v2_sbc_base/PetaLinux/sysroot/aarch64-xilinx-linux/usr/lib/ \
# --sysroot=$(VITIS_PLATFORM_DIR)/sw/u96v2_sbc_base/PetaLinux/sysroot/aarch64-xilinx-linux

# #
# # Host files
# #
# CLIENT_SOURCES = Client/client.cpp
# CLIENT_EXE = client

# # Include all necessary source files for the encoder
# SERVER_SOURCES = Server/encoder.cpp Server/server.cpp \
#                  Server/cdc.cpp Server/sha256.cpp \
#                  Server/deduplication.cpp Server/lzw.cpp \

# SERVER_OBJECTS = $(SERVER_SOURCES:.cpp=.o)
# SERVER_EXE = encoder

# DECODER_SOURCES = Decoder/Decoder.cpp
# DECODER_OBJECTS = $(DECODER_SOURCES:.cpp=.o)
# DECODER_EXE = decoder

# all: $(CLIENT_EXE) $(SERVER_EXE) $(DECODER_EXE)

# $(CLIENT_EXE):
# 	g++ -O3 $(CLIENT_SOURCES) -o "$@"

# $(SERVER_EXE): $(SERVER_OBJECTS)
# 	$(HOST_CXX) -o "$@" $(SERVER_OBJECTS) $(LDFLAGS)

# $(DECODER_EXE): $(DECODER_OBJECTS)
# 	$(HOST_CXX) -o "$@" $(DECODER_OBJECTS) $(LDFLAGS)

# # Compile .cpp files into .o files
# %.o: %.cpp
# 	$(HOST_CXX) $(CXXFLAGS) -o "$@" "$<"

# .NOTPARALLEL: clean

# clean:
# 	-$(RM) $(SERVER_EXE) $(SERVER_OBJECTS) $(DECODER_EXE) $(DECODER_OBJECTS) $(CLIENT_EXE)

#######################################################################################
#######################################################################################
.PHONY: help
help:
	@echo "Makefile Usage:"
	@echo ""
	@echo "  make cpu"
	@echo "      Command to build cpu."
	@echo ""
	@echo "  make fpga "
	@echo "      Command to build fpga."
	@echo ""
	@echo "  make host "
	@echo "      Command to build only the host program of fpga."
	@echo ""
	@echo "  make clean "
	@echo "      Command to remove the generated files."
	@echo ""
#######################################################################################

HOST_CXX ?= aarch64-linux-gnu-g++
VPP = ${XILINX_VITIS}/bin/v++
RM = rm -f
RMDIR = rm -rf

VITIS_PLATFORM = u96v2_sbc_base
VITIS_PLATFORM_DIR = ${PLATFORM_REPO_PATHS}
VITIS_PLATFORM_PATH = $(VITIS_PLATFORM_DIR)/u96v2_sbc_base.xpfm
# VITIS_PLATFORM_NAME = $(notdir $(VITIS_PLATFORM_PATH))

# host compiler global settings
CXXFLAGS += -march=armv8-a+simd -march=armv8.2-a+crypto -mtune=cortex-a53 -std=c++11 -DVITIS_PLATFORM=$(VITIS_PLATFORM) -D__USE_XOPEN2K8 -I$(XILINX_VIVADO)/include/ -I$(VITIS_PLATFORM_DIR)/sw/$(VITIS_PLATFORM)/PetaLinux/sysroot/aarch64-xilinx-linux/usr/include/xrt/ -O3 -g -Wall -c -fmessage-length=0 --sysroot=$(VITIS_PLATFORM_DIR)/sw/$(VITIS_PLATFORM)/PetaLinux/sysroot/aarch64-xilinx-linux
LDFLAGS += -lxilinxopencl -lpthread -lrt -ldl -lcrypt -lstdc++ -L$(VITIS_PLATFORM_DIR)/sw/$(VITIS_PLATFORM)/PetaLinux/sysroot/aarch64-xilinx-linux/usr/lib/ --sysroot=$(VITIS_PLATFORM_DIR)/sw/$(VITIS_PLATFORM)/PetaLinux/sysroot/aarch64-xilinx-linux

# hardware compiler shared settings
VPP_OPTS = --target hw


#
# OpenCL kernel files
#
XO := encoder.xo
XCLBIN := encoder.xclbin
ALL_MESSAGE_FILES = $(subst .xo,.mdb,$(XO)) $(subst .xclbin,.mdb,$(XCLBIN))

#
# host files
#
HOST_SOURCES = hls/Host.cpp hls/utils.cpp Server/cdc.cpp Server/sha256.cpp Server/lzw.cpp Server/lzw_hw.cpp Server/deduplication.cpp Server/server.cpp
HOST_OBJECTS =$(HOST_SOURCES:.cpp=.o)
HOST_EXE = encoder

DECODER_SOURCES = Decoder/Decoder.cpp
DECODER_OBJECTS =$(DECODER_SOURCES:.cpp=.o)
DECODER_EXE = decoder

ENCODER_SOURCES = Server/encoder.cpp Server/server.cpp \
                 Server/cdc.cpp Server/sha256.cpp \
                 Server/deduplication.cpp Server/lzw.cpp \

ENCODER_OBJECTS = $(ENCODER_SOURCES:.cpp=.o)
ENCODER_EXE = encoder_sw

CLIENT_SOURCES = Client/client.cpp
CLIENT_OBJECTS =$(DECODER_SOURCES:.cpp=.o)
CLIENT_EXE = client

$(HOST_EXE): $(HOST_OBJECTS)
	$(HOST_CXX) -o "$@" $(+) $(LDFLAGS)

$(DECODER_EXE): $(DECODER_OBJECTS)
	$(HOST_CXX) -o "$@" $(+) $(LDFLAGS)

$(ENCODER_EXE): $(ENCODER_OBJECTS)
	$(HOST_CXX) -o "$@" $(+) $(LDFLAGS)

$(CLIENT_EXE): $(CLIENT_OBJECTS)
	g++ -O3 $(CLIENT_SOURCES) -o "$@"

.cpp.o:
	$(HOST_CXX) $(CXXFLAGS) -I./hls/ -I./Server/ -o "$@" "$<"

#
# primary build targets
#
.PHONY: encoder
encoder: $(HOST_EXE)

.PHONY: decoder
decoder: $(DECODER_EXE)

.PHONY: encoder_sw
encoder_sw: $(ENCODER_EXE)

.PHONY: client
client: $(CLIENT_EXE)

.PHONY: fpga clean
fpga: package/sd_card.img

.NOTPARALLEL: clean


clean-host:
	-$(RM) $(HOST_EXE) $(HOST_OBJECTS) 

clean-decoder:
	-$(RM) $(DECODER_EXE) $(DECODER_OBJECTS) 

clean-encoder-sw:
	-$(RM) $(ENCODER_EXE) $(ENCODER_OBJECTS) 

clean-accelerators:
	-$(RM) $(XCLBIN) $(XO) $(ALL_MESSAGE_FILES)
	-$(RM) *.xclbin.sh *.xclbin.info *.xclbin.link_summary* *.compile_summary
	-$(RMDIR) .Xil ./hls/proj_mmult

clean-package:
	-${RMDIR} package
	-${RMDIR} package.build

clean: clean-host clean-accelerators clean-package
	-$(RM) *.log *.package_summary
	-${RMDIR} _x .ipcache

#
# binary container: kernel.xclbin
#

$(XO): ./Server/lzw_hw.cpp
	-@$(RM) $@
	$(VPP) $(VPP_OPTS) --platform $(VITIS_PLATFORM_PATH) -k lzw_encode_hw --compile -I"$(<D)" --config ./hls/design.cfg -o"$@" "$<"

$(XCLBIN): $(XO)
	$(VPP) $(VPP_OPTS) --platform $(VITIS_PLATFORM_PATH) --link --config ./hls/design.cfg -o"$@" $(+)

package/sd_card.img: $(HOST_EXE) $(XCLBIN) ./hls/xrt.ini
	$(VPP) --package $(VPP_OPTS) --config ./hls/package.cfg $(XCLBIN) \
		--package.out_dir package \
		--package.sd_file $(HOST_EXE) \
		--package.kernel_image $(PLATFORM_REPO_PATHS)/sw/$(VITIS_PLATFORM)/PetaLinux/image/image.ub \
		--package.rootfs $(PLATFORM_REPO_PATHS)/sw/$(VITIS_PLATFORM)/PetaLinux/rootfs/rootfs.ext4 \
		--package.sd_file $(XCLBIN) \
		--package.sd_file ./hls/xrt.ini

