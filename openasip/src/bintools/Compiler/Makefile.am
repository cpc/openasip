SUBDIRS=tceopgen llvm-tce riscv-tdgen

bin_SCRIPTS = oacc oacc-riscv
EXTRA_DIST = ${bin_SCRIPTS}

# Generate the default plugin here so that
# it is not done multiple times in parallel when compiling newlib
post-build: riscv-tdgen
	./oacc-riscv --generate-plugin

all: post-build

.PHONY: post-build
