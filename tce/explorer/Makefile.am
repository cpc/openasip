BUILD_EXPLORER_PLUGIN = ../src/codesign/Explorer/buildexplorerplugin
basedir = ${prefix}/share/tce/explorer/base

nobase_base_DATA = FrequencySweepExplorer.so SimpleICOptimizer.so RemoveUnconnectedComponents.so MinimizeMachine.so GrowMachine.so ImmediateGenerator.so ImplementationSelector.so ComponentAdder.so Evaluate.so MinimalOpSet.so

# Force recompilation always, dependency checking does not work here.
.PHONY: FrequencySweepExplorer.so \
SimpleICOptimizer.so RemoveUnconnectedComponents.so MinimizeMachine.so \
GrowMachine.so ImmediateGenerator.so ImplementationSelector.so \
ComponentAdder.so Evaluate.so MinimalOpSet.so

all-local: FrequencySweepExplorer.so SimpleICOptimizer.so RemoveUnconnectedComponents.so MinimizeMachine.so GrowMachine.so ImmediateGenerator.so ImplementationSelector.so ComponentAdder.so Evaluate.so MinimalOpSet.so

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

ImmediateGenerator.so: ImmediateGenerator.cc
	${BUILD_EXPLORER_PLUGIN} ImmediateGenerator.so \
	ImmediateGenerator.cc

ImplementationSelector.so: ImplementationSelector.cc
	${BUILD_EXPLORER_PLUGIN} ImplementationSelector.so \
	ImplementationSelector.cc

ComponentAdder.so: ComponentAdder.cc
	${BUILD_EXPLORER_PLUGIN} ComponentAdder.so \
	ComponentAdder.cc

Evaluate.so: Evaluate.cc
	${BUILD_EXPLORER_PLUGIN} Evaluate.so \
	Evaluate.cc

MinimalOpSet.so: MinimalOpSet.cc
	${BUILD_EXPLORER_PLUGIN} MinimalOpSet.so \
	MinimalOpSet.cc

clean:
	rm -f FrequencySweepExplorer.so SimpleICOptimizer.so RemoveUnconnectedComponents.so MinimizeMachine.so GrowMachine.so ImmediateGenerator.so ImplementationSelector.so ComponentAdder.so Evaluate.so MinimalOpSet.so

