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

In case you use OpenASIP in your own research, please cite one or more of the following papers
that is/are the most relevant to the topic (overall TCE, softcores/FPGA or RISC-V customization):

@Inbook{TCEToolset,  
&nbsp;&nbsp;  author="J{\"a}{\"a}skel{\"a}inen, Pekka and Viitanen, Timo and Takala, Jarmo and Berg,
Heikki",  
&nbsp;&nbsp;  editor="Hussain, Waqar and Nurmi, Jari and Isoaho, Jouni and Garzia, Fabio",  
&nbsp;&nbsp;  title="HW/SW Co-design Toolset for Customization of Exposed Datapath Processors",  
&nbsp;&nbsp;  bookTitle="Computing Platforms for Software-Defined Radio",  
&nbsp;&nbsp;  year="2017",  
&nbsp;&nbsp;  publisher="Springer International Publishing",  
&nbsp;&nbsp;  pages="147--164",  
&nbsp;&nbsp;  isbn="978-3-319-49679-5",  
&nbsp;&nbsp;  doi="10.1007/978-3-319-49679-5_8",  
&nbsp;&nbsp;  url="https://doi.org/10.1007/978-3-319-49679-5_8"  
}  

@inproceedings{esko2010customized,  
&nbsp;&nbsp;  title={Customized exposed datapath soft-core design flow with compiler support},  
&nbsp;&nbsp;  author={Esko, Otto and J{\"a}{\"a}skelainen, Pekka and Huerta, Pablo and Carlos, S and Takala, Jarmo and Martinez, Jose Ignacio},  
&nbsp;&nbsp;  booktitle={2010 International Conference on Field Programmable Logic and Applications},  
&nbsp;&nbsp;  pages={217--222},  
&nbsp;&nbsp;  year={2010},  
&nbsp;&nbsp;  organization={IEEE}  
}

@inproceedings{OpenASIP2_0,  
&nbsp;&nbsp;title={OpenASIP 2.0: Co-Design Toolset for RISC-V Application-Specific Instruction-Set Processors},  
&nbsp;&nbsp;author={Hepola, Kari and Multanen, Joonas and J{\"a}{\"a}skel{\"a}inen, Pekka},  
&nbsp;&nbsp;booktitle={2022 IEEE 33rd International Conference on Application-specific Systems, Architectures and Processors (ASAP)},  
&nbsp;&nbsp;pages={161--165},  
&nbsp;&nbsp;year={2022},  
&nbsp;&nbsp;organization={IEEE}  
}