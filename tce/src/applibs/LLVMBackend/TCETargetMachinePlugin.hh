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
   class MRegisterInfo;
   class FunctionPass;
   class TCETargetMachine;

   class TCETargetMachinePlugin {
    public:
       TCETargetMachinePlugin() {};
       virtual ~TCETargetMachinePlugin() {};

       virtual const TargetInstrInfo* getInstrInfo() const = 0;
       virtual const MRegisterInfo* getRegisterInfo() const = 0;
       virtual FunctionPass* createISelPass(TCETargetMachine* tm) = 0;
       virtual FunctionPass* createAsmPrinterPass(
           std::ostream& o, TCETargetMachine* tm) = 0;

       /// Returns name of the physical register file corresponding
       /// to a generated register ID.
       virtual std::string rfName(unsigned dwarfRegNum) = 0;

       /// Returns name of the physical register index corresponding
       /// to a generated register ID.
       virtual unsigned registerIndex(unsigned dwarfRegNum) = 0;

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
