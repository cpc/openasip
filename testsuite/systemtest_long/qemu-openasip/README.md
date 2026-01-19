# tcetest_qemu-openasip.py

tcetest_qemu-openasip_integration.py compiles and runs a small program in QEMU
that uses custom OSAL instructions. You should have built QEMU-OpenASIP beforehand,
for example with a script from tools/scripts/install_qemu.py

By default the test assumes that QEMU is installed in $HOME/qemu-openasip, but
a new path can be provided using OPENASIP_QEMU_PATH environment variable

You should also have OpenASIP installed and libopenasip.so in your LD_LIBRARY_PATH.
