################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
MCPara/Source/MCPara_EEPROM.obj: ../MCPara/Source/MCPara_EEPROM.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"D:/Software/ti/CCS6.2/ccsv6/tools/compiler/c2000_15.12.3.LTS/bin/cl2000" -v28 -ml -mt --float_support=fpu32 --include_path="D:/Software/ti/CCS6.2/ccsv6/tools/compiler/c2000_15.12.3.LTS/include" --include_path="D:/My_Pjt/DSPCoreV1-hanxun/MCCore/Include" --include_path="D:/My_Pjt/DSPCoreV1-hanxun/MCPara/Include" --include_path="D:/My_Pjt/DSPCoreV1-hanxun/MCSoft/Include" --include_path="D:/My_Pjt/DSPCoreV1-hanxun/MC_28335/Include" -g --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="MCPara/Source/MCPara_EEPROM.d" --obj_directory="MCPara/Source" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

MCPara/Source/MCPara_LoadSave.obj: ../MCPara/Source/MCPara_LoadSave.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"D:/Software/ti/CCS6.2/ccsv6/tools/compiler/c2000_15.12.3.LTS/bin/cl2000" -v28 -ml -mt --float_support=fpu32 --include_path="D:/Software/ti/CCS6.2/ccsv6/tools/compiler/c2000_15.12.3.LTS/include" --include_path="D:/My_Pjt/DSPCoreV1-hanxun/MCCore/Include" --include_path="D:/My_Pjt/DSPCoreV1-hanxun/MCPara/Include" --include_path="D:/My_Pjt/DSPCoreV1-hanxun/MCSoft/Include" --include_path="D:/My_Pjt/DSPCoreV1-hanxun/MC_28335/Include" -g --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="MCPara/Source/MCPara_LoadSave.d" --obj_directory="MCPara/Source" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


