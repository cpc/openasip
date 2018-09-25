/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file BEMSerializer.cc
 *
 * Implementation of BEMSerializer class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#include <string>
#include <vector>

#include "BEMSerializer.hh"
#include "BinaryEncoding.hh"
#include "SocketCodeTable.hh"
#include "FUPortCode.hh"
#include "RFPortCode.hh"
#include "IUPortCode.hh"
#include "ImmediateControlField.hh"
#include "MoveSlot.hh"
#include "LImmDstRegisterField.hh"
#include "ImmediateSlotField.hh"
#include "GuardField.hh"
#include "SourceField.hh"
#include "DestinationField.hh"
#include "GPRGuardEncoding.hh"
#include "FUGuardEncoding.hh"
#include "UnconditionalGuardEncoding.hh"
#include "SocketEncoding.hh"
#include "BridgeEncoding.hh"
#include "ImmediateEncoding.hh"
#include "NOPEncoding.hh"
#include "Application.hh"
#include "Environment.hh"
#include "ObjectState.hh"

using std::string;
using std::vector;

const string TRUE = "true";
const string FALSE = "false";

const string ADF_ENCODING = "adf-encoding";
const string ADF_VERSION = "version";
const string REQUIRED_VERSION = "required-version";

const string SOCKET_CODE_TABLE = "map-ports";
const string SC_TABLE_NAME = "name";
const string SC_TABLE_EXTRA_BITS = "extra-bits";
const string FU_PORT_CODE = "fu-port-code";
const string FU_PORT_CODE_PORT = "name";
const string FU_PORT_CODE_FU = "fu";
const string FU_PORT_CODE_OPERATION = "operation";
const string FU_PORT_CODE_ENCODING = "encoding";
const string FU_PORT_CODE_EXTRA_BITS = "extra-bits";
const string RF_PORT_CODE = "rf-port-code";
const string RF_PORT_CODE_RF = "rf";
const string RF_PORT_CODE_INDEX_WIDTH = "index-width";
const string RF_PORT_CODE_ENCODING = "encoding";
const string RF_PORT_CODE_EXTRA_BITS = "extra-bits";
const string IU_PORT_CODE = "iu-port-code";
const string IU_PORT_CODE_IU = "iu";

const string LONG_IMMEDIATE_TAG = "long-immediate-tag";
const string LIMM_TAG_POSITION = "pos";
const string LIMM_TAG_EXTRA_BITS = "extra-bits";
const string LIMM_TAG_MAP = "map";
const string LIMM_TAG_MAP_NAME = "name";

const string IMM_REG_FIELD = "immediate-register-field";
const string IMM_REG_FIELD_POS = "pos";
const string IMM_REG_FIELD_WIDTH = "width";
const string IMM_REG_FIELD_ITEMP = "instruction-template";
const string IMM_REG_FIELD_ITEMP_NAME = "name";
const string IMM_REG_FIELD_ITEMP_REG_INDEX = "reg-index-of";

const string SLOT = "slot";
const string SLOT_NAME = "name";
const string SLOT_POSITION = "pos";
const string SLOT_EXTRA_BITS = "extra-bits";
const string SLOT_GUARD = "guard";
const string SLOT_GUARD_POSITION = "pos";
const string SLOT_GUARD_EXTRA_BITS = "extra-bits";
const string REG_GUARD_CODE = "reg-guard-code";
const string REG_GUARD_CODE_RF = "rf";
const string REG_GUARD_CODE_INDEX = "index";
const string REG_GUARD_CODE_INVERTED = "inv";
const string PORT_GUARD_CODE = "port-guard-code";
const string PORT_GUARD_CODE_FU = "fu";
const string PORT_GUARD_CODE_PORT = "port";
const string PORT_GUARD_CODE_INVERTED = "inv";
const string ALWAYS_TRUE_GUARD_CODE = "always-true-guard-code";
const string ALWAYS_FALSE_GUARD_CODE = "always-false-guard-code";

const string SLOT_SOURCE = "source";
const string SLOT_DESTINATION = "destination";

const string SRC_DST_POSITION = "pos";
const string SRC_DST_EXTRA_BITS = "extra-bits";
const string SRC_DST_ID_POS = "id-pos";
const string ID_POS_LEFT = "left";
const string ID_POS_RIGHT = "right";

const string SOCKET = "socket";
const string SOCKET_NAME = "name";
const string SOCKET_MAP = "map";
const string SOCKET_MAP_EXTRA_BITS = "extra-bits";
const string SOCKET_MAP_CODES = "codes";

const string SOURCE_IMMEDIATE = "immediate";
const string SOURCE_IMMEDIATE_WIDTH = "width";
const string SOURCE_IMMEDIATE_MAP = "map";
const string SOURCE_IMMEDIATE_MAP_EXTRA_BITS = "extra-bits";
const string SOURCE_BRIDGE = "bridge";
const string SOURCE_BRIDGE_NAME = "name";
const string SOURCE_BRIDGE_MAP = "map";
const string SOURCE_BRIDGE_MAP_EXTRA_BITS = "extra-bits";

const string SRC_DST_NO_OPERATION = "no-operation";
const string SRC_DST_NO_OPERATION_MAP = "map";
const string SRC_DST_NO_OPERATION_MAP_EXTRA_BITS = "extra-bits";

const string IMMEDIATE_SLOT = "immediate-slot";
const string IMMEDIATE_SLOT_NAME = "name";
const string IMMEDIATE_SLOT_WIDTH = "width";
const string IMMEDIATE_SLOT_POS = "pos";

const string EXTRA_BITS = "extra-bits";
const string MAP = "map";

const string BEM_SCHEMA_FILE = "bem/BEM_Schema.xsd";

/**
 * The constructor.
 */
BEMSerializer::BEMSerializer() : XMLSerializer() {
    setSchemaFile(Environment::schemaDirPath(BEM_SCHEMA_FILE));
    setUseSchema(true);
}


/**
 * The destructor.
 */
BEMSerializer::~BEMSerializer() {
}


/**
 * Reads the BEM file and returns an ObjectState tree that can be loaded by
 * BinaryEncoding.
 *
 * @return An ObjectState tree that can be loaded by BinaryEncoding.
 * @exception SerializerException If an error occurs while reading the BEM
 *                                file.
 */
ObjectState*
BEMSerializer::readState()
    throw (SerializerException) {

    ObjectState* fileState = XMLSerializer::readState();

    double version = fileState->doubleAttribute(ADF_VERSION);
    if (version < 1.2) {
        convertZeroEncExtraBits(fileState);
    }
    ObjectState* omState = convertToOMFormat(fileState);
    delete fileState;
    return omState;
}

/**
 * Recursively convert all encodings with zero encodings to have
 * one more extra bit.
 */
void BEMSerializer::convertZeroEncExtraBits(ObjectState* os) {
    if (os->name() == MAP) {
        if (os->intValue() == 0 && os->hasAttribute(EXTRA_BITS)) {
            int extraBits = os->intAttribute(EXTRA_BITS);
            os->setAttribute(EXTRA_BITS, extraBits + 1);
        }
    }
    if (os->name() == FU_PORT_CODE &&
        os->hasChild(EXTRA_BITS) &&
        os->hasChild(FU_PORT_CODE_ENCODING)) {
        if (os->childByName(FU_PORT_CODE_ENCODING)->intValue() == 0) {
            ObjectState* eb = os->childByName(EXTRA_BITS);
            eb ->setValue(eb->intValue()+1);
        }
    }
    // call recursively for children
    for (int i = 0; i < os->childCount(); i++) {
        ObjectState* child = os->child(i);
        convertZeroEncExtraBits(child);
    }
}


/**
 * Writes the given ObjectState tree that represents a BEM object model to a
 * BEM file.
 *
 * @param state ObjectState tree obtained by calling
 *              BinaryEncoding::saveState().
 * @exception SerializerException If the destination file cannot be written.
 */
void
BEMSerializer::writeState(const ObjectState* state)
    throw (SerializerException) {

    ObjectState* fileState = convertToFileFormat(state);
    XMLSerializer::writeState(fileState);
    delete fileState;
}


/**
 * Reads the BEM file and creates a BinaryEncoding instance from it.
 *
 * @return The newly created BinaryEncoding instance.
 */
BinaryEncoding*
BEMSerializer::readBinaryEncoding()
    throw (SerializerException, ObjectStateLoadingException) {

    ObjectState* omState = readState();
    BinaryEncoding* bem = new BinaryEncoding(omState);
    delete omState;
    return bem;
}


/**
 * Writes the given binary encoding to a BEM file.
 *
 * @param bem The binary encoding.
 * @exception SerializerException If the destination file cannot be written.
 */
void
BEMSerializer::writeBinaryEncoding(const BinaryEncoding& bem)
    throw (SerializerException) {

    ObjectState* omState = bem.saveState();
    writeState(omState);
    delete omState;
}


/**
 * Converts the given ObjectState tree that represents a BEM file to an
 * ObjectState tree that can be loaded by BinaryEncoding.
 *
 * @param fileState ObjectState tree that represents a BEM file.
 * @return A newly created ObjectState tree that can be loaded by
 *         BinaryEncoding.
 */
ObjectState*
BEMSerializer::convertToOMFormat(const ObjectState* fileState) {

    ObjectState* bem = new ObjectState(BinaryEncoding::OSNAME_BEM);

    for (int i = 0; i < fileState->childCount(); i++) {
        ObjectState* child = fileState->child(i);
        if (child->name() == SOCKET_CODE_TABLE) {
            bem->addChild(socketCodeTableToOM(child));
        } else if (child->name() == SLOT) {
            bem->addChild(moveSlotToOM(child));
        } else if (child->name() == IMMEDIATE_SLOT) {
            bem->addChild(immediateSlotToOM(child));
        } else if (child->name() == LONG_IMMEDIATE_TAG) {
            bem->addChild(longImmTagToOM(child));
        } else if (child->name() == IMM_REG_FIELD) {
            bem->addChild(longImmDstRegFieldToOM(child));
        } else {
            assert(false);
        }
    }

    return bem;
}


/**
 * Converts the ObjectState tree that represent the BEM object model to an
 * ObjectState tree that represents the BEM file format.
 *
 * @param omState ObjectState tree that represents BEM object model (returned
 *                by BinaryEncoding::saveState()).
 * @return ObjectState tree representing BEM file.
 */
ObjectState*
BEMSerializer::convertToFileFormat(const ObjectState* state) {

    ObjectState* fileState = new ObjectState(ADF_ENCODING);
    const string version = "1.2";
    const string requiredVersion = "1.2";
    fileState->setAttribute(ADF_VERSION, version);
    fileState->setAttribute(REQUIRED_VERSION, requiredVersion);

    // create socket code tables
    for (int i = 0; i < state->childCount(); i++) {
        ObjectState* child = state->child(i);
        if (child->name() == SocketCodeTable::OSNAME_SOCKET_CODE_TABLE) {
            fileState->addChild(socketCodeTableToFile(child));
	}
    }

    // create long immediate tag
    for (int i = 0; i < state->childCount(); i++) {
        ObjectState* child = state->child(i);
        if (child->name() ==
            ImmediateControlField::OSNAME_IMM_CONTROL_FIELD) {
            fileState->addChild(longImmTagToFile(child));
        }
    }

    // create long immediate destination register fields
    for (int i = 0; i < state->childCount(); i++) {
        ObjectState* child = state->child(i);
        if (child->name() == 
            LImmDstRegisterField::OSNAME_LIMM_DST_REGISTER_FIELD) {
            fileState->addChild(longImmDstRegisterFieldToFile(child));
        }
    }         

    // create move slots
    for (int i = 0; i < state->childCount(); i++) {
        ObjectState* child = state->child(i);
        if (child->name() == MoveSlot::OSNAME_MOVE_SLOT) {
            fileState->addChild(moveSlotToFile(child));
        }
    }

    // create immediate slots
    for (int i = 0; i < state->childCount(); i++) {
        ObjectState* child = state->child(i);
        if (child->name() == 
            ImmediateSlotField::OSNAME_IMMEDIATE_SLOT_FIELD) {
            fileState->addChild(immediateSlotToFile(child));
        }
    }

    return fileState;
}


/**
 * Converts the given ObjectState tree that represents a socket code table in
 * the object model format to BEM file format.
 *
 * @return The newly created ObjectState tree.
 */
ObjectState*
BEMSerializer::socketCodeTableToFile(const ObjectState* omTable) {

    ObjectState* scTable = new ObjectState(SOCKET_CODE_TABLE);
    scTable->setAttribute(
	SC_TABLE_NAME, omTable->stringAttribute(
            SocketCodeTable::OSKEY_NAME));

    // create extra-bits element
    ObjectState* extraBits = new ObjectState(SC_TABLE_EXTRA_BITS);
    scTable->addChild(extraBits);
    extraBits->setValue(
	omTable->intAttribute(SocketCodeTable::OSKEY_EXTRA_BITS));

    // create fu-port-code elements
    for (int i = 0; i < omTable->childCount(); i++) {
	ObjectState* portCode = omTable->child(i);
	if (portCode->name() == FUPortCode::OSNAME_FU_PORT_CODE) {
	    ObjectState* fuPortCode = new ObjectState(FU_PORT_CODE);
	    scTable->addChild(fuPortCode);
	    fuPortCode->setAttribute(
		FU_PORT_CODE_PORT,
		portCode->stringAttribute(FUPortCode::OSKEY_PORT_NAME));
	    fuPortCode->setAttribute(
		FU_PORT_CODE_FU,
		portCode->stringAttribute(FUPortCode::OSKEY_UNIT_NAME));
	    if (portCode->hasAttribute(FUPortCode::OSKEY_OPERATION_NAME)) {
		fuPortCode->setAttribute(
		    FU_PORT_CODE_OPERATION,
		    portCode->stringAttribute(
			FUPortCode::OSKEY_OPERATION_NAME));
	    }
	    ObjectState* encoding = new ObjectState(FU_PORT_CODE_ENCODING);
	    fuPortCode->addChild(encoding);
	    encoding->setValue(
		portCode->stringAttribute(FUPortCode::OSKEY_ENCODING));
	    ObjectState* extraBits = new ObjectState(FU_PORT_CODE_EXTRA_BITS);
	    fuPortCode->addChild(extraBits);
	    extraBits->setValue(
		portCode->stringAttribute(FUPortCode::OSKEY_EXTRA_BITS));
	}
    }

    // create rf-port-code elements
    for (int i = 0; i < omTable->childCount(); i++) {
	ObjectState* portCode = omTable->child(i);
	if (portCode->name() == RFPortCode::OSNAME_RF_PORT_CODE) {
	    ObjectState* rfPortCode = new ObjectState(RF_PORT_CODE);
	    scTable->addChild(rfPortCode);
	    rfPortCode->setAttribute(
		RF_PORT_CODE_RF,
		portCode->stringAttribute(RFPortCode::OSKEY_UNIT_NAME));
	    rfPortCode->setAttribute(
		RF_PORT_CODE_INDEX_WIDTH,
		portCode->stringAttribute(RFPortCode::OSKEY_INDEX_WIDTH));
            if (portCode->hasAttribute(RFPortCode::OSKEY_ENCODING)) {
                ObjectState* encoding = new ObjectState(
                    RF_PORT_CODE_ENCODING);
                rfPortCode->addChild(encoding);
                encoding->setValue(
                    portCode->stringAttribute(RFPortCode::OSKEY_ENCODING));
                ObjectState* extraBits = new ObjectState(
                    RF_PORT_CODE_EXTRA_BITS);
                rfPortCode->addChild(extraBits);
                extraBits->setValue(
                    portCode->stringAttribute(RFPortCode::OSKEY_EXTRA_BITS));
            }
	}
    }

    // create iu-port-code elements
    for (int i = 0; i < omTable->childCount(); i++) {
	ObjectState* portCode = omTable->child(i);
	if (portCode->name() == IUPortCode::OSNAME_IU_PORT_CODE) {
	    ObjectState* iuPortCode = new ObjectState(IU_PORT_CODE);
	    scTable->addChild(iuPortCode);
	    iuPortCode->setAttribute(
		IU_PORT_CODE_IU,
		portCode->stringAttribute(IUPortCode::OSKEY_UNIT_NAME));
	    iuPortCode->setAttribute(
		RF_PORT_CODE_INDEX_WIDTH,
		portCode->stringAttribute(IUPortCode::OSKEY_INDEX_WIDTH));
            if (portCode->hasAttribute(IUPortCode::OSKEY_ENCODING)) {
                ObjectState* encoding = new ObjectState(
                    RF_PORT_CODE_ENCODING);
                iuPortCode->addChild(encoding);
                encoding->setValue(
                    portCode->stringAttribute(IUPortCode::OSKEY_ENCODING));
                ObjectState* extraBits = new ObjectState(
                    RF_PORT_CODE_EXTRA_BITS);
                iuPortCode->addChild(extraBits);
                extraBits->setValue(
                    portCode->stringAttribute(IUPortCode::OSKEY_EXTRA_BITS));
            }
	}
    }

    return scTable;
}


/**
 * Converts the given ObjectState tree that represents immediate control field
 * in object model format to BEM file file format.
 *
 * @param immTag The ObjectState tree.
 * @return The newly created ObjectState tree.
 */
ObjectState*
BEMSerializer::longImmTagToFile(const ObjectState* immTag) {

    ObjectState* fileTag = new ObjectState(LONG_IMMEDIATE_TAG);
    ObjectState* position = new ObjectState(LIMM_TAG_POSITION);
    fileTag->addChild(position);
    position->setValue(
	immTag->stringAttribute(ImmediateControlField::OSKEY_POSITION));
    ObjectState* extraBits = new ObjectState(LIMM_TAG_EXTRA_BITS);
    fileTag->addChild(extraBits);
    extraBits->setValue(
	immTag->stringAttribute(ImmediateControlField::OSKEY_EXTRA_BITS));

    // create map elements
    for (int i = 0; i < immTag->childCount(); i++) {
	ObjectState* child = immTag->child(i);
	assert(child->name() == ImmediateControlField::OSNAME_TEMPLATE_MAP);
	ObjectState* map = new ObjectState(LIMM_TAG_MAP);
	fileTag->addChild(map);
	map->setAttribute(
	    LIMM_TAG_MAP_NAME,
	    child->stringAttribute(
		ImmediateControlField::OSKEY_TEMPLATE_NAME));
	map->setValue(child->stringAttribute(
                          ImmediateControlField::OSKEY_ENCODING));
    }

    return fileTag;
}


/**
 * Converts the given ObjectState tree that represents a move slot in object
 * model format to BEM file format.
 *
 * @param slotState The ObjectState tree.
 * @return The newly created ObjectState tree.
 */
ObjectState*
BEMSerializer::moveSlotToFile(const ObjectState* slotState) {

    ObjectState* slot = new ObjectState(SLOT);
    slot->setAttribute(
        SLOT_NAME, slotState->stringAttribute(MoveSlot::OSKEY_BUS_NAME));

    ObjectState* position = new ObjectState(SLOT_POSITION);
    slot->addChild(position);
    position->setValue(slotState->stringAttribute(MoveSlot::OSKEY_POSITION));
    ObjectState* extraBits = new ObjectState(SLOT_EXTRA_BITS);
    slot->addChild(extraBits);
    extraBits->setValue(
        slotState->stringAttribute(MoveSlot::OSKEY_EXTRA_BITS));

    // add guard field
    if (slotState->hasChild(GuardField::OSNAME_GUARD_FIELD)) {
        ObjectState* guardField = slotState->childByName(
            GuardField::OSNAME_GUARD_FIELD);
        slot->addChild(guardFieldToFile(guardField));
    }

    // add source field
    if (slotState->hasChild(SourceField::OSNAME_SOURCE_FIELD)) {
        ObjectState* sourceField = slotState->childByName(
            SourceField::OSNAME_SOURCE_FIELD);
        slot->addChild(sourceFieldToFile(sourceField));
    }

    // add destination field
    if (slotState->hasChild(DestinationField::OSNAME_DESTINATION_FIELD)) {
        ObjectState* destinationField = slotState->childByName(
            DestinationField::OSNAME_DESTINATION_FIELD);
        slot->addChild(destinationFieldToFile(destinationField));
    }

    return slot;
}


/**
 * Converts the given ObjectState tree that represents a guard field
 * in object model to BEM file format.
 *
 * @param gFieldState The ObjectState tree.
 * @return The newly created ObjectState tree.
 */
ObjectState*
BEMSerializer::guardFieldToFile(const ObjectState* gFieldState) {

    ObjectState* guard = new ObjectState(SLOT_GUARD);
    ObjectState* position = new ObjectState(SLOT_GUARD_POSITION);
    guard->addChild(position);
    position->setValue(
        gFieldState->stringAttribute(GuardField::OSKEY_POSITION));
    ObjectState* extraBits = new ObjectState(SLOT_GUARD_EXTRA_BITS);
    guard->addChild(extraBits);
    extraBits->setValue(
        gFieldState->stringAttribute(GuardField::OSKEY_EXTRA_BITS));

    // add register guard encodings
    for (int i = 0; i < gFieldState->childCount(); i++) {
        ObjectState* child = gFieldState->child(i);
        if (child->name() == GPRGuardEncoding::OSNAME_GPR_GUARD_ENCODING) {
            ObjectState* regGuard = new ObjectState(REG_GUARD_CODE);
            guard->addChild(regGuard);
            regGuard->setAttribute(
                REG_GUARD_CODE_RF,
                child->stringAttribute(GPRGuardEncoding::OSKEY_RF_NAME));
            regGuard->setAttribute(
                REG_GUARD_CODE_INDEX,
                child->stringAttribute(
                    GPRGuardEncoding::OSKEY_REGISTER_INDEX));
            string inverted = child->boolAttribute(
                GPRGuardEncoding::OSKEY_INVERTED) ? TRUE : FALSE;
            regGuard->setAttribute(REG_GUARD_CODE_INVERTED, inverted);
            regGuard->setValue(
                child->stringAttribute(GPRGuardEncoding::OSKEY_ENCODING));
        }
    }

    // add port guard encodings
    for (int i = 0; i < gFieldState->childCount(); i++) {
        ObjectState* child = gFieldState->child(i);
        if (child->name() == FUGuardEncoding::OSNAME_FU_GUARD_ENCODING) {
            ObjectState* fuGuard = new ObjectState(PORT_GUARD_CODE);
            guard->addChild(fuGuard);
            fuGuard->setAttribute(
                PORT_GUARD_CODE_FU,
                child->stringAttribute(FUGuardEncoding::OSKEY_FU_NAME));
            fuGuard->setAttribute(
                PORT_GUARD_CODE_PORT,
                child->stringAttribute(FUGuardEncoding::OSKEY_PORT_NAME));
            string inverted = child->boolAttribute(
                FUGuardEncoding::OSKEY_INVERTED) ? TRUE : FALSE;
            fuGuard->setAttribute(PORT_GUARD_CODE_INVERTED, inverted);
            fuGuard->setValue(
                child->stringAttribute(FUGuardEncoding::OSKEY_ENCODING));
        }
    }

    // add unconditional always-true-guard encoding
    for (int i = 0; i < gFieldState->childCount(); i++) {
        ObjectState* child = gFieldState->child(i);
        if (child->name() == 
            UnconditionalGuardEncoding::
            OSNAME_UNCONDITIONAL_GUARD_ENCODING &&
            !child->boolAttribute(
                UnconditionalGuardEncoding::OSKEY_INVERTED)) {
            ObjectState* ucGuard = new ObjectState(ALWAYS_TRUE_GUARD_CODE);
            guard->addChild(ucGuard);
            ucGuard->setValue(
                child->stringAttribute(
                    UnconditionalGuardEncoding::OSKEY_ENCODING));
        }
    }

    // add always-false-guard encoding
    for (int i = 0; i < gFieldState->childCount(); i++) {
        ObjectState* child = gFieldState->child(i);
        if (child->name() == 
            UnconditionalGuardEncoding::OSNAME_UNCONDITIONAL_GUARD_ENCODING 
            && child->boolAttribute(
                UnconditionalGuardEncoding::OSKEY_INVERTED)) {
            ObjectState* ucGuard = new ObjectState(ALWAYS_FALSE_GUARD_CODE);
            guard->addChild(ucGuard);
            ucGuard->setValue(
                child->stringAttribute(
                    UnconditionalGuardEncoding::OSKEY_ENCODING));
        }
    }

    return guard;
}


/**
 * Converts the given ObjectState tree that represents a source field in
 * object model to BEM file format.
 *
 * @param sFieldState The ObjectState tree.
 * @return The newly created ObjectState tree.
 */
ObjectState*
BEMSerializer::sourceFieldToFile(const ObjectState* sFieldState) {

    ObjectState* source = slotFieldToFile(sFieldState);
    source->setName(SLOT_SOURCE);

    // add immediate element in front of socket elements, it has to be done
    // by removing the socket elements
    if (sFieldState->hasChild(ImmediateEncoding::OSNAME_IMM_ENCODING)) {
        std::vector<ObjectState*> socketElems;
        for (int i = 0; i < source->childCount();) {
            ObjectState* child = source->child(i);
            if (child->name() == SOCKET) {
                socketElems.push_back(child);
                source->removeChild(child);
            } else {
                i++;
            }
        }
        ObjectState* immediate = new ObjectState(SOURCE_IMMEDIATE);
        source->addChild(immediate);
        ObjectState* encodingState = sFieldState->childByName(
            ImmediateEncoding::OSNAME_IMM_ENCODING);
        immediate->setAttribute(
            SOURCE_IMMEDIATE_WIDTH,
            encodingState->stringAttribute(
                ImmediateEncoding::OSKEY_IMM_WIDTH));
        ObjectState* map = new ObjectState(SOURCE_IMMEDIATE_MAP);
        immediate->addChild(map);
        map->setAttribute(SOURCE_IMMEDIATE_MAP_EXTRA_BITS,
                          encodingState->stringAttribute(
                              ImmediateEncoding::OSKEY_EXTRA_BITS));
        map->setValue(
            encodingState->stringAttribute(
                ImmediateEncoding::OSKEY_ENCODING));

        // add the socket elements
        for (vector<ObjectState*>::const_iterator iter = socketElems.begin();
             iter != socketElems.end(); iter++) {
            source->addChild(*iter);
        }
    }

    // add bridge elements
    for (int i = 0; i < sFieldState->childCount(); i++) {
        ObjectState* child = sFieldState->child(i);
        if (child->name() == BridgeEncoding::OSNAME_BRIDGE_ENCODING) {
            ObjectState* bridge = new ObjectState(SOURCE_BRIDGE);
            source->addChild(bridge);
            bridge->setAttribute(
                SOURCE_BRIDGE_NAME,
                child->stringAttribute(BridgeEncoding::OSKEY_BRIDGE_NAME));
            ObjectState* map = new ObjectState(SOURCE_BRIDGE_MAP);
            bridge->addChild(map);
            map->setAttribute(
                SOURCE_BRIDGE_MAP_EXTRA_BITS,
                child->stringAttribute(BridgeEncoding::OSKEY_EXTRA_BITS));
            map->setValue(
                child->stringAttribute(BridgeEncoding::OSKEY_ENCODING));
        }
    }

    // move the no-operation element to the end
    if (source->hasChild(SRC_DST_NO_OPERATION)) {
        ObjectState* nop = source->childByName(SRC_DST_NO_OPERATION);
        source->removeChild(nop);
        source->addChild(nop);
    }

    return source;
}


/**
 * Converts the given ObjectState tree that represents a destination field in
 * object model to BEM file format.
 *
 * @param dFieldState The ObjectState tree.
 * @return The newly created ObjectState tree.
 */
ObjectState*
BEMSerializer::destinationFieldToFile(const ObjectState* dFieldState) {
    ObjectState* destination = slotFieldToFile(dFieldState);
    destination->setName(SLOT_DESTINATION);
    return destination;
}


/**
 * Converts the given ObjectState tree that represents either source or
 * destination in BEM object model to BEM file format.
 *
 * @param sFieldState The ObjectState tree.
 * @return The newly created ObjectState tree.
 */
ObjectState*
BEMSerializer::slotFieldToFile(const ObjectState* sFieldState) {

   ObjectState* srcOrDst = new ObjectState(SLOT_SOURCE);
   ObjectState* position = new ObjectState(SRC_DST_POSITION);
   srcOrDst->addChild(position);
   position->setValue(
       sFieldState->stringAttribute(SlotField::OSKEY_POSITION));
   ObjectState* extraBits = new ObjectState(SRC_DST_EXTRA_BITS);
   srcOrDst->addChild(extraBits);
   extraBits->setValue(
       sFieldState->stringAttribute(SlotField::OSKEY_EXTRA_BITS));
   ObjectState* encodingPos = new ObjectState(SRC_DST_ID_POS);
   srcOrDst->addChild(encodingPos);
   if (sFieldState->intAttribute(SlotField::OSKEY_COMPONENT_ID_POSITION) == 
       BinaryEncoding::LEFT) {
       encodingPos->setValue(ID_POS_LEFT);
   } else {
       encodingPos->setValue(ID_POS_RIGHT);
   }

   // add socket elements
   for (int i = 0; i < sFieldState->childCount(); i++) {
       ObjectState* child = sFieldState->child(i);
       if (child->name() == SocketEncoding::OSNAME_SOCKET_ENCODING) {
           ObjectState* socket = new ObjectState(SOCKET);
           srcOrDst->addChild(socket);
           socket->setAttribute(
               SOCKET_NAME,
               child->stringAttribute(SocketEncoding::OSKEY_SOCKET_NAME));
           ObjectState* map = new ObjectState(SOCKET_MAP);
           socket->addChild(map);
           map->setAttribute(
               SOCKET_MAP_EXTRA_BITS,
               child->stringAttribute(SocketEncoding::OSKEY_EXTRA_BITS));
           if (child->hasAttribute(SocketEncoding::OSKEY_SC_TABLE)) {
               map->setAttribute(
                   SOCKET_MAP_CODES,
                   child->stringAttribute(SocketEncoding::OSKEY_SC_TABLE));
           }
           map->setValue(
               child->stringAttribute(SocketEncoding::OSKEY_ENCODING));
       }
   }
   
   // add no-operation element
   if (sFieldState->hasChild(NOPEncoding::OSNAME_NOP_ENCODING)) {
       ObjectState* nopEncoding = sFieldState->childByName(
           NOPEncoding::OSNAME_NOP_ENCODING);
       ObjectState* noOperation = new ObjectState(SRC_DST_NO_OPERATION);
       srcOrDst->addChild(noOperation);
       ObjectState* map = new ObjectState(SRC_DST_NO_OPERATION_MAP);
       noOperation->addChild(map);
       map->setAttribute(
           SRC_DST_NO_OPERATION_MAP_EXTRA_BITS,
           nopEncoding->stringAttribute(NOPEncoding::OSKEY_EXTRA_BITS));
       map->setValue(
           nopEncoding->stringAttribute(NOPEncoding::OSKEY_ENCODING));
   }
   
   return srcOrDst;
}


/**
 * Converts the given ObjectState instance that represents an 
 * ImmediateSlotField instance to BEM file format.
 *
 * @param slotState The ObjectState instance that represents an
 *                  ImmediateSlotField instance.
 * @return The newly created ObjectState tree.
 */
ObjectState*
BEMSerializer::immediateSlotToFile(const ObjectState* slotState) {
    ObjectState* immSlot = new ObjectState(IMMEDIATE_SLOT);
    immSlot->setAttribute(
        IMMEDIATE_SLOT_NAME,
        slotState->stringAttribute(ImmediateSlotField::OSKEY_NAME));
    ObjectState* pos = new ObjectState(IMMEDIATE_SLOT_POS);
    immSlot->addChild(pos);
    pos->setValue(
        slotState->stringAttribute(ImmediateSlotField::OSKEY_POSITION));
    ObjectState* width = new ObjectState(IMMEDIATE_SLOT_WIDTH);
    immSlot->addChild(width);
    width->setValue(
        slotState->stringAttribute(ImmediateSlotField::OSKEY_WIDTH));
    return immSlot;
}


/**
 * Converts the given ObjectState tree that represent a long immediate
 * destination register field in BEM object model to BEM file format.
 *
 * @param omState The ObjectState tree.
 * @return The newly create ObjectState tree.
 */
ObjectState*
BEMSerializer::longImmDstRegisterFieldToFile(const ObjectState* omState) {

    ObjectState* fileState = new ObjectState(IMM_REG_FIELD);
    ObjectState* posElem = new ObjectState(IMM_REG_FIELD_POS);
    fileState->addChild(posElem);
    posElem->setValue(
        omState->stringAttribute(LImmDstRegisterField::OSKEY_POSITION));
    ObjectState* widthElem = new ObjectState(IMM_REG_FIELD_WIDTH);
    fileState->addChild(widthElem);
    widthElem->setValue(
        omState->stringAttribute(LImmDstRegisterField::OSKEY_WIDTH));

    for (int i = 0; i < omState->childCount(); i++) {
        ObjectState* child = omState->child(i);
        assert(child->name() == LImmDstRegisterField::OSNAME_IU_DESTINATION);
        ObjectState* iTempElem = new ObjectState(IMM_REG_FIELD_ITEMP);
        fileState->addChild(iTempElem);
        iTempElem->setAttribute(
            IMM_REG_FIELD_ITEMP_NAME, 
            child->stringAttribute(LImmDstRegisterField::OSKEY_ITEMP));
        ObjectState* regIndexElem = new ObjectState(
            IMM_REG_FIELD_ITEMP_REG_INDEX);
        iTempElem->addChild(regIndexElem);
        regIndexElem->setValue(
            child->stringAttribute(LImmDstRegisterField::OSKEY_DST_IU));
    }

    return fileState;
}

/**
 * Converts the given ObjectState tree that represents a socket code table
 * in BEM file to an ObjectState tree that can be loaded by SocketCodeTable.
 *
 * @param scTable The ObjectState tree.
 * @return The newly created ObjectState tree that can be loaded by
 *         SocketCodeTable.
 */
ObjectState*
BEMSerializer::socketCodeTableToOM(const ObjectState* scTable) {

    ObjectState* omTable = new ObjectState(
        SocketCodeTable::OSNAME_SOCKET_CODE_TABLE);
    string name = scTable->stringAttribute(SC_TABLE_NAME);
    omTable->setAttribute(SocketCodeTable::OSKEY_NAME, name);
    int extraBits = scTable->childByName(SC_TABLE_EXTRA_BITS)->intValue();
    omTable->setAttribute(
        SocketCodeTable::OSKEY_EXTRA_BITS, extraBits);

    // add port codes
    for (int i = 0; i < scTable->childCount(); i++) {
        ObjectState* child = scTable->child(i);
        if (child->name() == FU_PORT_CODE) {
            ObjectState* fuPortCode = new ObjectState(
                FUPortCode::OSNAME_FU_PORT_CODE);
            omTable->addChild(fuPortCode);
            string portName = child->stringAttribute(FU_PORT_CODE_PORT);
            string fuName = child->stringAttribute(FU_PORT_CODE_FU);
            fuPortCode->setAttribute(FUPortCode::OSKEY_UNIT_NAME, fuName);
            fuPortCode->setAttribute(FUPortCode::OSKEY_PORT_NAME, portName);
            if (child->hasAttribute(FU_PORT_CODE_OPERATION)) {
                string opName = child->stringAttribute(
                    FU_PORT_CODE_OPERATION);
                fuPortCode->setAttribute(
                    FUPortCode::OSKEY_OPERATION_NAME, opName);
            }

            fuPortCode->setAttribute(
                FUPortCode::OSKEY_ENCODING,
                child->childByName(FU_PORT_CODE_ENCODING)->stringValue());
            fuPortCode->setAttribute(
                FUPortCode::OSKEY_EXTRA_BITS,
                child->childByName(FU_PORT_CODE_EXTRA_BITS)->stringValue());
        } else if (child->name() == RF_PORT_CODE) {
            ObjectState* rfPortCode = new ObjectState(
                RFPortCode::OSNAME_RF_PORT_CODE);
            omTable->addChild(rfPortCode);
            readRegisterFilePortCode(child, rfPortCode);
        } else if (child->name() == IU_PORT_CODE) {
            ObjectState* iuPortCode = new ObjectState(
                IUPortCode::OSNAME_IU_PORT_CODE);
            omTable->addChild(iuPortCode);
            readRegisterFilePortCode(child, iuPortCode);
        }
    }

    return omTable;
}


/**
 * Converts the given ObjectState tree that represents a long-immediate-tag
 * element of BEM file to ObjectState tree that can be loaded by
 * ImmediateControlField.
 *
 * @param fileTag An ObjectState tree that represents a long-immediate-tag
 *                element.
 * @return The newly created ObjectState tree.
 */
ObjectState*
BEMSerializer::longImmTagToOM(const ObjectState* immTag) {

    ObjectState* immControlField = new ObjectState(
        ImmediateControlField::OSNAME_IMM_CONTROL_FIELD);
    ObjectState* pos = immTag->childByName(LIMM_TAG_POSITION);
    immControlField->setAttribute(
        ImmediateControlField::OSKEY_POSITION, pos->stringValue());
    ObjectState* extraBits = immTag->childByName(LIMM_TAG_EXTRA_BITS);
    immControlField->setAttribute(
        ImmediateControlField::OSKEY_EXTRA_BITS, extraBits->stringValue());

    // add template mappings
    for (int i = 0; i < immTag->childCount(); i++) {
        ObjectState* child = immTag->child(i);
        if (child->name() == LIMM_TAG_MAP) {
            ObjectState* mapping = new ObjectState(
                ImmediateControlField::OSNAME_TEMPLATE_MAP);
            immControlField->addChild(mapping);
            mapping->setAttribute(
                ImmediateControlField::OSKEY_TEMPLATE_NAME,
                child->stringAttribute(LIMM_TAG_MAP_NAME));
            mapping->setAttribute(
                ImmediateControlField::OSKEY_ENCODING, child->stringValue());
        }
    }

    return immControlField;
}


/**
 * Converts the given ObjectState tree that represents a slot element of
 * BEM file to ObjectState tree that can be loaded by MoveSlot.
 *
 * @param fileSlot The ObjectState tree that represents a slot element.
 * @return The newly created ObjectState tree.
 */
ObjectState*
BEMSerializer::moveSlotToOM(const ObjectState* fileSlot) {

    ObjectState* moveSlot = new ObjectState(MoveSlot::OSNAME_MOVE_SLOT);
    string busName = fileSlot->stringAttribute(SLOT_NAME);
    moveSlot->setAttribute(MoveSlot::OSKEY_BUS_NAME, busName);

    ObjectState* pos = fileSlot->childByName(SLOT_POSITION);
    moveSlot->setAttribute(MoveSlot::OSKEY_POSITION, pos->stringValue());

    ObjectState* extraBits = fileSlot->childByName(SLOT_EXTRA_BITS);
    moveSlot->setAttribute(
        MoveSlot::OSKEY_EXTRA_BITS, extraBits->stringValue());

    // add guard field
    if (fileSlot->hasChild(SLOT_GUARD)) {
        ObjectState* guard = fileSlot->childByName(SLOT_GUARD);
        moveSlot->addChild(guardFieldToOM(guard));
    }

    // add source field
    if (fileSlot->hasChild(SLOT_SOURCE)) {
        ObjectState* source = fileSlot->childByName(SLOT_SOURCE);
        moveSlot->addChild(sourceFieldToOM(source));
    }

    // add destination field
    if (fileSlot->hasChild(SLOT_DESTINATION)) {
        ObjectState* destination = fileSlot->childByName(SLOT_DESTINATION);
        moveSlot->addChild(destinationFieldToOM(destination));
    }

    return moveSlot;
}


/**
 * Converts the given ObjectState tree that represents a guard element of
 * BEM file to a new ObjectState tree that can be loaded by GuardField.
 *
 * @param fileGuard An ObjectState tree that represents a guard element of
 *                  BEM file.
 * @return The newly created ObjectState tree.
 */
ObjectState*
BEMSerializer::guardFieldToOM(const ObjectState* fileGuard) {

    ObjectState* guardField = new ObjectState(
        GuardField::OSNAME_GUARD_FIELD);
    ObjectState* pos = fileGuard->childByName(SLOT_GUARD_POSITION);
    guardField->setAttribute(GuardField::OSKEY_POSITION, pos->stringValue());
    ObjectState* extraBits = fileGuard->childByName(SLOT_GUARD_EXTRA_BITS);
    guardField->setAttribute(
        GuardField::OSKEY_EXTRA_BITS, extraBits->stringValue());

    // add guard encodings
    for (int i = 0; i < fileGuard->childCount(); i++) {
        ObjectState* child = fileGuard->child(i);
        if (child->name() == REG_GUARD_CODE) {
            ObjectState* guardCode = new ObjectState(
                GPRGuardEncoding::OSNAME_GPR_GUARD_ENCODING);
            guardField->addChild(guardCode);
            string regFile = child->stringAttribute(REG_GUARD_CODE_RF);
            guardCode->setAttribute(
                GPRGuardEncoding::OSKEY_RF_NAME, regFile);
            int regIndex = child->intAttribute(REG_GUARD_CODE_INDEX);
            guardCode->setAttribute(
                GPRGuardEncoding::OSKEY_REGISTER_INDEX, regIndex);
            string inverted = child->stringAttribute(
                REG_GUARD_CODE_INVERTED);
            if (inverted == TRUE) {
                guardCode->setAttribute(
                    GPRGuardEncoding::OSKEY_INVERTED, true);
            } else if (inverted == FALSE) {
                guardCode->setAttribute(
                    GPRGuardEncoding::OSKEY_INVERTED, false);
            } else {
                assert(false);
            }
            guardCode->setAttribute(
                GPRGuardEncoding::OSKEY_ENCODING, child->stringValue());

        } else if (child->name() == PORT_GUARD_CODE) {
            ObjectState* guardCode = new ObjectState(
                FUGuardEncoding::OSNAME_FU_GUARD_ENCODING);
            guardField->addChild(guardCode);
            string fu = child->stringAttribute(PORT_GUARD_CODE_FU);
            guardCode->setAttribute(FUGuardEncoding::OSKEY_FU_NAME, fu);
            string port = child->stringAttribute(PORT_GUARD_CODE_PORT);
            guardCode->setAttribute(FUGuardEncoding::OSKEY_PORT_NAME, port);
            string inverted = child->stringAttribute(
                PORT_GUARD_CODE_INVERTED);
            if (inverted == TRUE) {
                guardCode->setAttribute(
                    FUGuardEncoding::OSKEY_INVERTED, true);
            } else if (inverted == FALSE) {
                guardCode->setAttribute(
                    FUGuardEncoding::OSKEY_INVERTED, false);
            } else {
                assert(false);
            }
            guardCode->setAttribute(
                FUGuardEncoding::OSKEY_ENCODING, child->stringValue());

        } else if (child->name() == ALWAYS_TRUE_GUARD_CODE || 
                   child->name() == ALWAYS_FALSE_GUARD_CODE) {
            ObjectState* guardCode = new ObjectState(
                UnconditionalGuardEncoding::
                OSNAME_UNCONDITIONAL_GUARD_ENCODING);
            guardField->addChild(guardCode);
            guardCode->setAttribute(
                UnconditionalGuardEncoding::OSKEY_ENCODING, 
                child->stringValue());
            if (child->name() == ALWAYS_TRUE_GUARD_CODE) {
                guardCode->setAttribute(
                    UnconditionalGuardEncoding::OSKEY_INVERTED, false);
            } else {
                guardCode->setAttribute(
                    UnconditionalGuardEncoding::OSKEY_INVERTED, true);
            }
        }
    }

    return guardField;
}


/**
 * Converts the given ObjectState tree that represents a source element in
 * BEM file to an ObjectState tree that can be loaded by SourceField.
 *
 * @param fileSource An ObjectState tree that represents a source element
 *                   in a BEM file.
 * @return The newly created ObjectState tree.
 */
ObjectState*
BEMSerializer::sourceFieldToOM(const ObjectState* fileSource) {

    ObjectState* sourceField = slotFieldToOM(fileSource);
    sourceField->setName(SourceField::OSNAME_SOURCE_FIELD);

    // add immediate encoding
    if (fileSource->hasChild(SOURCE_IMMEDIATE)) {
        ObjectState* immEncoding = new ObjectState(
            ImmediateEncoding::OSNAME_IMM_ENCODING);
        sourceField->addChild(immEncoding);
        immEncoding->setAttribute(
            ImmediateEncoding::OSKEY_IMM_WIDTH,
            fileSource->childByName(SOURCE_IMMEDIATE)->stringAttribute(
                SOURCE_IMMEDIATE_WIDTH));
        ObjectState* map = fileSource->childByName(SOURCE_IMMEDIATE)->
            child(0);
        immEncoding->setAttribute(
            ImmediateEncoding::OSKEY_ENCODING, map->stringValue());
        immEncoding->setAttribute(
            ImmediateEncoding::OSKEY_EXTRA_BITS,
            map->stringAttribute(SOURCE_IMMEDIATE_MAP_EXTRA_BITS));
    }

    // add bridge encodings
    for (int i = 0; i < fileSource->childCount(); i++) {
        ObjectState* child = fileSource->child(i);
        if (child->name() == SOURCE_BRIDGE) {
            ObjectState* bridgeEncoding = new ObjectState(
                BridgeEncoding::OSNAME_BRIDGE_ENCODING);
            sourceField->addChild(bridgeEncoding);
            bridgeEncoding->setAttribute(
                BridgeEncoding::OSKEY_BRIDGE_NAME,
                child->stringAttribute(SOURCE_BRIDGE_NAME));
            ObjectState* map = child->childByName(SOURCE_BRIDGE_MAP);
            bridgeEncoding->setAttribute(
                BridgeEncoding::OSKEY_EXTRA_BITS,
                map->stringAttribute(SOURCE_BRIDGE_MAP_EXTRA_BITS));
            bridgeEncoding->setAttribute(
                BridgeEncoding::OSKEY_ENCODING, map->stringValue());
        }
    }

    return sourceField;
}


/**
 * Converts the given ObjectState tree that represents a destination element
 * in BEM file to an ObjectState tree that can be loaded by DestinationField.
 *
 * @param fileDestination An ObjectState tree that represents a destination
 *                        element in a BEM file.
 * @return The newly created ObjectState tree.
 */
ObjectState*
BEMSerializer::destinationFieldToOM(const ObjectState* fileDestination) {
    ObjectState* destinationField = slotFieldToOM(fileDestination);
    destinationField->setName(DestinationField::OSNAME_DESTINATION_FIELD);
    return destinationField;
}


/**
 * Converts the given ObjectState tree that represents either source or
 * destination element of BEM file to an ObjectState tree that can be loaded
 * by SlotField.
 *
 * @param fileSlot An ObjectState tree representing source or destination
 *                 field.
 * @return The newly created ObjectState tree.
 */
ObjectState*
BEMSerializer::slotFieldToOM(const ObjectState* fileSlot) {

    ObjectState* slotField = new ObjectState(SlotField::OSNAME_SLOT_FIELD);
    ObjectState* pos = fileSlot->childByName(SRC_DST_POSITION);
    slotField->setAttribute(
        SlotField::OSKEY_POSITION, pos->stringValue());
    ObjectState* extraBits = fileSlot->childByName(SRC_DST_EXTRA_BITS);
    slotField->setAttribute(
        SlotField::OSKEY_EXTRA_BITS, extraBits->stringValue());
    if (fileSlot->childByName(SRC_DST_ID_POS)->stringValue() == 
        ID_POS_RIGHT) {
        slotField->setAttribute(
            SlotField::OSKEY_COMPONENT_ID_POSITION, BinaryEncoding::RIGHT);
    } else {
        assert(
            fileSlot->childByName(SRC_DST_ID_POS)->stringValue() ==
            ID_POS_LEFT);
        slotField->setAttribute(
            SlotField::OSKEY_COMPONENT_ID_POSITION, BinaryEncoding::LEFT);
    }

    // read socket elements
    for (int i = 0; i < fileSlot->childCount(); i++) {
        ObjectState* child = fileSlot->child(i);
        if (child->name() == SOCKET) {
            ObjectState* socketEncoding = new ObjectState(
                SocketEncoding::OSNAME_SOCKET_ENCODING);
            slotField->addChild(socketEncoding);
            socketEncoding->setAttribute(
                SocketEncoding::OSKEY_SOCKET_NAME,
                child->stringAttribute(SOCKET_NAME));
            ObjectState* map = child->childByName(SOCKET_MAP);
            socketEncoding->setAttribute(
                SocketEncoding::OSKEY_ENCODING, map->stringValue());
            socketEncoding->setAttribute(
                SocketEncoding::OSKEY_EXTRA_BITS,
                map->stringAttribute(SOCKET_MAP_EXTRA_BITS));
            if (map->hasAttribute(SOCKET_MAP_CODES)) {
                socketEncoding->setAttribute(
                    SocketEncoding::OSKEY_SC_TABLE,
                    map->stringAttribute(SOCKET_MAP_CODES));
            }
        }
    }

    // read NOP encoding
    if (fileSlot->hasChild(SRC_DST_NO_OPERATION)) {
        ObjectState* nopEncoding = new ObjectState(
            NOPEncoding::OSNAME_NOP_ENCODING);
        slotField->addChild(nopEncoding);
        ObjectState* map = fileSlot->childByName(SRC_DST_NO_OPERATION)->
            child(0);
        nopEncoding->setAttribute(
            NOPEncoding::OSKEY_ENCODING, map->stringValue());
        nopEncoding->setAttribute(
            NOPEncoding::OSKEY_EXTRA_BITS,
            map->stringAttribute(SRC_DST_NO_OPERATION_MAP_EXTRA_BITS));
    }

    return slotField;
}


/**
 * Converts the given ObjectState tree that represents immediate-slot element
 * of BEM file to an ObjectState instance that can be loaded by 
 * ImmediateSlotField instance.
 *
 * @param fileSlot ObjectState tree that represents immediate-slot element
 *                 of BEM file.
 * @return The newly created ObjectState instance.
 */
ObjectState*
BEMSerializer::immediateSlotToOM(const ObjectState* fileSlot) {
    ObjectState* immSlotField = new ObjectState(
        ImmediateSlotField::OSNAME_IMMEDIATE_SLOT_FIELD);
    string name = fileSlot->stringAttribute(IMMEDIATE_SLOT_NAME);
    immSlotField->setAttribute(ImmediateSlotField::OSKEY_NAME, name);
    string width = fileSlot->childByName(IMMEDIATE_SLOT_WIDTH)->
        stringValue();
    immSlotField->setAttribute(ImmediateSlotField::OSKEY_WIDTH, width);
    string pos = fileSlot->childByName(IMMEDIATE_SLOT_POS)->stringValue();
    immSlotField->setAttribute(ImmediateSlotField::OSKEY_POSITION, pos);
    immSlotField->setAttribute(ImmediateSlotField::OSKEY_EXTRA_BITS, 0);
    return immSlotField;
}


/**
 * Converts the given ObjectState tree that represents a long immediate
 * destination register field in BEM file to an ObjectState tree that can
 * be loaded by LImmDstRegisterField instance.
 *
 * @param fileField The ObjectState tree that represents long immediate
 *                  destination register field in BEM file.
 * @return The newly created ObjectState tree.
 */
ObjectState*
BEMSerializer::longImmDstRegFieldToOM(const ObjectState* fileField) {

    ObjectState* omState = new ObjectState(
        LImmDstRegisterField::OSNAME_LIMM_DST_REGISTER_FIELD);
    omState->setAttribute(LImmDstRegisterField::OSKEY_EXTRA_BITS, 0);
    omState->setAttribute(
        LImmDstRegisterField::OSKEY_POSITION,
        fileField->childByName(IMM_REG_FIELD_POS)->stringValue());
    omState->setAttribute(
        LImmDstRegisterField::OSKEY_WIDTH, 
        fileField->childByName(IMM_REG_FIELD_WIDTH)->stringValue());

    for (int i = 0; i < fileField->childCount(); i++) {
        ObjectState* child = fileField->child(i);
        if (child->name() == IMM_REG_FIELD_ITEMP) {
            ObjectState* iuDst = new ObjectState(
                LImmDstRegisterField::OSNAME_IU_DESTINATION);
            omState->addChild(iuDst);
            iuDst->setAttribute(
                LImmDstRegisterField::OSKEY_ITEMP,
                child->stringAttribute(IMM_REG_FIELD_ITEMP_NAME));
            iuDst->setAttribute(
                LImmDstRegisterField::OSKEY_DST_IU,
                child->child(0)->stringValue());
        }
    }

    return omState;
}


/**
 * Reads rf-port-code or iu-port-code element from an ObjectState tree that
 * represents BEM file and adds the data to the given ObjectState instance
 * to make it loadable by RFPortCode or IUPortCode.
 *
 * @param filePortCode ObjectState tree representing rf-port-code or
 *                     iu-port-code element of BEM file.
 * @param omPortCode The ObjectState instance to which the data is added.
 */
void
BEMSerializer::readRegisterFilePortCode(
    const ObjectState* filePortCode,
    ObjectState* omPortCode) {

    string unit;
    if (filePortCode->name() == RF_PORT_CODE) {
        unit = filePortCode->stringAttribute(RF_PORT_CODE_RF);
    } else if (filePortCode->name() == IU_PORT_CODE) {
        unit = filePortCode->stringAttribute(IU_PORT_CODE_IU);
    } else {
        assert(false);
    }

    omPortCode->setAttribute(PortCode::OSKEY_UNIT_NAME, unit);
    omPortCode->setAttribute(
        PortCode::OSKEY_INDEX_WIDTH,
        filePortCode->stringAttribute(RF_PORT_CODE_INDEX_WIDTH));

    if (filePortCode->hasChild(RF_PORT_CODE_ENCODING)) {
        ObjectState* encoding = filePortCode->childByName(
            RF_PORT_CODE_ENCODING);
        omPortCode->setAttribute(
            PortCode::OSKEY_ENCODING, encoding->stringValue());
        ObjectState* extraBits = filePortCode->childByName(
                RF_PORT_CODE_EXTRA_BITS);
        omPortCode->setAttribute(
            PortCode::OSKEY_EXTRA_BITS, extraBits->stringValue());
    }

}
