# OpenASIP - Open Application-Specific Instruction-set Processor

OpenASIP is an open application-specific instruction-set processor (ASIP)
toolset for design and programming of customized co-processors (typically
programmable accelerators).

The internal processor template is based on the energy efficient Transport
Triggered Architecture (TTA) which is still also its primary target.

OpenASIP was previously called TTA-based Co-Design Environment (TCE),
but after adding support for customization of RISC-V ISA based processors
and more traditional VLIWs (Blocks CGRA), it was renamed to a more
general descriptive name.

The toolset provides a complete retargetable co-design flow from high-level
language programs down to FPGA/ASIC synthesizable processor RTL (VHDL and
Verilog generation supported) and parallel program binaries.

Processor customization points include the register files, function units,
supported operations, and the interconnection network.

OpenASIP has been developed by several researchers (and research assistants) of
Tampere University (Finland) and various other international contributors
since the early 2003.

Links:

 * [The toolset web pages](http://openasip.org)
 * [Customized Parallel Computing group's home page](http://cpc.cs.tut.fi). CPC leads the development of the tools.
