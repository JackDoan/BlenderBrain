################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
init.obj: ../init.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"/home/jack/ti/ccsv6/tools/compiler/ti-cgt-c2000_6.4.6/bin/cl2000" -v28 -mt -ml --include_path="/home/jack/ti/workspace_v6_1/BlenderBrain/libF28027" --include_path="/home/jack/ti/workspace_v6_1/BlenderBrain/libF28027/include" --preproc_with_compile --preproc_dependency="init.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

libSCI.obj: ../libSCI.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"/home/jack/ti/ccsv6/tools/compiler/ti-cgt-c2000_6.4.6/bin/cl2000" -v28 -mt -ml --include_path="/home/jack/ti/workspace_v6_1/BlenderBrain/libF28027" --include_path="/home/jack/ti/workspace_v6_1/BlenderBrain/libF28027/include" --preproc_with_compile --preproc_dependency="libSCI.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

main.obj: ../main.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"/home/jack/ti/ccsv6/tools/compiler/ti-cgt-c2000_6.4.6/bin/cl2000" -v28 -mt -ml --include_path="/home/jack/ti/workspace_v6_1/BlenderBrain/libF28027" --include_path="/home/jack/ti/workspace_v6_1/BlenderBrain/libF28027/include" --preproc_with_compile --preproc_dependency="main.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '


