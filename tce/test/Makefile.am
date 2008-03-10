SUBDIRS=base tools procgen applibs

clean_gcov: 
	@@(for dname in ${SUBDIRS}; do \
		cd $$dname; \
		test -s Makefile && ${MAKE} clean_gcov; \
		cd .. ; \
	done)

