SUBDIRS = ProgramDependenceGraph ControlFlowGraph ControlDependenceGraph \
	DataDependenceGraph

lib_LTLIBRARIES = libprogramrepresentations.la

libprogramrepresentations_la_SOURCES = 
libprogramrepresentations_la_LIBADD = ProgramDependenceGraph/libprogramdependencegraph.la ControlFlowGraph/libcfg.la ControlDependenceGraph/libcdg.la DataDependenceGraph/libddg.la

EXTRA_DIST = ProgramDependenceGraph ControlFlowGraph ControlDependenceGraph \
		DataDependenceGraph
