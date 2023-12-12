################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
app/sort/%.o: ../app/sort/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: GNU Compiler'
	"/Applications/ti/ccs1040/ccs/tools/compiler/msp430-gcc-9.3.1.11_macos/bin/msp430-elf-gcc-9.3.1" -c -mmcu=msp430fr5994 -mhwmult=f5series -I"/Applications/ti/ccs1040/ccs/ccs_base/msp430/include_gcc" -I"/Users/liusongran/MyProject/Intermittent System/tides_rt/tides_msp430_fr5994" -I"/Applications/ti/ccs1040/ccs/tools/compiler/msp430-gcc-9.3.1.11_macos/msp430-elf/include" -Os -Wall -mlarge -mcode-region=none -mdata-region=lower -MMD -MP -MF"app/sort/$(basename $(<F)).d_raw" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


