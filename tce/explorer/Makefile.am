BUILD_EXPLORER_PLUGIN = ../src/codesign/Explorer/buildexplorerplugin
basedir = ${prefix}/share/tce/explorer/base

nobase_base_DATA = InitialMachineExplorer.so FrequencySweepExplorer.so SimpleICOptimizer.so RemoveUnconnectedComponents.so MinimizeMachine.so GrowMachine.so

# Force recompilation always, dependency checking does not work here.
.PHONY: InitialMachineExplorer.so FrequencySweepExplorer.so \
SimpleICOptimizer.so RemoveUnconnectedComponents.so MinimizeMachine.so GrowMachine.so

all-local: InitialMachineExplorer.so FrequencySweepExplorer.so SimpleICOptimizer.so RemoveUnconnectedComponents.so MinimizeMachine.so GrowMachine.so

InitialMachineExplorer.so: InitialMachineExplorer.cc
	${BUILD_EXPLORER_PLUGIN} InitialMachineExplorer.so \
	InitialMachineExplorer.cc

FrequencySweepExplorer.so: FrequencySweepExplorer.cc
	${BUILD_EXPLORER_PLUGIN} FrequencySweepExplorer.so \
	FrequencySweepExplorer.cc

SimpleICOptimizer.so: SimpleICOptimizer.cc
	${BUILD_EXPLORER_PLUGIN} SimpleICOptimizer.so \
	SimpleICOptimizer.cc

RemoveUnconnectedComponents.so: RemoveUnconnectedComponents.cc
	${BUILD_EXPLORER_PLUGIN} RemoveUnconnectedComponents.so \
	RemoveUnconnectedComponents.cc

MinimizeMachine.so: MinimizeMachine.cc
	${BUILD_EXPLORER_PLUGIN} MinimizeMachine.so \
	MinimizeMachine.cc

GrowMachine.so: GrowMachine.cc
	${BUILD_EXPLORER_PLUGIN} GrowMachine.so \
	GrowMachine.cc

clean:
	rm -f InitialMachineExplorer.so FrequencySweepExplorer.so SimpleICOptimizer.so RemoveUnconnectedComponents.so MinimizeMachine.so GrowMachine.so

