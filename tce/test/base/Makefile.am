SUBDIRS = program tpef mach memory bem idf umach Graph

if HAVE_RDYNAMIC
SUBDIRS += osal
endif

clean_gcov:
	@@(for dname in ${SUBDIRS}; do \
		cd $$dname; \
		test -s Makefile && ${MAKE} clean_gcov; \
		cd .. ; \
	done)
