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
 * @file TCETargetMachine.h
 *
 * Declaration of TCETargetMachine class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2007 (vjaaskel-no.spam-cs.tut.fi)
 */

#ifndef TCE_TARGET_MACHINE_H
#define TCE_TARGET_MACHINE_H

#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetData.h"
#include "llvm/Target/TargetFrameInfo.h"
#include "llvm/PassManager.h"
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

        virtual const TargetRegisterInfo* getRegisterInfo() const {
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
