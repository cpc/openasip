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
 * @author Veli-Pekka Jääskeläinen 2007 (vjaaskel-no.spam-cs.tut.fi)
 */

#ifndef TTA_TCE_SUBTARGET_H
#define TTA_TCE_SUBTARGET_H

#include <string>

#include "tce_config.h"

#include "llvm/CodeGen/TargetSubtargetInfo.h"

#define GET_SUBTARGETINFO_HEADER
#include "TCEGenSubTargetInfo.inc"
#undef GET_SUBTARGETINFO_HEADER

#include "llvm/CodeGen/SelectionDAGTargetInfo.h"

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

        virtual bool is64bit() const { return false; }

        const InstrItineraryData*
        getInstrItineraryData() const override {
            return &InstrItins;
        }

        virtual const TargetInstrInfo* getInstrInfo() const override;
        virtual const TargetFrameLowering* getFrameLowering() const override;
        virtual const TargetLowering* getTargetLowering() const override;
        virtual const SelectionDAGTargetInfo* getSelectionDAGInfo()
            const override;

        virtual const TargetRegisterInfo* getRegisterInfo() const override;
        virtual bool enableMachinePipeliner() const override;

    protected:
        /// ParseSubtargetFeatures - Parses features string setting specified
        /// subtarget options.  Definition of function is auto generated by
        /// tblgen.
        void ParseSubtargetFeatures(
            StringRef CPU, StringRef TuneCPU, StringRef FS);
    private:
        std::string pluginFile_;
        TCETargetMachinePlugin* plugin_;
        InstrItineraryData InstrItins;
    };

    class  TCELESubtarget: public TCESubtarget {
    public:
        TCELESubtarget(TCETargetMachinePlugin* plugin) : 
            TCESubtarget(plugin) {}

        virtual bool isLittleEndian() const override { return true; }
    };

    class TCELE64Subtarget: public TCELESubtarget {
    public:
        TCELE64Subtarget(TCETargetMachinePlugin* plugin) :
            TCELESubtarget(plugin) {}

        virtual bool is64bit() const override { return true; }
    };
}

#endif