################################################################################
# Automatically-generated file. Do not edit!
################################################################################

CG_TOOL_ROOT := /Applications/ti/ccs1040/ccs/tools/compiler/msp430-gcc-9.3.1.11_macos

GEN_OPTS__FLAG := 
GEN_CMDS__FLAG := 

ORDERED_OBJS += \
"./main.o" \
"./app/sort/sort.o" \
"./driverlib/MSP430FR5xx_6xx/crc.o" \
"./driverlib/MSP430FR5xx_6xx/crc32.o" \
"./driverlib/MSP430FR5xx_6xx/cs.o" \
"./driverlib/MSP430FR5xx_6xx/dma.o" \
"./driverlib/MSP430FR5xx_6xx/eusci_a_uart.o" \
"./driverlib/MSP430FR5xx_6xx/gpio.o" \
"./kernel/src/checkpointing.o" \
"./kernel/src/scheduler.o" \
"./kernel/src/sram_checker.o" \
"./kernel/src/task.o" \
"./profile/profile.o" \
-T"../msp430fr5994.ld" \
$(GEN_CMDS__FLAG) \
-Wl,--start-group \
-lgcc \
-lc \
-Wl,--end-group \

-include ../makefile.init

RM := rm -rf
RMDIR := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include app/sort/subdir_vars.mk
-include driverlib/MSP430FR5xx_6xx/subdir_vars.mk
-include kernel/src/subdir_vars.mk
-include profile/subdir_vars.mk
-include subdir_rules.mk
-include app/sort/subdir_rules.mk
-include driverlib/MSP430FR5xx_6xx/subdir_rules.mk
-include kernel/src/subdir_rules.mk
-include profile/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(SV7A_DEPS)),)
-include $(SV7A_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
tides_FR5994.out \

EXE_OUTPUTS__QUOTED += \
"tides_FR5994.out" \

BIN_OUTPUTS += \
tides_FR5994.hex \

BIN_OUTPUTS__QUOTED += \
"tides_FR5994.hex" \


# All Target
all: $(OBJS) $(CMD_SRCS) $(LD_SRCS) $(GEN_CMDS)
	@$(MAKE) --no-print-directory -Onone "tides_FR5994.out"

# Tool invocations
tides_FR5994.out: $(OBJS) $(CMD_SRCS) $(LD_SRCS) $(GEN_CMDS)
	@echo 'Building target: "$@"'
	@echo 'Invoking: GNU Linker'
	"/Applications/ti/ccs1040/ccs/tools/compiler/msp430-gcc-9.3.1.11_macos/bin/msp430-elf-gcc-9.3.1" -mhwmult=f5series -Os -Wall -mcode-region=none -mlarge -Wl,-Map,"tides_FR5994.map" -Wl,--gc-sections -L"/Applications/ti/ccs1040/ccs/ccs_base/msp430/include_gcc" -mmcu=msp430fr5994 -o"tides_FR5994.out" $(ORDERED_OBJS)
	@echo 'Finished building target: "$@"'
	@echo ' '

tides_FR5994.hex: $(EXE_OUTPUTS)
	@echo 'Building secondary target: "$@"'
	@echo 'Invoking: GNU Objcopy Utility'
	"/Applications/ti/ccs1040/ccs/tools/compiler/msp430-gcc-9.3.1.11_macos/bin/msp430-elf-objcopy"  $(EXE_OUTPUTS__QUOTED) "tides_FR5994.hex"
	@echo 'Finished building secondary target: "$@"'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(BIN_OUTPUTS__QUOTED)$(EXE_OUTPUTS__QUOTED)
	-$(RM) "main.o" "app/sort/sort.o" "driverlib/MSP430FR5xx_6xx/crc.o" "driverlib/MSP430FR5xx_6xx/crc32.o" "driverlib/MSP430FR5xx_6xx/cs.o" "driverlib/MSP430FR5xx_6xx/dma.o" "driverlib/MSP430FR5xx_6xx/eusci_a_uart.o" "driverlib/MSP430FR5xx_6xx/gpio.o" "kernel/src/checkpointing.o" "kernel/src/scheduler.o" "kernel/src/sram_checker.o" "kernel/src/task.o" "profile/profile.o" 
	-$(RM) "main.d" "app/sort/sort.d" "driverlib/MSP430FR5xx_6xx/crc.d" "driverlib/MSP430FR5xx_6xx/crc32.d" "driverlib/MSP430FR5xx_6xx/cs.d" "driverlib/MSP430FR5xx_6xx/dma.d" "driverlib/MSP430FR5xx_6xx/eusci_a_uart.d" "driverlib/MSP430FR5xx_6xx/gpio.d" "kernel/src/checkpointing.d" "kernel/src/scheduler.d" "kernel/src/sram_checker.d" "kernel/src/task.d" "profile/profile.d" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

