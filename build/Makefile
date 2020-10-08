## Author: Yago Teodoro de Mello
## yago.t.mello@gmail.com
## V1.0.0
## 2020-10-07
## LICENSE: MIT

MAKEFLAGS += --warn-undefined-variables
SHELL := bash
.SHELLFLAGS := -eu -o pipefail -c
.DELETE_ON_ERROR:
.SUFFIXES:
.PHONY: release debug clean

# UNCOMMENT MODEL
#LDS = GD32VF103x4.lds
#LDS = GD32VF103x6.lds
#LDS = GD32VF103x8.lds
LDS = GD32VF103xB.lds

# TOOLCHAIN
TOOLCHAIN_PREFIX = riscv32-unknown-elf-
TOOLCHAIN_FOLDER = /opt/riscv-gcc/gd32vf103/bin/

# EXECUTABLE
NAME = prog

# GCC toolchain programs.
CC  = $(TOOLCHAIN_FOLDER)/$(TOOLCHAIN_PREFIX)gcc
CXX = $(TOOLCHAIN_FOLDER)/$(TOOLCHAIN_PREFIX)g++
OC  = $(TOOLCHAIN_FOLDER)/$(TOOLCHAIN_PREFIX)objcopy
OS  = $(TOOLCHAIN_FOLDER)/$(TOOLCHAIN_PREFIX)size

# Folders
BIN = ../bin
SRC = ../src
EXT = ../ext
TMP = ../build
FW  = ../Firmware

# Source files.
ASM      += $(wildcard $(SRC)/.S $(SRC)/.s)
ASM      += $(EXT)/*.S
CODE_C   += $(wildcard $(SRC)/*.c)
CODE_CPP += $(wildcard $(SRC)/*.cpp $(SRC)/*.cc)
FIRM     += $(EXT)/*.c
FIRM     += $(FW)/RISCV/stubs/*.c 
FIRM     += $(FW)/RISCV/drivers/*.c 
FIRM     += $(FW)/GD32VF103_standard_peripheral/*.c 
FIRM     += $(FW)/GD32VF103_standard_peripheral/Source/*.c

# Arch
MCUFLAGS += -march=rv32imac
MCUFLAGS += -mabi=ilp32
MCUFLAGS += -mcmodel=medlow

# Optimization
DBGFLAGS += -Og
DBGFLAGS += -g3
DBGFLAGS += -D_FORTIFY_SOURCE=2

RELFLAGS += -O3
RELFLAGS += -flto
RELFLAGS += -fuse-linker-plugin

# Assembly directives.
ASFLAGS += -c
ASFLAGS += -O0
ASFLAGS += -Wall
ASFLAGS += -Wextra
ASFLAGS += -x assembler-with-cpp

# C compilation directives
CFLAGS += -c
CFLAGS += -Wall
CFLAGS += -Wextra
CFLAGS += -Wno-unused-variable
CFLAGS += -Wno-unused-parameter
CFLAGS += --specs=nosys.specs
CFLAGS += -flto -fuse-linker-plugin

# CXX compilation directives
CXXFLAGS += -std=c++17
CXXFLAGS += -c
CXXFLAGS += -Wall
CXXFLAGS += -Wextra
CXXFLAGS += -Wno-unused-variable
CXXFLAGS += -Wno-unused-parameter
CXXFLAGS += --specs=nosys.specs

# DEBUG Linker directives.
DLFLAGS += -Wall
DLFLAGS += -Wextra
DLFLAGS += -Wl,--no-relax
DLFLAGS += -Wl,--gc-sections
DLFLAGS += -nostartfiles
DLFLAGS += -lc
DLFLAGS += -lgcc
DLFLAGS += -lstdc++
DLFLAGS += --specs=nosys.specs
DLFLAGS += -T$(EXT)/$(LDS)

# RELEASE Linker directives
RLFLAGS += -Wall
RLFLAGS += -Wextra
RLFLAGS += -Wl,--relax
RLFLAGS += -Wl,--gc-sections
RLFLAGS += -nostartfiles
RLFLAGS += -lc
RLFLAGS += -lgcc
RLFLAGS += -lstdc++
RLFLAGS += --specs=nosys.specs
RLFLAGS += -flto -fuse-linker-plugin
RLFLAGS += -T$(EXT)/$(LDS)

# Header file directories.
INCLUDE += -I$(EXT)/
INCLUDE += -I$(FW)/RISCV/stubs/
INCLUDE += -I$(FW)/RISCV/drivers/
INCLUDE += -I$(FW)/GD32VF103_standard_peripheral/
INCLUDE += -I$(FW)/GD32VF103_standard_peripheral/Include/

release:
	$(CC)  $(MCUFLAGS) $(ASFLAGS)  $(RELFLAGS) $(INCLUDE) $(ASM)
	$(CC)  $(MCUFLAGS) $(CFLAGS)   $(RELFLAGS) $(INCLUDE) $(FIRM) $(CODE_C)
	$(CXX) $(MCUFLAGS) $(CXXFLAGS) $(RELFLAGS) $(INCLUDE) $(CODE_CPP)
	$(CXX) $(MCUFLAGS) $(RLFLAGS)  *.o -o      $(BIN)/$(NAME).elf
	$(OC) -S -O binary $(BIN)/$(NAME).elf      $(BIN)/$(NAME).bin
	@echo  ''
	@file  $(BIN)/$(NAME).elf
	@$(OS) $(BIN)/$(NAME).elf
	@echo  ''

debug:
	$(CC)  $(MCUFLAGS) $(ASFLAGS)  $(DBGFLAGS) $(INCLUDE) $(ASM)
	$(CC)  $(MCUFLAGS) $(CFLAGS)   $(DBGFLAGS) $(INCLUDE) $(FIRM) $(CODE_C)
	$(CXX) $(MCUFLAGS) $(CXXFLAGS) $(DBGFLAGS) $(INCLUDE) $(CODE_CPP)
	$(CXX) $(MCUFLAGS) $(DLFLAGS)  *.o -o      $(BIN)/$(NAME).elf
	$(OC) -S -O binary $(BIN)/$(NAME).elf      $(BIN)/$(NAME).bin
	@echo  ''
	@file  $(BIN)/$(NAME).elf
	@$(OS) $(BIN)/$(NAME).elf
	@echo  ''

clean:
	rm -f *.o $(BIN)/$(NAME).elf $(BIN)/$(NAME).bin
