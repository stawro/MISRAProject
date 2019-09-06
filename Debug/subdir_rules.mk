################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
%.obj: ../%.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccs901/ccs/tools/compiler/arm_5.1.14/bin/armcl" -mv4 --code_state=32 --abi=ti_arm9_abi --include_path="C:/ti/ccs901/ccs/tools/compiler/arm_5.1.14/include" --misra_required=error --check_misra="all,-1" --misra_advisory=error -g --display_error_number --diag_wrap=off --diag_warning=225 --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


