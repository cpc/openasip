/**
 * @file TCETargetMachine.h
 *
 * Declaration of TCETargetMachine class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2007 (vjaaskel@cs.tut.fi)
 */

#ifndef TCE_TARGET_MACHINE_H
#define TCE_TARGET_MACHINE_H

#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetData.h"
#include "llvm/Target/TargetFrameInfo.h"
#include "TCESubtarget.hh"
#include "TCETargetMachinePlugin.hh"

namespace TTAMachine {
    class Machine;
}

class PluginTools;

namespace llvm {

    class Module;

    /**
     * TCE Universal machine target description.
     */
    class TCETargetMachine : public LLVMTargetMachine {

    public:
        TCETargetMachine(
	    const Module& m,
	    const std::string& fs,
	    TCETargetMachinePlugin& plugin);

        virtual ~TCETargetMachine();

        virtual const TargetSubtarget* getSubtargetImpl() const {
            return &subtarget_; }

        virtual const TargetInstrInfo* getInstrInfo() const {
	   return plugin_.getInstrInfo();
	}

        virtual const MRegisterInfo *getRegisterInfo() const {
            return plugin_.getRegisterInfo();
        }

        virtual const TargetData* getTargetData() const {
            return &dataLayout_; }

        virtual const TargetFrameInfo* getFrameInfo() const {
            return &frameInfo_; }
        
        virtual bool addInstSelector(FunctionPassManager& pm, bool fast);
        virtual bool addAssemblyEmitter(
            FunctionPassManager& pm, bool fast, std::ostream& out);

        static unsigned getModuleMatchQuality(const Module &M);

        std::string operationName(unsigned opc) {
            return plugin_.operationName(opc);
        }

        std::string rfName(unsigned dwarfRegNum) {
            return plugin_.rfName(dwarfRegNum);
        }
        unsigned registerIndex(unsigned dwarfRegNum) {
            return plugin_.registerIndex(dwarfRegNum);
        }

        TTAMachine::Machine* createMachine();

        std::string dataASName() {
            return plugin_.dataASName();
        }

        unsigned raPortDRegNum() {
            return plugin_.raPortDRegNum();
        }

        void loadPlugin();

        unsigned spDRegNum() {
            return plugin_.spDRegNum();
        }

    protected:
        const TargetAsmInfo* createTargetAsmInfo() const;

    private:
        const TargetData dataLayout_;
        TCESubtarget subtarget_;
        TargetFrameInfo frameInfo_;
        TCETargetMachinePlugin& plugin_;
        PluginTools* pluginTool_;
    };
}

#endif
