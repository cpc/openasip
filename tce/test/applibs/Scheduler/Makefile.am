SUBDIRS = ProgramRepresentations Algorithms ResourceManager Selector

clean_gcov:
	@@(for dname in ${SUBDIRS}; do \
		cd $$dname; \
		test -s Makefile && ${MAKE} clean_gcov; \
		cd .. ; \
	done)
