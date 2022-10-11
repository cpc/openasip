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
 * @file IDFSerializer.cc
 *
 * Implementation of IDFSerializer class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#include <string>
#include <set>

#include "IDFSerializer.hh"
#include "MachineImplementation.hh"
#include "UnitImplementationLocation.hh"
#include "Environment.hh"
#include "Application.hh"
#include "ObjectState.hh"

using std::string;

const string ADF_IMPLEMENTATION = "adf-implementation";
const string IC_DECODER_PLUGIN = "ic-decoder-plugin";
const string IC_DECODER_PLUGIN_NAME = "name";
const string IC_DECODER_PLUGIN_FILE = "file";
const string IC_DECODER_PARAMETER = "parameter";
const string IC_DECODER_PARAMETER_NAME = "name";
const string IC_DECODER_PARAMETER_VALUE = "value";

const string DECOMPRESSOR_FILE = "decompressor-file";

const string HDB_FILE = "hdb-file";

const string FU_GENERATE = "fu-generate";
const string FU_GENERATE_NAME = "name";
const string FU_GENERATE_OPERATION = "operation";
const string FU_GENERATE_OPERATION_NAME = "name";
const string FU_GENERATE_OPERATION_ID = "operation-id";
const string FU_GENERATE_OPTION = "option";

const string FU = "fu";
const string FU_NAME = "name";
const string FU_ID = "fu-id";

const string RF = "rf";
const string RF_NAME = "name";
const string RF_ID = "rf-id";

const string IU = "iu";
const string IU_NAME = "name";

const string BUS = "bus";
const string BUS_NAME = "name";
const string BUS_ID = "bus-id";

const string SOCKET = "socket";
const string SOCKET_NAME = "name";
const string SOCKET_ID = "socket-id";

const string IDF_SCHEMA_FILE = "idf/IDF_Schema.xsd";

namespace IDF {

/**
 * The constructor.
 */
IDFSerializer::IDFSerializer() : XMLSerializer() {
    setSchemaFile(Environment::schemaDirPath(IDF_SCHEMA_FILE));
    setUseSchema(true);
}


/**
 * The destructor.
 */
IDFSerializer::~IDFSerializer() {
}


/**
 * Reads the source file that is set and creates an ObjectState tree that
 * can be loaded by MachineImplementation instance.
 *
 * @return The newly created ObjectState tree.
 * @exception SerializerException If an error occurs while reading the file.
 */
ObjectState*
IDFSerializer::readState() {
    ObjectState* fileState = XMLSerializer::readState();
    ObjectState* omState = convertToOMFormat(fileState);

    // set the source IDF
    omState->setAttribute(
        MachineImplementation::OSKEY_SOURCE_IDF, sourceFile());
    delete fileState;
    return omState;
}

/**
 * Writes the given ObjectState tree created by 
 * MachineImplementation::saveState to an IDF file.
 *
 * @param state ObjectState tree that represents the MachineImplementation
 *              instance.
 * @exception SerializerException If an error occurs while writing the file.
 */
void
IDFSerializer::writeState(const ObjectState* state) {
    ObjectState* fileState = convertToFileFormat(state);
    XMLSerializer::writeState(fileState);
    delete fileState;
}

/**
 * Reads the source IDF file that is set and creates a MachineImplementation
 * instance from it.
 *
 * @return The newly created MachineImplementation instance.
 * @exception SerializerException If an error occurs while reading the file.
 * @exception ObjectStateLoadingException If an error occurs while loading
 *                                        the state of MachineImplementation
 *                                        instance.
 */
MachineImplementation*
IDFSerializer::readMachineImplementation() {
    ObjectState* omState = readState();
    MachineImplementation* implementation = new MachineImplementation(
        omState);
    delete omState;
    return implementation;
}

/**
 * Writes the given machine implementation to the destination IDF file.
 *
 * @param implementation The implementation to be written.
 * @exception SerializerException If an error occurs while writing file.
 */
void
IDFSerializer::writeMachineImplementation(
    const MachineImplementation& implementation) {
    ObjectState* omState = implementation.saveState();
    writeState(omState);
    delete omState;
}

/**
 * Converts the given ObjectState tree that represents an IDF file to format
 * that can be loaded by MachineImplementation instance.
 *
 * @param fileState ObjectState tree that represents an IDF file.
 * @return The newly created ObjectState tree.
 */
ObjectState*
IDFSerializer::convertToOMFormat(const ObjectState* fileState) {
    
    ObjectState* omState = new ObjectState(
        MachineImplementation::OSNAME_MACHINE_IMPLEMENTATION);
  
    // add unit implementations
    ObjectState* fuGenerated =
        new ObjectState(MachineImplementation::OSNAME_FU_GENERATED);
    ObjectState* fuImpls = new ObjectState(
        MachineImplementation::OSNAME_FU_IMPLEMENTATIONS);
    ObjectState* rfImpls = new ObjectState(
        MachineImplementation::OSNAME_RF_IMPLEMENTATIONS);
    ObjectState* iuImpls = new ObjectState(
        MachineImplementation::OSNAME_IU_IMPLEMENTATIONS);
    ObjectState* busImpls = new ObjectState(
        MachineImplementation::OSNAME_BUS_IMPLEMENTATIONS);
    ObjectState* socketImpls = new ObjectState(
        MachineImplementation::OSNAME_SOCKET_IMPLEMENTATIONS);

    omState->addChild(fuImpls);
    omState->addChild(rfImpls);
    omState->addChild(iuImpls);
    omState->addChild(busImpls);
    omState->addChild(socketImpls);
    omState->addChild(fuGenerated);

    // the IC&decoder plugin data 
    if (fileState->hasChild(IC_DECODER_PLUGIN)) {
        ObjectState* child = fileState->childByName(IC_DECODER_PLUGIN);
        ObjectState* icdecState = new ObjectState(
            MachineImplementation::OSNAME_IC_DECODER_PLUGIN);

        std::string name = "";
        std::string fileName = "";
        std::string hdbFile = "";

        for (int i = 0; i < child->childCount(); i++) {
            ObjectState* attr = child->child(i);
            if (attr->name() == IC_DECODER_PLUGIN_NAME) {
                name = attr->stringValue();
            } else if (attr->name() == IC_DECODER_PLUGIN_FILE) {
                fileName = attr->stringValue();
            } else if (attr->name() == HDB_FILE) {
                hdbFile = attr->stringValue();
            } else if (attr->name() == IC_DECODER_PARAMETER) {

                // IC&decoder parameter.
                ObjectState* parameterState = new ObjectState(
                    MachineImplementation::OSNAME_IC_DECODER_PARAMETER);

                std::string parameterName = attr->stringAttribute(
                    IC_DECODER_PARAMETER_NAME);

                parameterState->setAttribute(
                    MachineImplementation::OSKEY_IC_DECODER_PARAMETER_NAME,
                    parameterName);

                std::string parameterValue = "";

                if (attr->hasChild(IC_DECODER_PARAMETER_VALUE)) {
                    parameterValue = attr->childByName(
                        IC_DECODER_PARAMETER_VALUE)->stringValue();
                }

                parameterState->setAttribute(
                    MachineImplementation::OSKEY_IC_DECODER_PARAMETER_VALUE,
                    parameterValue);

                icdecState->addChild(parameterState);
            }
        }


        icdecState->setAttribute(
            MachineImplementation::OSKEY_IC_DECODER_NAME, name);
        icdecState->setAttribute(
            MachineImplementation::OSKEY_IC_DECODER_FILE, fileName);
        icdecState->setAttribute(
            MachineImplementation::OSKEY_IC_DECODER_HDB, hdbFile);
        omState->addChild(icdecState);
    }

    // decompressor file
    if (fileState->hasChild(DECOMPRESSOR_FILE)) {
        ObjectState* child = fileState->childByName(DECOMPRESSOR_FILE);
        omState->setAttribute(
            MachineImplementation::OSKEY_DECOMPRESSOR_FILE, 
            child->stringValue());
    }

    // Generated FUs.
    for (int i = 0; i < fileState->childCount(); i++) {
        ObjectState* child = fileState->child(i);

        if (child->name() != FU_GENERATE) {
            continue;
        }

        ObjectState* impl = new ObjectState(*child);
        fuGenerated->addChild(impl);
    }

    const std::set<std::string> handledElements{FU, RF, IU, SOCKET, BUS};
    for (int i = 0; i < fileState->childCount(); i++) {
        ObjectState* child = fileState->child(i);

        if (!handledElements.count(child->name())) {
            continue;
        }

        ObjectState* impl = new ObjectState(
            UnitImplementationLocation::OSNAME_UNIT_IMPLEMENTATION);
        impl->setAttribute(
            UnitImplementationLocation::OSKEY_HDB_FILE, 
            child->childByName(HDB_FILE)->stringValue());

        if (child->name() == FU) {
            fuImpls->addChild(impl);
            impl->setAttribute(
                UnitImplementationLocation::OSKEY_UNIT_NAME,
                child->stringAttribute(FU_NAME));
            impl->setAttribute(
                UnitImplementationLocation::OSKEY_ID, 
                child->childByName(FU_ID)->stringValue());
        } else if (child->name() == RF) {
            rfImpls->addChild(impl);
            impl->setAttribute(
                UnitImplementationLocation::OSKEY_UNIT_NAME,
                child->stringAttribute(RF_NAME));
            impl->setAttribute(
                UnitImplementationLocation::OSKEY_ID, 
                child->childByName(RF_ID)->stringValue());
        } else if (child->name() == IU) {
            iuImpls->addChild(impl);
            impl->setAttribute(
                UnitImplementationLocation::OSKEY_UNIT_NAME,
                child->stringAttribute(IU_NAME));
            impl->setAttribute(
                UnitImplementationLocation::OSKEY_ID, 
                child->childByName(RF_ID)->stringValue());
        } else if (child->name() == BUS) {
            busImpls->addChild(impl);
            impl->setAttribute(
                UnitImplementationLocation::OSKEY_UNIT_NAME,
                child->stringAttribute(BUS_NAME));
            impl->setAttribute(
                UnitImplementationLocation::OSKEY_ID, 
                child->childByName(BUS_ID)->stringValue());
        } else if (child->name() == SOCKET) {
            socketImpls->addChild(impl);
            impl->setAttribute(
                UnitImplementationLocation::OSKEY_UNIT_NAME,
                child->stringAttribute(SOCKET_NAME));
            impl->setAttribute(
                UnitImplementationLocation::OSKEY_ID, 
                child->childByName(SOCKET_ID)->stringValue());
        } else {
            assert(false);
        }
    }

    return omState;
}           


/**
 * Converts the given ObjectState tree that represents state of a
 * MachineImplementation instance to IDF file format.
 *
 * @param omState ObjectState tree that represents state of a 
 *                MachineImplementation instance.
 * @return The newly created ObjectState tree.
 */
ObjectState*
IDFSerializer::convertToFileFormat(const ObjectState* omState) {

    ObjectState* fileState = new ObjectState(ADF_IMPLEMENTATION);

    // the IC&decoder plugin data 
    if (omState->hasChild(MachineImplementation::OSNAME_IC_DECODER_PLUGIN)) {
        ObjectState* child = omState->childByName(
            MachineImplementation::OSNAME_IC_DECODER_PLUGIN);
        ObjectState* icdecState = new ObjectState(IC_DECODER_PLUGIN);

        if (child->hasAttribute(
                MachineImplementation::OSKEY_IC_DECODER_NAME)) {
            ObjectState* newChild = new ObjectState(IC_DECODER_PLUGIN_NAME);
            newChild->setValue(
                child->stringAttribute(
                    MachineImplementation::OSKEY_IC_DECODER_NAME));
            icdecState->addChild(newChild);
        }

        if (child->hasAttribute(
                MachineImplementation::OSKEY_IC_DECODER_FILE)) {
            ObjectState* newChild = new ObjectState(
                IC_DECODER_PLUGIN_FILE);
            newChild->setValue(
                child->stringAttribute(
                    MachineImplementation::OSKEY_IC_DECODER_FILE));
            icdecState->addChild(newChild);
        }

        if (child->hasAttribute(
                MachineImplementation::OSKEY_IC_DECODER_HDB)) {
            ObjectState* newChild = new ObjectState(HDB_FILE);
            newChild->setValue(
                child->stringAttribute(
                    MachineImplementation::OSKEY_IC_DECODER_HDB));
            icdecState->addChild(newChild);
        }


        // Parameters
        for (int i = 0; i < child->childCount(); i++) {
            ObjectState* parameter = child->child(i);
            assert(parameter->name() == 
                   MachineImplementation::OSNAME_IC_DECODER_PARAMETER);

            ObjectState* fileParam = new ObjectState(IC_DECODER_PARAMETER);
            icdecState->addChild(fileParam);
            fileParam->setAttribute(
                IC_DECODER_PARAMETER_NAME,
                parameter->stringAttribute(
                    MachineImplementation::OSKEY_IC_DECODER_PARAMETER_NAME));

            if (parameter->hasAttribute(
                    MachineImplementation::OSKEY_IC_DECODER_PARAMETER_VALUE)) {

                ObjectState* value =
                    new ObjectState(IC_DECODER_PARAMETER_VALUE);

                value->setValue(parameter->stringAttribute(
                   MachineImplementation::OSKEY_IC_DECODER_PARAMETER_VALUE));

                fileParam->addChild(value);
            }
        }

        fileState->addChild(icdecState);
    }

    // decompressor file
    if (omState->hasAttribute(
            MachineImplementation::OSKEY_DECOMPRESSOR_FILE)) {
        ObjectState* newChild = new ObjectState(DECOMPRESSOR_FILE);
        fileState->addChild(newChild);
        newChild->setValue(
            omState->stringAttribute(
                MachineImplementation::OSKEY_DECOMPRESSOR_FILE));

    }

    // add generated FUs.
    ObjectState* fuGens =
        omState->childByName(MachineImplementation::OSNAME_FU_GENERATED);
    for (int i = fuGens->childCount() - 1; i >= 0; --i) {
        ObjectState* child = fuGens->child(i);
        fileState->addChild(new ObjectState(*child));
    }

    // add FU implementations
    ObjectState* fuImpls = omState->childByName(
        MachineImplementation::OSNAME_FU_IMPLEMENTATIONS);
    for (int i = 0; i < fuImpls->childCount(); i++) {
        ObjectState* child = fuImpls->child(i);
        ObjectState* fu = new ObjectState(FU);
        fileState->addChild(fu);
        fu->setAttribute(
            FU_NAME, 
            child->stringAttribute(
                UnitImplementationLocation::OSKEY_UNIT_NAME));
        ObjectState* hdbFile = new ObjectState(HDB_FILE);
        fu->addChild(hdbFile);
        hdbFile->setValue(
            child->stringAttribute(
                UnitImplementationLocation::OSKEY_HDB_FILE));
        ObjectState* id = new ObjectState(FU_ID);
        fu->addChild(id);
        id->setValue(
            child->stringAttribute(UnitImplementationLocation::OSKEY_ID));
    }

    // add RF implementations
    ObjectState* rfImpls = omState->childByName(
        MachineImplementation::OSNAME_RF_IMPLEMENTATIONS);
    for (int i = 0; i < rfImpls->childCount(); i++) {
        ObjectState* child = rfImpls->child(i);
        ObjectState* rf = new ObjectState(RF);
        fileState->addChild(rf);
        rf->setAttribute(
            RF_NAME, 
            child->stringAttribute(
                UnitImplementationLocation::OSKEY_UNIT_NAME));
        ObjectState* hdbFile = new ObjectState(HDB_FILE);
        rf->addChild(hdbFile);
        hdbFile->setValue(
            child->stringAttribute(
                UnitImplementationLocation::OSKEY_HDB_FILE));
        ObjectState* id = new ObjectState(RF_ID);
        rf->addChild(id);
        id->setValue(
            child->stringAttribute(UnitImplementationLocation::OSKEY_ID));
    }

    // add IU implementations
    ObjectState* iuImpls = omState->childByName(
        MachineImplementation::OSNAME_IU_IMPLEMENTATIONS);
    for (int i = 0; i < iuImpls->childCount(); i++) {
        ObjectState* child = iuImpls->child(i);
        ObjectState* iu = new ObjectState(IU);
        fileState->addChild(iu);
        iu->setAttribute(
            IU_NAME, 
            child->stringAttribute(
                UnitImplementationLocation::OSKEY_UNIT_NAME));
        ObjectState* hdbFile = new ObjectState(HDB_FILE);
        iu->addChild(hdbFile);
        hdbFile->setValue(
            child->stringAttribute(
                UnitImplementationLocation::OSKEY_HDB_FILE));
        ObjectState* id = new ObjectState(RF_ID);
        iu->addChild(id);
        id->setValue(
            child->stringAttribute(UnitImplementationLocation::OSKEY_ID));
    }

    // add bus implementations
    ObjectState* busImpls = omState->childByName(
        MachineImplementation::OSNAME_BUS_IMPLEMENTATIONS);
    for (int i = 0; i < busImpls->childCount(); i++) {
        ObjectState* child = busImpls->child(i);
        ObjectState* bus = new ObjectState(BUS);
        fileState->addChild(bus);
        bus->setAttribute(
            BUS_NAME, 
            child->stringAttribute(
                UnitImplementationLocation::OSKEY_UNIT_NAME));
        ObjectState* hdbFile = new ObjectState(HDB_FILE);
        bus->addChild(hdbFile);
        hdbFile->setValue(
            child->stringAttribute(
                UnitImplementationLocation::OSKEY_HDB_FILE));
        ObjectState* id = new ObjectState(BUS_ID);
        bus->addChild(id);
        id->setValue(
            child->stringAttribute(UnitImplementationLocation::OSKEY_ID));
    }

    // add socket implementations
    ObjectState* socketImpls = omState->childByName(
        MachineImplementation::OSNAME_SOCKET_IMPLEMENTATIONS);
    for (int i = 0; i < socketImpls->childCount(); i++) {
        ObjectState* child = socketImpls->child(i);
        ObjectState* socket = new ObjectState(SOCKET);
        fileState->addChild(socket);
        socket->setAttribute(
            IU_NAME, 
            child->stringAttribute(
                UnitImplementationLocation::OSKEY_UNIT_NAME));
        ObjectState* hdbFile = new ObjectState(HDB_FILE);
        socket->addChild(hdbFile);
        hdbFile->setValue(
            child->stringAttribute(
                UnitImplementationLocation::OSKEY_HDB_FILE));
        ObjectState* id = new ObjectState(SOCKET_ID);
        socket->addChild(id);
        id->setValue(
            child->stringAttribute(UnitImplementationLocation::OSKEY_ID));
    }


    return fileState;
}

}
