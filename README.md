# OpenASIP - Open Application-Specific Instruction-set Processor toolset

OpenASIP is an open application-specific instruction-set processor (ASIP)
toolset for design and programming of customized co-processors (typically
programmable accelerators).

The toolset provides a complete retargetable co-design flow from high-level
language programs down to FPGA/ASIC synthesizable processor RTL (VHDL and
Verilog generation supported) and instruction-parallel program binaries.

Processor customization points include the register files, function units,
supported operations, and the datapath interconnection network.

The internal processor template of OpenASIP is based on the energy efficient and
modular Transport Triggered Architecture (TTA), which is still its default
target programming model for static multi-issue designs. OpenASIP, however, also has initial support for other programming models such
as standard operation-based VLIW (demonstrated in Blocks CGRA) and since 2.0
it received the first features to support customizing RISC-V ISA based
processors.

OpenASIP has been developed by several researchers (and research assistants) of
Tampere University (Finland) and various other international contributors
since the early 2003.

Links:

 * [The toolset web pages](http://openasip.org)
 * [Customized Parallel Computing group's home page](http://cpc.cs.tut.fi). CPC leads the development of the tools.

(*) OpenASIP was previously called TTA-based Co-Design Environment (TCE).
After adding support for other ISAs than TTAs, its name was changed to a more
general one to describe its wider utility.
