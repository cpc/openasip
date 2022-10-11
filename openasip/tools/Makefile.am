datadir = ${prefix}/share/openasip

nobase_data_DATA = ${srcdir}/scripts/install_llvm_*.sh \
	${srcdir}/patches/*.patch

EXTRA_DIST = ${nobase_data_DATA}

install-data-hook: $(pkgdata_SCRIPTS)
	chmod +x $(DESTDIR)/${datadir}/scripts/*

