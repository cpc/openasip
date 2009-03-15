DOXYGEN_CONFIG_FILE = tools/Doxygen/doxygen.config	

MAN_DIR = ${prefix}/share/tce/man

EXTRA_DIST = latexdefs misc man/TCE

doxygen-docs: 
	@(cd .. && doxygen ${DOXYGEN_CONFIG_FILE})

clean-local:
	rm -rf online-manual
