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
 * @file ADFSerializer.hh
 *
 * Declaration of ADFSerializer class.
 *
 * @author Lasse Laasonen 2004 (lasse.laasonen-no.spam-tut.fi)
 * @note reviewed 15 Jun 2004 by pj, vpj, ml, ll
 * @note rating: red
 */

#ifndef TTA_MDF_SERIALIZER_HH
#define TTA_MDF_SERIALIZER_HH

#include "XMLSerializer.hh"

namespace TTAMachine {
    class Machine;
}

/**
 * This class is used to generate a machine object model from MDF file and to
 * generate MDF file from a machine object model.
 *
 * By default ADFSerializer validates the files it reads with ADF Schema.
 */
class ADFSerializer : public XMLSerializer {
public:
    ADFSerializer();
    virtual ~ADFSerializer();

    void writeState(const ObjectState* machineState);
    ObjectState* readState();
    void writeMachine(const TTAMachine::Machine& machine);
    TTAMachine::Machine* readMachine();

protected:
    /// Copying forbidden.
    ADFSerializer(const ADFSerializer&);
    /// Assingment forbidden.
    ADFSerializer& operator=(const ADFSerializer&);

private:
    // machine format to MDF format conversion functions
    static ObjectState* convertToMDFFormat(const ObjectState* machineState);
    static ObjectState* busToMDF(const ObjectState* busState);
    static ObjectState* socketToMDF(const ObjectState* socketState);
    static ObjectState* bridgeToMDF(const ObjectState* bridgeState);
    static ObjectState* functionUnitToMDF(const ObjectState* fuState);
    static ObjectState* registerFileToMDF(const ObjectState* rfState);
    static ObjectState* immediateUnitToMDF(
        const ObjectState* iuState,
        const ObjectState* machineState);
    static ObjectState* addressSpaceToMDF(const ObjectState* asState);
    static ObjectState* controlUnitToMDF(const ObjectState* cuState);
    static ObjectState* immediateSlotToMDF(const ObjectState* isState);

    // MDF format to machine format conversion functions
    static ObjectState* convertToMachineFormat(const ObjectState* mdfState);
    static ObjectState* busToMachine(const ObjectState* busState);
    static ObjectState* socketToMachine(const ObjectState* socketState);
    static ObjectState* bridgeToMachine(const ObjectState* bridgeState);
    static ObjectState* functionUnitToMachine(
    	const ObjectState* fuState,
        const int orderNumber);
    static ObjectState* registerFileToMachine(const ObjectState* rfState);
    static ObjectState* addressSpaceToMachine(const ObjectState* asState);
    static ObjectState* controlUnitToMachine(const ObjectState* cuState);
    static ObjectState* immediateUnitToMachine(
        const ObjectState* iuState, ObjectState* machineState);
    static ObjectState* immediateSlotToMachine(const ObjectState* isState);

    // small helper functions
    static ObjectState* machineSRPort(const ObjectState* mdfSRPortState);
    static ObjectState* mdfSRPort(const ObjectState* machineSRPortState);
    static void setIUExtensionMode(
        const ObjectState* mdfIUState,
        ObjectState* momIUState);
    static ObjectState* machineRFPort(const ObjectState* mdfPortState);
    static ObjectState* mdfPort(const ObjectState* machinePortState);
    static ObjectState* machineFUPort(const ObjectState* mdfFUPortState);
    static ObjectState* mdfFUPort(const ObjectState* machineFUPortState);
    static ObjectState* machineOperation(
        const ObjectState* mdfOperationState);
    static ObjectState* mdfOperation(
        const ObjectState* machineOperationState);
    static ObjectState* machinePipeline(const ObjectState* mdfPipelineState);
    static ObjectState* mdfPipeline(const ObjectState* machinePipelineState);
    static void instructionTemplateToMDF(
        const ObjectState* momITState,
        ObjectState* mdfIUState);
    static void instructionTemplateToMachine(
        const ObjectState* mdfITState, ObjectState* momMachineState,
        const std::string& iuName = std::string(""));
    static ObjectState* momTemplateSlot(
        const ObjectState* momITState,
        const std::string& busName);
    static bool hasEmptyInstructionTemplate(
        const ObjectState* momMachineState);
        
};


#endif
