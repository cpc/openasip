SUBDIRS = tceopgen llvm-tce riscv-tdgen

bin_SCRIPTS = oacc oacc-riscv
EXTRA_DIST = ${bin_SCRIPTS}

all-local:
	+./oacc-riscv --generate-plugin --tce-build-mode

.PHONY: all-local
