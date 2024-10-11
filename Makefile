TARGET_EXEC ?= MW601KB.elf

PREFIX := /home/xinjie/ThirdParty/MRS_Toolchain_Linux_x64_V1.92/RISC-V_Embedded_GCC12/bin/

AS := $(PREFIX)riscv-none-elf-gcc
CC := $(PREFIX)riscv-none-elf-gcc
CXX := $(PREFIX)riscv-none-elf-g++
AR := $(PREFIX)riscv-none-elf-ar

BUILD_DIR ?= ./build
SRC_DIRS ?= ./src ./vendor/Core ./vendor/Debug ./vendor/Peripheral ./vendor/User

SRCS := $(shell find $(SRC_DIRS) -name *.cpp -or -name *.c -or -name *.S)
SRCS += \
./CherryUSB/core/usbd_core.c \
./CherryUSB/port/ch32/usb_dc_usbhs.c \
./CherryUSB/class/cdc/usbd_cdc.c \
./CherryUSB/class/hid/usbd_hid.c \
./vendor/Startup/startup_ch32v30x_D8C.S


OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_DIRS += \
./CherryUSB/port/ch32 \
./CherryUSB/core \
./CherryUSB/common \
./CherryUSB/class/cdc \
./CherryUSB/class/hid \


INC_FLAGS := $(addprefix -I,$(INC_DIRS))


FLAGS ?= -march=rv32imafc -mabi=ilp32f -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized -DCONFIG_USB_HS -g
ASFLAGS ?= $(FLAGS) -x assembler $(INC_FLAGS) -MMD -MP
CPPFLAGS ?=  $(FLAGS) $(INC_FLAGS) -std=gnu99 -MMD -MP
LDFLAGS ?= $(FLAGS) -T ./vendor/Ld/Link.ld -nostartfiles -Xlinker --gc-sections -Wl,-Map,"$(BUILD_DIR)/CH32V307RCT6.map" --specs=nano.specs --specs=nosys.specs




$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

# assembly
$(BUILD_DIR)/%.S.o: %.S
	$(MKDIR_P) $(dir $@)
	$(CC) $(ASFLAGS) -c $< -o $@

# c source
$(BUILD_DIR)/%.c.o: %.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# c++ source
$(BUILD_DIR)/%.cpp.o: %.cpp
	$(MKDIR_P) $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@


.PHONY: clean

clean:
	$(RM) -r $(BUILD_DIR)

-include $(DEPS)

MKDIR_P ?= mkdir -p
