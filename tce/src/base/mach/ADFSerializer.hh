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
#include "Machine.hh"

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

    void writeState(const ObjectState* machineState)
        throw (SerializerException);
    ObjectState* readState()
        throw (SerializerException);
    void writeMachine(const TTAMachine::Machine& machine)
        throw (SerializerException);
    TTAMachine::Machine* readMachine()
        throw (SerializerException, ObjectStateLoadingException);

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
    static ObjectState* convertToMachineFormat(const ObjectState* mdfState)
        throw (SerializerException);
    static ObjectState* busToMachine(const ObjectState* busState);
    static ObjectState* socketToMachine(const ObjectState* socketState);
    static ObjectState* bridgeToMachine(const ObjectState* bridgeState);
    static ObjectState* functionUnitToMachine(const ObjectState* fuState);
    static ObjectState* registerFileToMachine(const ObjectState* rfState);
    static ObjectState* addressSpaceToMachine(const ObjectState* asState);
    static ObjectState* controlUnitToMachine(const ObjectState* cuState);
    static ObjectState* immediateUnitToMachine(
        const ObjectState* iuState,
        ObjectState* machineState)
        throw (SerializerException);
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
        const ObjectState* mdfITState,
        ObjectState* momMachineState,
        const std::string& iuName)
        throw (SerializerException);
    static ObjectState* momTemplateSlot(
        const ObjectState* momITState,
        const std::string& busName);
    static bool hasEmptyInstructionTemplate(
        const ObjectState* momMachineState);
};


#endif
