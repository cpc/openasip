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

Supported Operating Systems
===========================

OpenASIP requires a Unix-style operating system such as Linux. Debian-based
distributions like Debian and Ubuntu should have most of the required
libraries included, but other recent distribution versions should work
fine too. MacOS support is experimental.

The following installation steps install prerequisities, the OpenASIP-patched
LLVM and OpenASIP to in $HOME/local.

Installing Prerequisities
=========================

This step needs root/admin privileges.

For Ubuntus, you need to enable the universe and multiverse repositories first:
https://help.ubuntu.com/community/Repositories/Ubuntu#Adding_Repositories_in_Ubuntu

Ubuntu 20.04+ / Debian 11
-------------------------

&nbsp; _sudo apt-get install libwxgtk3.0-gtk3-dev libboost-all-dev \  <br>
&nbsp; tcl8.6-dev libedit-dev libsqlite3-dev sqlite3 libxerces-c-dev g++ make \ <br>
&nbsp; latex2html libffi-dev autoconf automake libtool subversion git cmake graphviz_

Ubuntu 18.04 LTS and older
--------------------------

Not supported anymore due to SQLite version too low (currently requires 3.25+)

Debian 10
---------

&nbsp; _sudo apt-get install libwxgtk3.0-dev libboost-{filesystem,graph,regex,thread}-dev \ <br>
&nbsp; tcl8.6-dev libedit-dev libsqlite3-dev sqlite3 libxerces-c-dev g++ make latex2html \ <br>
&nbsp; libffi-dev autoconf automake libtool subversion git cmake_

Debian 9 and older
------------------

Not supported anymore due to SQLite version too low (currently requires 3.25+)

Red Hat Enterprise Linux 8 & clones
-----------------------------------

&nbsp; _sudo yum install -y wxGTK3-devel boost-devel tcl-devel libedit-devel \ <br>
&nbsp; sqlite-devel xerces-c-devel gcc-c++ make autoconf automake libtool git svn_

Get the Latest OpenASIP Source Code
==============================

&nbsp; _git clone https://github.com/cpc/openasip.git openasip-devel_

Alternatively you can get one of the release branches, e.g.:

&nbsp; _git clone -b openasip-2.0 https://github.com/cpc/openasip.git openasip-2.0_

LLVM and Clang
==============

In order to compile programs from C/C++ with OpenASIP, you need to install
LLVM and Clang with a few additional patches and use correct LLVM build
switches to fix some issues affecting OpenASIP usage.

You can do this easiest as follows:

&nbsp; _cd openasip-2.0/openasip OR cd openasip-devel/openasip_ <br>
&nbsp; _tools/scripts/install_llvm_15.sh $HOME/local_  

After successful installation, add information about installed libraries to the
search path. We assume OpenASIP will be installed to the same location later.

&nbsp; export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$HOME/local/lib <br>
&nbsp; export PATH=$HOME/local/bin:$PATH <br> 
&nbsp; export LDFLAGS=-L$HOME/local/lib  

You might want to put the above environment variable modification commands to
your ~/.bashrc or similar so they are taken in effect automatically whenever
you start a new shell. Otherwise you need to remember to enter them
before attempting to use LLVM or OpenASIP.

Notice that OpenASIP and LLVM installation folder comes first in the path variable.
This is to prevent OpenASIP of using system's LLVM installation. This might
interfere with other tools in your system which rely on using system's LLVM
installation. If this is the case, then better option is to put above lines to
a separate tce-env.sh file and place this file somewhere in your path or home
directory. Now when you open a new shell and need to use OpenASIP, just source this
file to your shell with:

&nbsp; _source tce-env.sh_

RISC-V
------
If you wish to build OpenASIP with support for RISC-V, you need to install the
following prerequisites before installing OpenASIP.

You need to install the RISC-V GNU Toolchain (version 12.1.0+)
which brings you the common bintools:

&nbsp; https://github.com/riscv-collab/riscv-gnu-toolchain

Note: If you are using the the RISCV GNU Toolchain with OpenASIP for other
purposes, the toolchain must be built with the RV32IM target as OpenASIP does
not currently support the RISC-V C-extension. Thus, you need to configure the
toolchain build with commands like the following:

&nbsp; _./configure --prefix=/opt/riscv --with-arch=rv32im_ <br>
&nbsp; _make_

In addition, the elf2hex tool is needed to produce program hex images
from the ELF files as follows:

&nbsp; _git clone https://github.com/sifive/elf2hex.git <br>
&nbsp; cd elf2hex <br>
&nbsp; autoreconf -i <br>
&nbsp; ./configure --prefix=/opt/riscv --target=riscv32-unknown-elf <br>
&nbsp; make <br>
&nbsp; make install_

Add the install bin directory to the $PATH environment. If you installed
the tools to /opt/riscv, add:

&nbsp; export PATH=/opt/riscv/bin:$PATH

Building and Installing OpenASIP
===========================

In the root of OpenASIP sources (e.g. openasip-devel/openasip), run:

&nbsp; _./autogen.sh && ./configure --prefix=$HOME/local && make -j8 && make install_

Now OpenASIP commands such as 'ttasim' should work. Check this with:

&nbsp; _ttasim --version_

Now try to run the smoke test script to see if it finds any problems with
your installation:

Then run the smoke test script:

&nbsp; _oa-selftest -v_

If this finished correctly, you are all set! For learning to use OpenASIP, a good
way is to start with the OpenASIP user manual's (openasip/manual/OpenASIP_manual.pdf)
tutorials, e.g., the "OpenASIP tour"
that goes through the basic TTA customization aspects. The RISC-V customization
features are demonstrated in the "RISC-V Tutorial".

Upgrading OpenASIP
=============

Later on, if you want to update your OpenASIP installation with the latest changes
committed in the version control system, you can do the following steps:

Update the source code modifications:

&nbsp; _cd openasip-devel/openasip_ <br>
&nbsp; _git pull_

Build and install the updated OpenASIP:

&nbsp; _make && make install_

Further information of library and tools prerequisite versions
==============================================================

These are the current prerequisites for libraries and tools required to
build OpenASIP:  

| Host compiler 	| Supported versions 	|
|---------------	|--------------------	|
| GCC           	| 9.0+               	|
| Clang         	| 13 to 14           	|


| Library       | Supported versions 	| Library license      	|
|------------	|--------------------	|----------------------	|
| Xerces-C++ 	| 2.3.0+             	| Apache v2.0          	|
| wxWidgets  	| 2.8+               	| wxWidgets            	|
| Tcl        	| 8.0-8.4            	| BSD-style            	|
| Boost      	| 1.48.0-1.53.0      	| very permissive      	|
| sqlite3    	| 3.2.0+             	| public domain        	|
| LLVM       	| 15                 	| LLVM Release License 	|
| Editline   	| 2.9                	| BSD-style            	|
| Python     	| 3.6+               	| PSF license          	|


Please remember to install the '-dev' versions of the packages in order to get
the library headers which are need to compile client code against them.
You also need a version of "GNU Make" to build OpenASIP.

Editline
--------

Editline is used for command line editing/history browsing capabilities to
the command line interface of the simulator.

You can download the sources here: http://www.thrysoee.dk/editline/

Note: this library might require 'termcap' library in some distributions.
The package is named 'libedit-dev' in Debian-based distributions.

Boost
-----
The development files of the following Boost libraries are required by OpenASIP:<br>
. regexp,<br>
. spirit,<br>
. filesystem,<br>
. format,<br>
. thread,<br>
. graph,<br>
. tuple<br>

Some distributions, like Fedora Core, include all Boost libraries in a
single package, but some, like Debian, have separated them in several
packages.


