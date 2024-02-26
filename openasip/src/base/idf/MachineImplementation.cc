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
 * @file MachineImplementation.cc
 *
 * Implementation of MachineImplementation class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#include <string>
#include <vector>

#include "MachineImplementation.hh"
#include "UnitImplementationLocation.hh"
#include "SequenceTools.hh"
#include "FileSystem.hh"
#include "IDFSerializer.hh"
#include "ObjectState.hh"

using std::string;
using std::vector;

namespace IDF {

const std::string MachineImplementation::OSNAME_MACHINE_IMPLEMENTATION = 
    "mach_impl";
const std::string MachineImplementation::OSKEY_SOURCE_IDF = "source_idf";
const std::string MachineImplementation::OSNAME_IC_DECODER_PLUGIN = 
    "ic_dec_plugin";
const std::string MachineImplementation::OSKEY_IC_DECODER_NAME = 
    "ic_dec_name";
const std::string MachineImplementation::OSKEY_IC_DECODER_FILE = 
    "ic_dec_file";
const std::string MachineImplementation::OSNAME_IC_DECODER_PARAMETER = 
    "ic_dec_parameter";
const std::string MachineImplementation::OSKEY_IC_DECODER_PARAMETER_NAME = 
    "ic_dec_parameter_name";
const std::string MachineImplementation::OSKEY_IC_DECODER_PARAMETER_VALUE = 
    "ic_dec_parameter_value";
const std::string MachineImplementation::OSKEY_IC_DECODER_HDB = 
    "ic_dec_hdb";

const std::string MachineImplementation::OSKEY_DECOMPRESSOR_FILE = 
    "decompressor_file";
const std::string MachineImplementation::OSNAME_FU_GENERATED = "fu_generated";
const std::string MachineImplementation::OSNAME_FU_IMPLEMENTATIONS =
    "fu_impls";
const std::string MachineImplementation::OSNAME_RF_IMPLEMENTATIONS = 
    "rf_impls";
const std::string MachineImplementation::OSNAME_IU_IMPLEMENTATIONS = 
    "iu_impls";
const std::string MachineImplementation::OSNAME_BUS_IMPLEMENTATIONS = 
    "bus_impls";
const std::string MachineImplementation::OSNAME_SOCKET_IMPLEMENTATIONS = 
    "socket_impls";


/**
 * The constructor.
 */
MachineImplementation::MachineImplementation():
    icDecoderPluginName_(""), icDecoderPluginFile_(""), icDecoderHDB_(""),
    decompressorFile_(""), sourceIDF_("") {
}

/**
 * The constructor.
 *
 * Loads the state of the object from the given ObjectState tree.
 *
 * @param state The ObjectState tree.
 * @exception ObjectStateLoadingException If the given ObjectState tree is
 *                                        invalid.
 */
MachineImplementation::MachineImplementation(const ObjectState* state)
    : icDecoderPluginName_(""),
      icDecoderPluginFile_(""),
      icDecoderHDB_(""),
      decompressorFile_(""),
      sourceIDF_("") {
    loadState(state);
}

/**
 * The destructor.
 */
MachineImplementation::~MachineImplementation() {
    clearState();
}


/**
 * Returns the path to the source IDF file.
 *
 * @return The source IDF.
 */
std::string
MachineImplementation::sourceIDF() const {
    return sourceIDF_;
}

    
/**
 * Returns the name of the IC/decoder plugin.
 *
 * @return The name of the IC/decoder plugin.
 */
std::string 
MachineImplementation::icDecoderPluginName() const {
    return icDecoderPluginName_;
}

/**
 * Returns true in case IC/decoder name is set.
 *
 * @return True in case IC/decoder name is set.
 */
bool 
MachineImplementation::hasICDecoderPluginName() const {
    return icDecoderPluginName_ != "";
}

/**
 * Returns the absolute path to the IC/decoder plugin file given in IDF.
 *
 * @return The absolute path to the IC/decoder plugin file.
 * @exception FileNotFound If the file is not found in search paths.
 */
std::string
MachineImplementation::icDecoderPluginFile() const {
    vector<string> paths = Environment::icDecoderPluginPaths();
    paths.insert(paths.begin(), FileSystem::directoryOfPath(sourceIDF_));
    TCEString expandedPath(icDecoderPluginFile_);
    expandedPath.replaceString("tce:", "");
    expandedPath = FileSystem::expandTilde(expandedPath);
    return FileSystem::findFileInSearchPaths(paths, expandedPath);
}

/**
 * Returns true in case IC/decoder file is set.
 *
 * @return True in case IC/decoder file is set.
 */
bool 
MachineImplementation::hasICDecoderPluginFile() const {
    return icDecoderPluginFile_ != "";
}

/**
 * Returns the absolute path to the IC/decoder HDB file.
 *
 * @return the absolute path to the IC/decoder HDB file.
 * @exception FileNotFound If the file is not found in search paths.
 */
std::string
MachineImplementation::icDecoderHDB() const {
    vector<string> paths = Environment::hdbPaths();
    paths.insert(paths.begin(), FileSystem::directoryOfPath(sourceIDF_));
    TCEString expandedPath(icDecoderHDB_);
    expandedPath.replaceString("tce:", "");
    return FileSystem::findFileInSearchPaths(paths, expandedPath);
}

/**
 * Returns true in case IC/decoder HDB file is set.
 *
 * @return True in case IC/decoder HDB file is set.
 */
bool 
MachineImplementation::hasICDecoderHDB() const {
    return icDecoderHDB_ != "";
}


/**
 * Returns the absolute path to the the decompressor definition file.
 *
 * @return The absolute path to the decompressor definition file.
 * @exception FileNotFound If the file is not found in search paths.
 */
std::string
MachineImplementation::decompressorFile() const {
    vector<string> paths = Environment::decompressorPaths();
    paths.insert(paths.begin(), FileSystem::directoryOfPath(sourceIDF_));
    TCEString expandedPath(decompressorFile_);
    expandedPath.replaceString("tce:", "");
    return FileSystem::findFileInSearchPaths(paths, expandedPath);
}

/**
 * Tells whether the decompressor definition file is given in IDF.
 *
 * @return True if the file is given, otherwise false.
 */
bool
MachineImplementation::hasDecompressorFile() const {
    return decompressorFile_ != "";
}


/**
 * Tells whether there is an implementation for the given FU defined.
 *
 * @param unitName Name of the FU.
 * @return True if there is an implementation, otherwise false.
 */
bool
MachineImplementation::hasFUImplementation(
    const std::string& unitName) const {

    return findImplementation(fuImplementations_, unitName) != NULL;
}


/**
 * Tells whether there is an implementation for the given RF defined.
 *
 * @param unitName Name of the RF.
 * @return True if there is an implementation, otherwise false.
 */
bool
MachineImplementation::hasRFImplementation(
    const std::string& unitName) const {

    return findImplementation(rfImplementations_, unitName) != NULL;
}


/**
 * Tells whether there is an implementation for the given IU defined.
 *
 * @param unitName Name of the IU.
 * @return True if there is an implementation, otherwise false.
 */
bool
MachineImplementation::hasIUImplementation(
    const std::string& unitName) const {

    return findImplementation(iuImplementations_, unitName) != NULL;
}

/**
 * Tells whether there is an implementation for the given bus defined.
 *
 * @param busName Name of the bus.
 * @return True if there is an implementation, otherwise false.
 */
bool
MachineImplementation::hasBusImplementation(
    const std::string& busName) const {

    return findImplementation(busImplementations_, busName) != NULL;
}


/**
 * Tells whether there is an implementation for the given socket defined.
 *
 * @param socketName Name of the IU.
 * @return True if there is an implementation, otherwise false.
 */
bool
MachineImplementation::hasSocketImplementation(
    const std::string& socketName) const {

    return findImplementation(socketImplementations_, socketName) != NULL;
}


/**
 * Returns the number of FU implementations.
 *
 * @return The number of FU implementations.
 */
int
MachineImplementation::fuImplementationCount() const {
    return fuImplementations_.size();
}


/**
 * Returns the number of RF implementations.
 *
 * @return The number of RF implementations.
 */
int
MachineImplementation::rfImplementationCount() const {
    return rfImplementations_.size();
}


/**
 * Returns the number of IU implementations.
 *
 * @return The number of IU implementations.
 */
int
MachineImplementation::iuImplementationCount() const {
    return iuImplementations_.size();
}

/**
 * Returns the number of bus implementations.
 *
 * @return The number of bus implementations.
 */
int
MachineImplementation::busImplementationCount() const {
    return busImplementations_.size();
}

/**
 * Returns the number of socket implementations.
 *
 * @return The number of socket implementations.
 */
int
MachineImplementation::socketImplementationCount() const {
    return socketImplementations_.size();
}


/**
 * Returns the implementation data of the given FU.
 *
 * @param fu Name of the FU.
 * @return The implementation data.
 * @exception InstanceNotFound If there is no implementation defined for 
 *                             the given FU.
 */
UnitImplementationLocation&
MachineImplementation::fuImplementation(const std::string& fu) const {
    UnitImplementationLocation* impl = findImplementation(
        fuImplementations_, fu);
    if (impl == NULL) {
        const string procName = "MachineImplementation::fuImplementation";
        throw InstanceNotFound(
            __FILE__, __LINE__, procName, 
            "No implementation data found for function unit " + fu + ".");
    } else {
        return *impl;
    }
}

/**
 * Returns the implementation data of the given RF.
 *
 * @param rf Name of the RF.
 * @return The implementation data.
 * @exception InstanceNotFound If there is no implementation defined for 
 *                             the given RF.
 */
UnitImplementationLocation&
MachineImplementation::rfImplementation(const std::string& rf) const {
    UnitImplementationLocation* impl = findImplementation(
        rfImplementations_, rf);
    if (impl == NULL) {
        const string procName = "MachineImplementation::rfImplementation";
        throw InstanceNotFound(
            __FILE__, __LINE__, procName,
            "No implementation data found for register file " + rf + ".");
    } else {
        return *impl;
    }
}

/**
 * Returns the implementation data of the given IU.
 *
 * @param iu Name of the IU.
 * @return The implementation data.
 * @exception InstanceNotFound If there is no implementation defined for 
 *                             the given IU.
 */
UnitImplementationLocation&
MachineImplementation::iuImplementation(const std::string& iu) const {
    UnitImplementationLocation* impl = findImplementation(
        iuImplementations_, iu);
    if (impl == NULL) {
        throw InstanceNotFound(
            __FILE__, __LINE__, __func__,
            "No implementation data found for immediate unit " + iu + ".");
    } else {
        return *impl;
    }
}

/**
 * Returns the implementation data of the given bus.
 *
 * @param bus Name of the bus.
 * @return The implementation data.
 * @exception InstanceNotFound If there is no implementation defined for 
 *                             the given bus.
 */
UnitImplementationLocation&
MachineImplementation::busImplementation(const std::string& bus) const {
    UnitImplementationLocation* impl = findImplementation(
        busImplementations_, bus);
    if (impl == NULL) {
        throw InstanceNotFound(
            __FILE__, __LINE__, __func__,
            "No implementation data found for bus " + bus + ".");
    } else {
        return *impl;
    }
}

/**
 * Returns the implementation data of the given socket.
 *
 * @param socket Name of the socket.
 * @return The implementation data.
 * @exception InstanceNotFound If there is no implementation defined for 
 *                             the given socket.
 */
UnitImplementationLocation&
MachineImplementation::socketImplementation(const std::string& socket) const {
    UnitImplementationLocation* impl = findImplementation(
        socketImplementations_, socket);
    if (impl == NULL) {
        throw InstanceNotFound(
            __FILE__, __LINE__, __func__,
            "No implementation data found for socket " + socket + ".");
    } else {
        return *impl;
    }
}

/**
 * Returns the FU implementation at the given position.
 *
 * @param index The position index.
 * @return The implementation data.
 * @exception OutOfRange If the index is smaller than 0 or not smaller than
 *                       the number of FU implementations.
 */
UnitImplementationLocation&
MachineImplementation::fuImplementation(int index) const {
    ensureIndexValidity(index, fuImplementations_);
    return *fuImplementations_[index];
}

/**
 * Returns the RF implementation at the given position.
 *
 * @param index The position index.
 * @return The implementation data.
 * @exception OutOfRange If the index is smaller than 0 or not smaller than
 *                       the number of RF implementations.
 */
UnitImplementationLocation&
MachineImplementation::rfImplementation(int index) const {
    ensureIndexValidity(index, rfImplementations_);
    return *rfImplementations_[index];
}

/**
 * Returns the IU implementation at the given position.
 *
 * @param index The position index.
 * @return The implementation data.
 * @exception OutOfRange If the index is smaller than 0 or not smaller than
 *                       the number of IU implementations.
 */
UnitImplementationLocation&
MachineImplementation::iuImplementation(int index) const {
    ensureIndexValidity(index, iuImplementations_);
    return *iuImplementations_[index];
}

/**
 * Returns the bus implementation at the given position.
 *
 * @param index The position index.
 * @return The implementation data.
 * @exception OutOfRange If the index is smaller than 0 or not smaller than
 *                       the number of bus implementations.
 */
UnitImplementationLocation&
MachineImplementation::busImplementation(int index) const {
    ensureIndexValidity(index, busImplementations_);
    return *busImplementations_[index];
}

/**
 * Returns the socket implementation at the given position.
 *
 * @param index The position index.
 * @return The implementation data.
 * @exception OutOfRange If the index is smaller than 0 or not smaller than
 *                       the number of socket implementations.
 */
UnitImplementationLocation&
MachineImplementation::socketImplementation(int index) const {
    ensureIndexValidity(index, socketImplementations_);
    return *socketImplementations_[index];
}

/**
 * Adds the given FU implementation.
 *
 * @param implementation The implementation to add.
 * @exception ObjectAlreadyExists If there is an implementation for the same
 *                                FU already.
 * @exception InvalidData If the given implementation is registered to
 *                        another MachineImplementation instance.
 */
void
MachineImplementation::addFUImplementation(
    UnitImplementationLocation* implementation) {
    if (hasFUImplementation(implementation->unitName())) {
        const string procName = "MachineImplementation::addFUImplementation";
        throw ObjectAlreadyExists(__FILE__, __LINE__, procName);
    } else {
        fuImplementations_.push_back(implementation);
        implementation->setParent(*this);
    }
}

/**
 * Adds the given RF implementation.
 *
 * @param implementation The implementation to add.
 * @exception ObjectAlreadyExists If there is an implementation for the same
 *                                RF already.
 * @exception InvalidData If the given implementation is registered to
 *                        another MachineImplementation instance.
 */
void
MachineImplementation::addRFImplementation(
    UnitImplementationLocation* implementation) {
    if (hasRFImplementation(implementation->unitName())) {
        const string procName = "MachineImplementation::addRFImplementation";
        throw ObjectAlreadyExists(__FILE__, __LINE__, procName);
    } else {
        rfImplementations_.push_back(implementation);
        implementation->setParent(*this);
    }
}

/**
 * Adds the given IU implementation.
 *
 * @param implementation The implementation to add.
 * @exception ObjectAlreadyExists If there is an implementation for the same
 *                                IU already.
 * @exception InvalidData If the given implementation is registered to
 *                        another MachineImplementation instance.
 */
void
MachineImplementation::addIUImplementation(
    UnitImplementationLocation* implementation) {
    if (hasIUImplementation(implementation->unitName())) {
        const string procName = "MachineImplementation::addRFImplementation";
        throw ObjectAlreadyExists(__FILE__, __LINE__, procName);
    } else {
        iuImplementations_.push_back(implementation);
        implementation->setParent(*this);
    }
}

/**
 * Adds the given bus implementation.
 *
 * @param implementation The implementation to add.
 * @exception ObjectAlreadyExists If there is an implementation for the same
 *                                bus already.
 * @exception InvalidData If the given implementation is registered to
 *                        another MachineImplementation instance.
 */
void
MachineImplementation::addBusImplementation(
    UnitImplementationLocation* implementation) {
    if (hasBusImplementation(implementation->unitName())) {
        throw ObjectAlreadyExists(__FILE__, __LINE__, __func__);
    } else {
        busImplementations_.push_back(implementation);
        implementation->setParent(*this);
    }
}

/**
 * Adds the given socket implementation.
 *
 * @param implementation The implementation to add.
 * @exception ObjectAlreadyExists If there is an implementation for the same
 *                                socket already.
 * @exception InvalidData If the given implementation is registered to
 *                        another MachineImplementation instance.
 */
void
MachineImplementation::addSocketImplementation(
    UnitImplementationLocation* implementation) {
    if (hasSocketImplementation(implementation->unitName())) {
        throw ObjectAlreadyExists(__FILE__, __LINE__, __func__);
    } else {
        socketImplementations_.push_back(implementation);
        implementation->setParent(*this);
    }
}

/** 
 * Removes the FU implementation with given name.
 *
 * @param unitName Name of the implementation to remove.
 * @exception InstanceNotFound If there is no implementation defined for 
 *                             the given FU.
 */
void
MachineImplementation::removeFUImplementation(const std::string& unitName) {
    bool removed = false;
    for (ImplementationTable::iterator iter = fuImplementations_.begin(); 
         iter != fuImplementations_.end(); iter++) {

        UnitImplementationLocation* implementation = *iter;
        if (implementation->unitName() == unitName) {
            fuImplementations_.erase(iter);
            removed = true;
            break;
        }
    }
    if (!removed) {
        throw InstanceNotFound(__FILE__, __LINE__, __func__);
    }
}

/** 
 * Removes the RF implementation with given name.
 *
 * @param unitName Name of the implementation to remove.
 * @exception InstanceNotFound If there is no implementation defined for 
 *                             the given RF.
 */
void
MachineImplementation::removeRFImplementation(const std::string& unitName) {
    bool removed = false;
    for (ImplementationTable::iterator iter = rfImplementations_.begin(); 
         iter != rfImplementations_.end(); iter++) {

        UnitImplementationLocation* implementation = *iter;
        if (implementation->unitName() == unitName) {
            rfImplementations_.erase(iter);
            removed = true;
            break;
        }
    }
    if (!removed) {
        throw InstanceNotFound(__FILE__, __LINE__, __func__);
    }
}

/** 
 * Removes the IU implementation with given name.
 *
 * @param unitName Name of the implementation to remove.
 * @exception InstanceNotFound If there is no implementation defined for 
 *                             the given IU.
 */
void
MachineImplementation::removeIUImplementation(const std::string& unitName) {
    bool removed = false;
    for (ImplementationTable::iterator iter = iuImplementations_.begin(); 
         iter != iuImplementations_.end(); iter++) {

        UnitImplementationLocation* implementation = *iter;
        if (implementation->unitName() == unitName) {
            iuImplementations_.erase(iter);
            removed = true;
            break;
        }
    }
    if (!removed) {
        throw InstanceNotFound(__FILE__, __LINE__, __func__);
    }
}

/** 
 * Removes the bus implementation with given name.
 *
 * @param unitName Name of the implementation to remove.
 * @exception InstanceNotFound If there is no implementation defined for 
 *                             the given bus.
 */
void
MachineImplementation::removeBusImplementation(const std::string& unitName) {
    bool removed = false;
    for (
        ImplementationTable::iterator iter = busImplementations_.begin(); 
        iter != busImplementations_.end(); iter++) {

        UnitImplementationLocation* implementation = *iter;
        if (implementation->unitName() == unitName) {
            busImplementations_.erase(iter);
            removed = true;
            break;
        }
    }
    if (!removed) {
        throw InstanceNotFound(__FILE__, __LINE__, __func__);
    }
}

/** 
 * Removes the socket implementation with given name.
 *
 * @param unitName Name of the implementation to remove.
 * @exception InstanceNotFound If there is no implementation defined for 
 *                             the given socket.
 */
void
MachineImplementation::removeSocketImplementation(const std::string& unitName) {
    bool removed = false;
    for (
        ImplementationTable::iterator iter = 
            socketImplementations_.begin(); 
        iter != socketImplementations_.end(); iter++) {

        UnitImplementationLocation* implementation = *iter;
        if (implementation->unitName() == unitName) {
            socketImplementations_.erase(iter);
            removed = true;
            break;
        }
    }
    if (!removed) {
        throw InstanceNotFound(__FILE__, __LINE__, __func__);
    }
}

/**
 * Loads the state of the object from the given ObjectState tree.
 *
 * @param state The given ObjectState tree.
 * @exception ObjectStateLoadingException If the given ObjectState tree is
 *                                        invalid.
 */
void
MachineImplementation::loadState(const ObjectState* state) {
    const string procName = "MachineImplementation::loadState";

    if (state->name() != OSNAME_MACHINE_IMPLEMENTATION) {
        throw ObjectStateLoadingException(__FILE__, __LINE__, procName);
    }

    clearState();


    if (state->hasChild(OSNAME_IC_DECODER_PLUGIN)) {
        ObjectState* icdecState = state->childByName(OSNAME_IC_DECODER_PLUGIN);
        icDecoderPluginName_ = 
            icdecState->stringAttribute(OSKEY_IC_DECODER_NAME);
        icDecoderPluginFile_ =
            icdecState->stringAttribute(OSKEY_IC_DECODER_FILE);
        if (icdecState->hasAttribute(OSKEY_IC_DECODER_HDB)) {
            icDecoderHDB_ = icdecState->stringAttribute(OSKEY_IC_DECODER_HDB);
        }

        // Load ic/decoder plugin parameters.
        for (int i = 0; i < icdecState->childCount(); i++) {
            ObjectState* parameterState = icdecState->child(i);
            if (parameterState->name() != OSNAME_IC_DECODER_PARAMETER) {
                throw ObjectStateLoadingException(
                    __FILE__, __LINE__, procName);
            }
            std::string name = parameterState->stringAttribute(
                OSKEY_IC_DECODER_PARAMETER_NAME);

            std::string value = parameterState->stringAttribute(
                OSKEY_IC_DECODER_PARAMETER_VALUE);

            Parameter parameter = { name, value };
            icDecoderParameters_.push_back(parameter);
        }
    }

    if (state->hasAttribute(OSKEY_DECOMPRESSOR_FILE)) {
        decompressorFile_ = state->stringAttribute(OSKEY_DECOMPRESSOR_FILE);
    }

    ObjectState* fuGenerate = state->childByName(OSNAME_FU_GENERATED);

    for (int i = 0; i < fuGenerate->childCount(); i++) {
        ObjectState* child = fuGenerate->child(i);
        FUGenerated newfug;
        newfug.loadState(child);
        fuGenerated_.emplace_back(newfug);
    }

    try {
        sourceIDF_ = state->stringAttribute(OSKEY_SOURCE_IDF);
        ObjectState* fuImplementations = state->childByName(
            OSNAME_FU_IMPLEMENTATIONS);
        ObjectState* rfImplementations = state->childByName(
            OSNAME_RF_IMPLEMENTATIONS);
        ObjectState* iuImplementations = state->childByName(
            OSNAME_IU_IMPLEMENTATIONS);
        ObjectState* busImplementations = state->childByName(
            OSNAME_BUS_IMPLEMENTATIONS);
        ObjectState* socketImplementations = state->childByName(
            OSNAME_SOCKET_IMPLEMENTATIONS);
        
        for (int i = 0; i < fuImplementations->childCount(); i++) {
            ObjectState* child = fuImplementations->child(i);
            addFUImplementation(new UnitImplementationLocation(child));
        }

        for (int i = 0; i < rfImplementations->childCount(); i++) {
            ObjectState* child = rfImplementations->child(i);
            addRFImplementation(new UnitImplementationLocation(child));
        }

        for (int i = 0; i < iuImplementations->childCount(); i++) {
            ObjectState* child = iuImplementations->child(i);
            addIUImplementation(new UnitImplementationLocation(child));
        }
        for (int i = 0; i < busImplementations->childCount(); i++) {
            ObjectState* child = busImplementations->child(i);
            addBusImplementation(new UnitImplementationLocation(child));
        }

        for (int i = 0; i < socketImplementations->childCount(); i++) {
            ObjectState* child = socketImplementations->child(i);
            addSocketImplementation(new UnitImplementationLocation(child));
        }

    } catch (const Exception& exception) {
        throw ObjectStateLoadingException(
            __FILE__, __LINE__, procName, exception.errorMessage());
    }
}

/**
 * Saves the state of the object to an ObjectState tree.
 *
 * @return The newly created ObjectState tree.
 */
ObjectState*
MachineImplementation::saveState() const {

    ObjectState* state = new ObjectState(OSNAME_MACHINE_IMPLEMENTATION);
    state->setAttribute(OSKEY_SOURCE_IDF, sourceIDF_);

    // add ic&decoder data
    if (hasICDecoderPluginName()) {
        ObjectState* icdecState = new ObjectState(OSNAME_IC_DECODER_PLUGIN);
        if (hasICDecoderPluginName()) {
            icdecState->setAttribute(
                OSKEY_IC_DECODER_NAME, icDecoderPluginName());
        }
        if (hasICDecoderPluginFile()) {
            icdecState->setAttribute(
                OSKEY_IC_DECODER_FILE, icDecoderPluginFile_);
        }
        if (hasICDecoderHDB()) {
            icdecState->setAttribute(
                OSKEY_IC_DECODER_HDB, icDecoderHDB_);
        }

        std::vector<Parameter>::const_iterator iter =
            icDecoderParameters_.begin();

        // add ic&decoder parameters
        for (; iter != icDecoderParameters_.end(); iter++) {
            ObjectState* parameterState =
                new ObjectState(OSNAME_IC_DECODER_PARAMETER);
            parameterState->setAttribute(
                OSKEY_IC_DECODER_PARAMETER_NAME, (*iter).name);
            parameterState->setAttribute(
                OSKEY_IC_DECODER_PARAMETER_VALUE, (*iter).value);
            icdecState->addChild(parameterState);
        }
        state->addChild(icdecState);
    }

    // add decompressor file data
    if (hasDecompressorFile()) {
        state->setAttribute(OSKEY_DECOMPRESSOR_FILE, decompressorFile_);
    }

    // add Generated FUs.
    ObjectState* fuGenerated = new ObjectState(OSNAME_FU_GENERATED);
    state->addChild(fuGenerated);
    for (const auto fug : fuGenerated_) {
        fuGenerated->addChild(fug.saveState());
    }

    // add FU implementations
    ObjectState* fuImplementations = new ObjectState(
        OSNAME_FU_IMPLEMENTATIONS);
    state->addChild(fuImplementations);
    for (int i = 0; i < fuImplementationCount(); i++) {
        UnitImplementationLocation& impl = fuImplementation(i);
        fuImplementations->addChild(impl.saveState());
    }

    // add RF implementations
    ObjectState* rfImplementations = new ObjectState(
        OSNAME_RF_IMPLEMENTATIONS);
    state->addChild(rfImplementations);
    for (int i = 0; i < rfImplementationCount(); i++) {
        UnitImplementationLocation& impl = rfImplementation(i);
        rfImplementations->addChild(impl.saveState());
    }

    // add IU implementations
    ObjectState* iuImplementations = new ObjectState(
        OSNAME_IU_IMPLEMENTATIONS);
    state->addChild(iuImplementations);
    for (int i = 0; i < iuImplementationCount(); i++) {
        UnitImplementationLocation& impl = iuImplementation(i);
        iuImplementations->addChild(impl.saveState());
    }

    // add bus implementations
    ObjectState* busImplementations = new ObjectState(
        OSNAME_BUS_IMPLEMENTATIONS);
    state->addChild(busImplementations);
    for (int i = 0; i < busImplementationCount(); i++) {
        UnitImplementationLocation& impl = busImplementation(i);
        busImplementations->addChild(impl.saveState());
    }

    // add socket implementations
    ObjectState* socketImplementations = new ObjectState(
        OSNAME_SOCKET_IMPLEMENTATIONS);
    state->addChild(socketImplementations);
    for (int i = 0; i < socketImplementationCount(); i++) {
        UnitImplementationLocation& impl = socketImplementation(i);
        socketImplementations->addChild(impl.saveState());
    }

    return state;
}

/**
 * Changes file paths in machine implementation to relative file paths.
 *
 * Tries to find relative file paths under provided search paths.
 * 
 * @param sPaths Search paths, used for finding relative paths.
 */
void
MachineImplementation::makeImplFilesRelative(
    const std::vector<std::string>& sPaths) {
    
    // ic&decoder files
    if (hasICDecoderPluginName()) {
        if (hasICDecoderPluginFile()) {
            string filePath = icDecoderPluginFile_;
            string relPath;
            if (FileSystem::makeRelativePath(sPaths, filePath, relPath)) {
                icDecoderPluginFile_ = relPath;
            } else if (FileSystem::makeRelativePath(
                    Environment::icDecoderPluginPaths(true),
                    filePath, relPath)) {
                icDecoderPluginFile_ = std::string("tce:") + relPath;
            }
        }
        if (hasICDecoderHDB()) {
            string filePath = icDecoderHDB_;
            string relPath;
            if (FileSystem::makeRelativePath(sPaths, filePath, relPath)) {
                icDecoderHDB_ = relPath;
            } else if (FileSystem::makeRelativePath(
                    Environment::decompressorPaths(true), filePath, relPath)) {
                icDecoderHDB_ = std::string("tce:") + relPath;
            }
        }
    }

    // decompressor file
    if (hasDecompressorFile()) {
        string filePath = decompressorFile_;
        string relPath;
        if (FileSystem::makeRelativePath(sPaths, filePath, relPath)) {
            decompressorFile_ = relPath;
        } else if (FileSystem::makeRelativePath(
                Environment::decompressorPaths(), filePath, relPath)) {
            decompressorFile_ = std::string("tce:") + relPath;
        }
    }

    // FU files
    for (int i = 0; i < fuImplementationCount(); i++) {
        UnitImplementationLocation& impl = fuImplementation(i);
        makeHDBPathRelative(sPaths, impl);
    }

    // RF files
    for (int i = 0; i < rfImplementationCount(); i++) {
        UnitImplementationLocation& impl = rfImplementation(i);
        makeHDBPathRelative(sPaths, impl);
    }

    // IU files
    for (int i = 0; i < iuImplementationCount(); i++) {
        UnitImplementationLocation& impl = iuImplementation(i);
        makeHDBPathRelative(sPaths, impl);
    }

    // bus files
    for (int i = 0; i < busImplementationCount(); i++) {
        UnitImplementationLocation& impl = busImplementation(i);
        makeHDBPathRelative(sPaths, impl);
    }

    // socket files
    for (int i = 0; i < socketImplementationCount(); i++) {
        UnitImplementationLocation& impl = socketImplementation(i);
        makeHDBPathRelative(sPaths, impl);
    }

    // Generated FUs and their operations.
    for (auto&& fug : FUGenerations()) {
        for (auto&& op : fug.operations()) {
            std::string& hdb = op.hdb;
            std::string rel;
            if (FileSystem::makeRelativePath(sPaths, hdb, rel)) {
                hdb = rel;
            } else if (FileSystem::makeRelativePath(
                           Environment::hdbPaths(true), hdb, rel)) {
                hdb = "tce:" + rel;
            }
        }
    }
}
 

/**
 * Checks that every file defined in IDF exists.
 *
 * If a file can't be found under current working directory or absolute
 * path, it is searched under default search paths. If the file is found
 * under a default search path, the original file path will be replaced with
 * the found path.
 * 
 * @param missingFiles Amount of missing files is returned using this.
 * @param alternativeFiles Amount of alt. files found for missing files.
 * @return True if every file was found locally or from absolute paths.
 */
bool
MachineImplementation::checkImplFiles(
    size_t& missingFiles,
    size_t& alternativeFiles) {
    
    missingFiles_.clear();
    alternativeFiles_.clear();
    
    // local search paths (current working directory)
    vector<string> localPaths;
    localPaths.push_back(FileSystem::currentWorkingDir());
    
    // default search paths for different implementation files
    vector<string> defSearchPaths;
    
    // file that will be searched under search paths
    TCEString filePath;

    // ic&decoder files
    if (hasICDecoderPluginName()) {
        if (hasICDecoderPluginFile()) {
            defSearchPaths = Environment::icDecoderPluginPaths();
            filePath = icDecoderPluginFile_;

            if (isLibraryImplFile(filePath, filePath)) {
                icDecoderPluginFile_ = filePath;
            // try to find file under local or default search paths
            } else if (checkImplFile(localPaths, defSearchPaths, filePath)) {
                // found under default search paths, fix the path
                icDecoderPluginFile_ = filePath;
            }
        }
      
        if (hasICDecoderHDB()) {
            defSearchPaths = Environment::hdbPaths();
            filePath = icDecoderHDB_;

            if (isLibraryImplFile(filePath, filePath)) {
                icDecoderHDB_ = filePath;
                // try to find file under local or default search paths
            } else if (checkImplFile(localPaths, defSearchPaths, filePath)) {
                icDecoderHDB_ = filePath;
            }
        }
    }
    
    // decompressor file
    if (hasDecompressorFile()) {
        defSearchPaths = Environment::decompressorPaths();
        filePath = decompressorFile_;

        if (isLibraryImplFile(filePath, filePath)) {
            decompressorFile_ = filePath;
            // try to find file under local or default search paths
        } else if (checkImplFile(localPaths, defSearchPaths, filePath)) {
            decompressorFile_ = filePath;
        }
    }

    // HDB files
    defSearchPaths = Environment::hdbPaths();

    // FU files
    for (int i = 0; i < fuImplementationCount(); i++) {
        UnitImplementationLocation& impl = fuImplementation(i);
        filePath = impl.hdbFileOriginal();

        if (isLibraryImplFile(filePath, filePath)) {
            impl.setHDBFile(filePath);
            // try to find file under local or default search paths
        } else if (checkImplFile(localPaths, defSearchPaths, filePath)) {
            impl.setHDBFile(filePath);
        }
    }
    
    // RF files
    for (int i = 0; i < rfImplementationCount(); i++) {
        UnitImplementationLocation& impl = rfImplementation(i);
        filePath = impl.hdbFileOriginal();
        
        if (isLibraryImplFile(filePath, filePath)) {
            impl.setHDBFile(filePath);
            // try to find file under local or default search paths
        } else if (checkImplFile(localPaths, defSearchPaths, filePath)) {
            impl.setHDBFile(filePath);
        }
    }

    // IU files
    for (int i = 0; i < iuImplementationCount(); i++) {
        UnitImplementationLocation& impl = iuImplementation(i);
        filePath = impl.hdbFileOriginal();

        if (isLibraryImplFile(filePath, filePath)) {
            impl.setHDBFile(filePath);
            // try to find file under local or default search paths
        } else if (checkImplFile(localPaths, defSearchPaths, filePath)) {
            impl.setHDBFile(filePath);
        }
    }

    // bus files
    for (int i = 0; i < busImplementationCount(); i++) {
        UnitImplementationLocation& impl = busImplementation(i);
        filePath = impl.hdbFileOriginal();

        if (isLibraryImplFile(filePath, filePath)) {
            impl.setHDBFile(filePath);
            // try to find file under local or default search paths
        } else if (checkImplFile(localPaths, defSearchPaths, filePath)) {
            impl.setHDBFile(filePath);
        }
    }

    // socket files
    for (int i = 0; i < socketImplementationCount(); i++) {
        UnitImplementationLocation& impl = socketImplementation(i);
        filePath = impl.hdbFileOriginal();

        if (isLibraryImplFile(filePath, filePath)) {
            impl.setHDBFile(filePath);
            // try to find file under local or default search paths
        } else if (checkImplFile(localPaths, defSearchPaths, filePath)) {
            impl.setHDBFile(filePath);
        }
    }

    // check amount of missing files and alternative files that were found

    missingFiles = missingFiles_.size();    
    alternativeFiles = 0;
    for (size_t i = 0; i < alternativeFiles_.size(); ++i) {
        if (alternativeFiles_.at(i) != "") {
            ++alternativeFiles;
        }
    }
    
    // were all the files found under local paths or from absolute paths?
    if (missingFiles_.size() == 0) {
        return true;
    }
    
    return false;
}

/**
 * Tries to find a file under provided search paths.
 *
 * If the file cannot be found under primary search paths, it is searched
 * under secondary paths. If an alternative file path is found under 
 * secondary paths, it is returned using the string reference parameter.
 * 
 * @param primarySearchPaths Paths where the file is searched first.
 * @param secondarySearchPaths Paths where the file is searched after.
 * @param file Path to file, might be relative of absolute. The alternative
 *             file path is returned in this.
 * @return True if alternative path was placed in the reference parameter.
 */
bool
MachineImplementation::checkImplFile(
    const std::vector<std::string>& primarySearchPaths,
    const std::vector<std::string>& secondarySearchPaths,
    std::string& file) {

    if (file == "") {
        return false;
    }

    // Remove tce file specifier.
    if (TCEString(file).startsWith("tce:")) {
        file = TCEString(file).replaceString("tce:", "");
    }

    // return if the file path has already been processed as a missing file
    for (unsigned int i = 0; i < missingFiles_.size(); ++i) {
        if (file.compare(missingFiles_.at(i)) == 0) {
            if (alternativeFiles_.at(i) != "") {
                file = alternativeFiles_.at(i);
                return true;
            } else {
                return false;
            }
        }
    }

    // first search: primary search paths
    try {
        FileSystem::findFileInSearchPaths(primarySearchPaths, file);
        return false;
    } catch (Exception& e) {
        // file was not found
    }

    // second search: secondary search paths (with the plain file name)
    try {
        string alternativePath = FileSystem::findFileInSearchPaths(
            secondarySearchPaths, FileSystem::fileOfPath(file));

        // file was not found, but alternative file path was found
        missingFiles_.push_back(file);
        alternativeFiles_.push_back(alternativePath);

        file = alternativePath;
        return true;
    } catch (Exception& e) {
        // file was not found, and no alternative path was found either
        missingFiles_.push_back(file);
        alternativeFiles_.push_back("");
        return false;
    }
}

/**
 * Tries to find a relative path for an HDB file. 
 *
 * If a relative path is found under any of the search paths, the first
 * match is saved. In case the HDB file path is invalid, the path field is 
 * left empty.
 *
 * @param searchPaths Search for relative paths is done under these paths.
 * @param implem Object containing a file path, which points to an HDB file.
 */
void
MachineImplementation::makeHDBPathRelative(
    const std::vector<std::string>& searchPaths,
    UnitImplementationLocation& implem) const {
    
    try {
        string filePath = implem.hdbFile();
        string relPath;
        if (FileSystem::makeRelativePath(searchPaths, filePath, relPath)) {
            implem.setHDBFile(relPath);
        } else if (FileSystem::makeRelativePath(Environment::hdbPaths(true),
                filePath, relPath)) {
            implem.setHDBFile(std::string("tce:") + relPath);
        }
    } catch (FileNotFound& e) {
        implem.setHDBFile("");
    }
}

/**
 * Finds implementation for the given unit from the given table.
 *
 * @param table The table to search from.
 * @param unitName Name of the unit.
 * @return The correct UnitImplementationLocation instance or NULL if no 
 *         implementation is found.
 */
UnitImplementationLocation*
MachineImplementation::findImplementation(
    const ImplementationTable& table,
    const std::string& unitName) const {

    for (ImplementationTable::const_iterator iter = table.begin(); 
         iter != table.end(); iter++) {
        UnitImplementationLocation* implementation = *iter;
        if (implementation->unitName() == unitName) {
            return implementation;
        }
    }

    return NULL;
}


/**
 * Ensures that the given index is valid for getting an instance from the
 * given implementation table.
 *
 * @param index The index.
 * @param table The table.
 * @exception OutOfRange If the given index is not valid.
 */
void
MachineImplementation::ensureIndexValidity(
    int index, const ImplementationTable& table) const {
    if (index < 0 || static_cast<size_t>(index) >= table.size()) {
        const string procName = "MachineImplementation::ensureIndexValidity";
        throw OutOfRange(__FILE__, __LINE__, procName);
    }
}

/**
 * Clears the state of the object.
 */
void
MachineImplementation::clearState() {
    SequenceTools::deleteAllItems(fuImplementations_);
    SequenceTools::deleteAllItems(rfImplementations_);
    SequenceTools::deleteAllItems(iuImplementations_);
    SequenceTools::deleteAllItems(busImplementations_);
    SequenceTools::deleteAllItems(socketImplementations_);
    fuGenerated_.clear();
    icDecoderParameters_.clear();
    icDecoderPluginName_ = "";
    icDecoderPluginFile_ = "";
    icDecoderHDB_ = "";
    decompressorFile_ = "";
    sourceIDF_ = "";
}

/**
 * Returns number of ic&decoder plugin parameters defined.
 */
unsigned
MachineImplementation::icDecoderParameterCount() const {
    return icDecoderParameters_.size();
}

/**
 * Returns name of the ic/decoder parameter with the given index.
 *
 * @param param Index of the parameter.
 * @return Name of the parameter.
 */
std::string
MachineImplementation::icDecoderParameterName(unsigned param) const {
    if (param >= icDecoderParameters_.size()) {
        const string procName =
            "MachineImplementation::icDecoderParameterName";
        throw OutOfRange(__FILE__, __LINE__, procName);

    }
    return icDecoderParameters_[param].name;
}

/**
 * Returns value of the ic/decoder parameter with the given index.
 *
 * @param param Index of the parameter.
 * @return Value of the parameter.
 */
std::string
MachineImplementation::icDecoderParameterValue(unsigned param) const {
    if (param >= icDecoderParameters_.size()) {
        const string procName = "MachineImplementation::icDecoderParamterName";
        throw OutOfRange(__FILE__, __LINE__, procName);

    }
    return icDecoderParameters_[param].value;
}

/**
 * Returns value of the ic/decoder parameter with the given name.
 *
 * @param name Name of the parameter.
 * @return Value of the parameter.
 */
std::string
MachineImplementation::icDecoderParameterValue(const std::string& name) const {

    std::vector<Parameter>::const_iterator iter =
        icDecoderParameters_.begin();

    for (; iter != icDecoderParameters_.end(); iter++) {
        if ((*iter).name == name) {
            return (*iter).value;
        }
    }

    // Parameter value not defined.
    return "";
}

/**
 * Sets value of an ic/decoder parameter.
 *
 * @param name Name of the parameter.
 * @param value Value of the parameter.
 */
void
MachineImplementation::setICDecoderParameter(
    const std::string& name, const std::string& value) {

    std::vector<Parameter>::iterator iter =
        icDecoderParameters_.begin();

    // Check if the parameter already exists.
    for (; iter != icDecoderParameters_.end(); iter++) {
        if ((*iter).name == name) {
            (*iter).value = value;
            return;
        }
    }

    // New parameter.
    Parameter parameter = { name, value };
    icDecoderParameters_.push_back(parameter);
}

/**
 * Sets the ic/decoder plugin name.
 *
 * @param name Name of the ic/decoder plugin.
 */
void
MachineImplementation::setICDecoderPluginName(const std::string& name) {
    icDecoderPluginName_ = name;

}


/**
 * Sets the ic/decoder plugin file.
 *
 * @param file Full path of the ic/decoder plugin file.
 */
void
MachineImplementation::setICDecoderPluginFile(const std::string& file) {
    vector<string> paths = Environment::icDecoderPluginPaths();
    paths.insert(paths.begin(), FileSystem::directoryOfPath(sourceIDF_));
    string expandedPath = FileSystem::expandTilde(file);
    icDecoderPluginFile_ =
        FileSystem::findFileInSearchPaths(paths, expandedPath);
}

/**
 * Sets the ic/decoder HDB file.
 *
 * @param file Full path of the ic/decoder HDB file.
 */
void
MachineImplementation::setICDecoderHDB(const std::string& file) {
    vector<string> paths = Environment::hdbPaths();
    paths.insert(paths.begin(), FileSystem::directoryOfPath(sourceIDF_));
    string expandedPath = FileSystem::expandTilde(file);
    icDecoderHDB_ =
        FileSystem::findFileInSearchPaths(paths, expandedPath);
}

/**
 * Sets the decompressor block file.
 *
 * @param file Full path to the decompressor block file.
 */
void
MachineImplementation::setDecompressorFile(const std::string& file) {
    vector<string> paths = Environment::decompressorPaths();
    paths.insert(paths.begin(), FileSystem::directoryOfPath(sourceIDF_));
    decompressorFile_ = FileSystem::findFileInSearchPaths(paths, file);
}

/**
 * Clears the ic/decoder parameters.
 */
void
MachineImplementation::clearICDecoderParameters() {
    icDecoderParameters_.clear();
}

/**
 * Loads a MachineImplementation from the given IDF file.
 *
 * @param idfFileName The name of the file to load the IDF from.
 * @return A machine implementation instance.
 * @exception Exception In case some error occured.
 */
MachineImplementation*
MachineImplementation::loadFromIDF(const std::string& idfFileName) {
    IDFSerializer serializer;
    serializer.setSourceFile(idfFileName);

    return serializer.readMachineImplementation();
}

/**
 * Returns true if the given file is a library file of TCE.
 *
 * If the file is a TCE library file, its absolute absolute path returned via
 * reference.
 *
 * @param path The file path that is relative or absolute.
 * @param resolvedPath The resolved absolute path.
 * @return True, if the file is TCE library file. Otherwise, false.
 */
bool
MachineImplementation::isLibraryImplFile(
        const std::string& path,
        std::string& resolvedPath) {

    TCEString toSearched(path);
    toSearched.replaceString("tce:", "");

    std::vector<std::string> defaultPaths;
    std::vector<std::string> tmp = Environment::hdbPaths(true);
    defaultPaths.insert(defaultPaths.end(), tmp.begin(), tmp.end());
    tmp = Environment::icDecoderPluginPaths(true);
    defaultPaths.insert(defaultPaths.end(), tmp.begin(), tmp.end());
    tmp = Environment::decompressorPaths(true);
    defaultPaths.insert(defaultPaths.end(), tmp.begin(), tmp.end());

    try {
        resolvedPath = FileSystem::findFileInSearchPaths(
            defaultPaths, toSearched);
        return true;
    } catch (Exception& e) {
        // file was not found
    }

    return false;
}

/**
 * Return all FUs to generate.
 */
const std::vector<FUGenerated>&
MachineImplementation::FUGenerations() const {
    return fuGenerated_;
}

/**
 * Return all FUs to generate.
 */
std::vector<FUGenerated>&
MachineImplementation::FUGenerations() {
    return fuGenerated_;
}

/**
 * Return true if fu is to be generated.
 *
 * @param name Name of the FU Generation to check.
 */
bool
MachineImplementation::hasFUGeneration(const std::string& name) const {
    for (const auto fug : fuGenerated_) {
        if (fug.name() == name) {
            return true;
        }
    }
    return false;
}

/**
 * Return true if RF is to be generated.
 *
 * @param name Name of the RF Generation to check.
 */
bool
MachineImplementation::hasRFGeneration(const std::string& name) const {
    for (const auto rfg : RFGenerated_) {
        if (rfg.name() == name) {
            return true;
        }
    }
    return false;
}

/**
 * Remove fu from generation list.
 *
 * @param name Name of the FU Generation to remove.
 */
void
MachineImplementation::removeFuGeneration(const std::string& name) {
    fuGenerated_.erase(
        std::remove_if(
            fuGenerated_.begin(), fuGenerated_.end(),
            [name](FUGenerated& fu) { return fu.name() == name; }),
        fuGenerated_.end());
}

/**
 * Add fu to generation list.
 *
 * @param fug FU Generation to add.
 */
void
MachineImplementation::addFuGeneration(const FUGenerated& fug) {
    fuGenerated_.emplace_back(fug);
}

/**
 * Return all RFs to generate.
 */
const std::vector<RFGenerated>&
MachineImplementation::RFGenerations() const {
    return RFGenerated_;
}

/**
 * Return all RFs to generate.
 */
std::vector<RFGenerated>&
MachineImplementation::RFGenerations() {
    return RFGenerated_;
}

/**
 * Add RF to generation list.
 *
 * @param rfg RF Generation to add.
 */
void
MachineImplementation::addRFGeneration(const RFGenerated& rfg) {
    RFGenerated_.emplace_back(rfg);
}
}

