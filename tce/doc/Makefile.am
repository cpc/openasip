DOXYGEN_CONFIG_FILE = tools/Doxygen/doxygen.config	


if MANUALS
MAKEMANUALS = true
else
MAKEMANUALS = false
endif

all-local: online-manual

online-manual:
	@if test "x${MAKEMANUALS}" = "xtrue" ;then\
	cd man/ProDe && ${MAKE} ProDeManual >& /dev/null;\
	cd ../..; cd man/OSALGUI && ${MAKE} OSALGUIManual >& /dev/null;\
	cd ../..; touch online-manual;\
	fi

MAN_DIR = ${prefix}/share/tce/man
PRODE_MAN_DIR = ${prefix}/share/tce/man/ProDe/
OSED_MAN_DIR = ${prefix}/share/tce/man/OSALGUI

install-data-local:
	@if test "x${MAKEMANUALS}" = "xtrue" ;then\
	mkdir ${MAN_DIR}; mkdir ${PRODE_MAN_DIR};\
	mkdir ${PRODE_MAN_DIR}/ProDeManual;\
	mkdir ${OSED_MAN_DIR}; mkdir ${OSED_MAN_DIR}/OSALGUIManual;\
	cp man/ProDe/ProDe/* ${PRODE_MAN_DIR}/ProDeManual;\
	cp man/OSALGUI/OSALGUI/* ${OSED_MAN_DIR}/OSALGUIManual;\
	fi

all: 
	@(cd .. && doxygen ${DOXYGEN_CONFIG_FILE})

clean-local:
	rm -rf online-manual
