/*
    Copyright (c) 2002-2009 Tampere University.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
 */
/**
 * @file TCESubtarget.h
 *
 * Declaration of TCESubtarget class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2007 (vjaaskel-no.spam-cs.tut.fi)
 */

#ifndef TTA_TCE_SUBTARGET_H
#define TTA_TCE_SUBTARGET_H

#include <string>

#include "tce_config.h"

#ifdef LLVM_OLDER_THAN_6_0
#include "llvm/Target/TargetSubtargetInfo.h"
#else
#include "llvm/CodeGen/TargetSubtargetInfo.h"
#endif

#define GET_SUBTARGETINFO_HEADER
#include "TCEGenSubTargetInfo.inc"
#undef GET_SUBTARGETINFO_HEADER

#ifndef LLVM_OLDER_THAN_3_9
#include "llvm/CodeGen/SelectionDAGTargetInfo.h"
#endif

namespace llvm {

    class TCETargetMachinePlugin;
    class Module;

    /**
     * Currently there is only one generic universal machine target,
     * so this class doesn't contain any relevant information.
     */
    class TCESubtarget : public TCEGenSubtargetInfo {
    public:
        TCESubtarget(TCETargetMachinePlugin* plugin); //const std::string &TT, const std::string &FS);
        std::string pluginFileName();
        virtual bool isLittleEndian() const { return false; };

        virtual const TargetInstrInfo* getInstrInfo() const override;
        virtual const TargetFrameLowering* getFrameLowering() const override;
        virtual const TargetLowering* getTargetLowering() const override;
#ifdef LLVM_OLDER_THAN_3_9
        virtual const TargetSelectionDAGInfo* getSelectionDAGInfo() const override;
#else
       virtual const SelectionDAGTargetInfo* getSelectionDAGInfo() const override;
#endif

        virtual const TargetRegisterInfo* getRegisterInfo() const override;

    protected:
        void ParseSubtargetFeatures(llvm::StringRef, llvm::StringRef);
    private:
        std::string pluginFile_;
        TCETargetMachinePlugin* plugin_;
    };

    class  TCELESubtarget: public TCESubtarget {
    public:
        TCELESubtarget(TCETargetMachinePlugin* plugin) : 
            TCESubtarget(plugin) {}

        virtual bool isLittleEndian() const { return true; }
    };
}

#endif
