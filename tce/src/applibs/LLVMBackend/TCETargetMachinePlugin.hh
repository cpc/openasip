/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file TCETargetMachinePlugin.hh
 *
 * Declaration of TCETargetMachinePlugin class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2007 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TCE_TARGET_MACHINE_PLUGIN_HH
#define TCE_TARGET_MACHINE_PLUGIN_HH

#include <iostream>
#include <llvm/Target/TargetInstrInfo.h>

namespace TTAMachine {
    class Machine;
}

/**
 * TCE target machine plugin interface.
 */
namespace llvm {
   class TargetInstrInfo;
   class TargetRegisterInfo;
   class FunctionPass;
   class TCETargetMachine;

   class TCETargetMachinePlugin {
    public:
       TCETargetMachinePlugin() {};
       virtual ~TCETargetMachinePlugin() {};

       virtual const TargetInstrInfo* getInstrInfo() const = 0;
       virtual const TargetRegisterInfo* getRegisterInfo() const = 0;
       virtual FunctionPass* createISelPass(TCETargetMachine* tm) = 0;
       virtual FunctionPass* createAsmPrinterPass(
           std::ostream& o, TCETargetMachine* tm) = 0;

       /// Returns name of the physical register file corresponding
       /// to a generated register ID.
       virtual std::string rfName(unsigned dwarfRegNum) = 0;

       /// Returns name of the physical register index corresponding
       /// to a generated register ID.
       virtual unsigned registerIndex(unsigned dwarfRegNum) = 0;

       /// Returns operation name corresponding to llvm target opcode.
       virtual std::string operationName(unsigned opc) = 0;

       /// Returns pointer to xml string of the target machine .adf
       virtual const std::string* adfXML() = 0;
       /// Returns name of the data address space.
       virtual std::string dataASName() = 0;
       /// Returns ID number of the return address register.
       virtual unsigned raPortDRegNum() = 0;
       /// Returns ID number of the stack pointer register.
       virtual unsigned spDRegNum() = 0;

   protected:
       /// Target machine instruction info for the llvm framework.
       TargetInstrInfo* instrInfo_;
   };

}
#endif
