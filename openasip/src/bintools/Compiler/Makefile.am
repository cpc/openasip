SUBDIRS = tceopgen llvm-tce

if RISCV
SUBDIRS += riscv-tdgen
endif

bin_SCRIPTS = oacc

if RISCV
bin_SCRIPTS += oacc-riscv
endif

EXTRA_DIST = ${bin_SCRIPTS}

if RISCV
all-local:
	+./oacc-riscv --generate-plugin --tce-build-mode
endif

.PHONY: all-local
