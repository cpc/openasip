SUBDIRS = Simulator Disassembler bem Assembler hdb FSA \
Interpreter Scheduler costdb Explorer dsdb TraceDB mach osal

if WX

SUBDIRS += wxToolkit

endif


clean_gcov:
	@@(for dname in ${SUBDIRS}; do \
		cd $$dname; \
		test -s Makefile && ${MAKE} clean_gcov; \
		cd .. ; \
	done)
