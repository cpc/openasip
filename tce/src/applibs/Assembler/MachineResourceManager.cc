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
 * @file MachineResourceManager.cc
 *
 * Implementation of MachineResourceManager class.
 *
 * @author Mikael Lepistö 2005 (tmlepist-no.spam-cs.tut.fi)
 * @note rating: yellow
 */

#include "MachineResourceManager.hh"

#include "Binary.hh"
#include "ASpaceSection.hh"
#include "AddressSpace.hh"
#include "ResourceSection.hh"
#include "NullSection.hh"
#include "MoveElement.hh"

#include "ControlUnit.hh"
#include "SpecialRegisterPort.hh"
#include "FUPort.hh"
#include "HWOperation.hh"
#include "Guard.hh"
#include "Assembler.hh"
#include "Exception.hh"

#include "Bus.hh"

using namespace TPEF;
using namespace TTAMachine;


/**
 * Constructor.
 *
 * @param tpef Binary where to generated resources are added.
 * @param adf Machine where from requested resources are found.
 * @param parent Assembler root class for adding warning information.
 */
MachineResourceManager::MachineResourceManager(
    Binary& tpef,
    Machine& adf,
    Assembler* parent) :
    tpef_(tpef), adf_(adf), aSpaceSection_(NULL), strings_(NULL),
    undefASpace_(NULL), codeASpace_(NULL), nullSection_(NULL),
    resourceSection_(NULL), lastFunctionUnitID_(0), lastRegisterFileID_(0),
    lastOpOrSpecRegisterID_(0), parent_(parent) {
}


/**
 * Finds data address space by name from ADF and adds it to TPEF.
 *
 * @param name Name of requested address space.
 * @return ASpaceElement or NULL if not found.
 * @exception IllegalMachine Address space was not found from ADF.
 * @exception InvalidData Found address space is the code address space.
 */
ASpaceElement*
MachineResourceManager::findDataAddressSpace(std::string name)
    throw (IllegalMachine, InvalidData) {

    // check if address space is already created
    if (!MapTools::containsKey(addressSpaces_, name)) {

        // try to find address space for name
        Machine::AddressSpaceNavigator aSpaces = adf_.addressSpaceNavigator();

        // Check that address space is found from machine and create address
        // space section and address undef address space if needed to TPEF.
        if (!aSpaces.hasItem(name)) {
            throw IllegalMachine(
                __FILE__, __LINE__, __func__,
                "ADF doesn't contain address space: " + name);
        }

        undefinedAddressSpace();

        // create new address space and add it to TPEF
        AddressSpace* adfAddressSpace = aSpaces.item(name);
        assert(adfAddressSpace != NULL);

        ASpaceElement* newASpace = new ASpaceElement();
        newASpace->setMAU(adfAddressSpace->width());
        newASpace->setName(stringToChunk(name));
        addressSpaces_[name] = newASpace;

        aSpaceSection_->addElement(newASpace);
    }

    if (addressSpaces_[name] == codeAddressSpace()) {
        throw InvalidData(
            __FILE__, __LINE__, __func__,
            "Method can't be used for getting code address space.");
    }

    return addressSpaces_[name];
}


/**
 * Returns bitwidth of requested bus.
 *
 * @param Slot number of requested bus.
 * @return Bitwidth of requested bus.
 * @exception OutOfRange If slot number is out of bounds.
 */
UValue
MachineResourceManager::findBusWidth(UValue slotNumber)
    throw (OutOfRange) {

    Bus* bus = adf_.busNavigator().item(slotNumber);
    return bus->width();
}


/**
 * Returns NullSection instance of TPEF.
 *
 * @return NullSection instance of TPEF.
 */
Section*
MachineResourceManager::nullSection() {

    if (nullSection_ == NULL) {

        nullSection_ = dynamic_cast<NullSection*>(
            Section::createSection(Section::ST_NULL));

        assert(nullSection_ != NULL);

        nullSection_->setASpace(undefinedAddressSpace());
        nullSection_->setName(stringToChunk(""));
        nullSection_->setLink(nullSection_);

        tpef_.addSection(nullSection_);
    }

    return nullSection_;
}


/**
 * Returns string table of TPEF.
 *
 * @return String table of TPEF.
 */
StringSection*
MachineResourceManager::stringSection() {
    // makes sure that strings section is initialized
    undefinedAddressSpace();
    return strings_;
}


/**
 * Returns undefined address space element.
 *
 * @return undefined address space element.
 */
ASpaceElement*
MachineResourceManager::undefinedAddressSpace() {

    // create address space section when needed
    if (aSpaceSection_ == NULL) {

        // create address space section with undefined address space element
        aSpaceSection_ =
            dynamic_cast<ASpaceSection*>(
                Section::createSection(Section::ST_ADDRSP));

        assert(aSpaceSection_ != NULL);

        undefASpace_ = new ASpaceElement();
        undefASpace_->setName(stringToChunk(""));

        aSpaceSection_->setLink(strings_);
        aSpaceSection_->setName(stringToChunk(""));
        aSpaceSection_->setASpace(undefinedAddressSpace());
        aSpaceSection_->setUndefinedASpace(undefASpace_);

        aSpaceSection_->addElement(undefASpace_);
        tpef_.addSection(aSpaceSection_);
    }

    return undefASpace_;
}


/**
 * Returns the code address space.
 *
 * @return The code address space.
 */
ASpaceElement*
MachineResourceManager::codeAddressSpace() {

    if (codeASpace_ == NULL) {
        // make sure that there is address space section created...
        undefinedAddressSpace();

        // find codeAddressSpace, create tpef version and add it to section
        AddressSpace* adfAddressSpace = adf_.controlUnit()->addressSpace();

        if (adfAddressSpace == NULL) {
            throw IllegalMachine(
                __FILE__, __LINE__, __func__,
                "Control unit has no address space.");
        }

        ASpaceElement* newASpace = new ASpaceElement();
        newASpace->setMAU(0);
        newASpace->setName(stringToChunk(adfAddressSpace->name()));

        addressSpaces_[adfAddressSpace->name()] = newASpace;
        codeASpace_ = newASpace;

        aSpaceSection_->addElement(newASpace);
    }

    return codeASpace_;
}

/**
 * Finds or adds string to TPEF string section and returns
 * chunk pointing to it.
 *
 * @param aStr String whose chunk is needed.
 * @return Chunk pointing to string stored in TPEF.
 */
Chunk*
MachineResourceManager::stringToChunk(const std::string aStr) {

    if (strings_ == NULL) {
        strings_ = dynamic_cast<StringSection*>(
            Section::createSection(Section::ST_STRTAB));

        // first byte must be zero
        strings_->addByte(0);

        tpef_.setStrings(strings_);
        tpef_.addSection(strings_);

        // start zero of string section must be added befor this, since
        // undefined address space calls this function to get chunk(0)
        strings_->setASpace(undefinedAddressSpace());
        strings_->setLink(nullSection());
    }

    return strings_->string2Chunk(aStr);
}


/**
 * Resolves type, unit and index fields, of given register term.
 *
 * Also checks if requested term is possible and creates machine
 * resource section.
 *
 * @param currentLine line number where term is located in assembly code.
 * @param term Term to resolve.
 * @param slotNumber Number of move slot, that was used.
 * @param type Is read, write or guard.
 * @return Information of Term's type, unit and index for instruction element.
 * @exception IllegalMachine There is problem to find or access the resource.
 */
MachineResourceManager::ResourceID&
MachineResourceManager::resourceID(
    UValue currentLine,
    const RegisterTerm& term,
    UValue slotNumber,
    RequestType type)
    throw (IllegalMachine) {

    // create resource key for reference
    ResourceID newResID;

    ResourceKey resKey;
    resKey.slotNumber = slotNumber;
    resKey.type = type;

#if 0
    // Valuable debug info (currently resolved term)
    term.clear();
    std::cerr << "\ttype: " << term.type << std::endl;
#endif

    // create key for requested resource
    if (term.type == RegisterTerm::INDEX) {
        resKey.keyString = term.indexTerm.part1;

        if (term.indexTerm.part2Used) {
            resKey.keyString += "." + term.indexTerm.part2;
        }

        resKey.keyString += "." + Conversion::toString(term.indexTerm.index);

    } else if (term.type == RegisterTerm::FUNCTION_UNIT) {
        resKey.keyString =
            term.fuTerm.part1 + "." + term.fuTerm.part2;

        if (term.fuTerm.part3Used) {
            resKey.keyString += "." + term.fuTerm.part3;
        }

    } else if (term.type == RegisterTerm::BUS) {
        if (term.busTerm.prev) {
            resKey.keyString = "{prev}";
        } else {
            resKey.keyString = "{next}";
        }

    } else {
        abortWithError(
            "Unknown term type." + Conversion::toString(term.type));
    }

    // check if resource is used before or do we have to
    // retrieve it and add it to resource section
    if (!MapTools::containsKey(resourceMap_, resKey)) {

        if (term.type == RegisterTerm::INDEX) {
            // check if rf[.port].index  or fu.operation.index
            newResID = indexResource(
                currentLine, term, slotNumber, type, resKey.keyString);

        } else if (term.type == RegisterTerm::FUNCTION_UNIT) {
            // this is definately unit.port[.operation]
            newResID = functionUnitPortResource(term);

        } else if (term.type == RegisterTerm::BUS) {
            abortWithError("{prev} and {next} might never be supported.");

        } else {
            assert(false);
        }

#if 0
        // Very useful debugging information
        std::cerr << "resKey: " << resKey.keyString
                  << "\ttype: " << resKey.type
                  << "\tslot:" << resKey.slotNumber
                  << std::endl;
#endif
        // copy to map
        resourceMap_[resKey] = newResID;
    }

    // get resource id from map and give it away
    assert(MapTools::containsKey(resourceMap_, resKey));

    return resourceMap_[resKey];
}


/**
 * Returns the resource section of TPEF.
 *
 * @return The resource section of TPEF.
 */
ResourceSection* MachineResourceManager::resourceSection() {
    initResourceSection();
    return resourceSection_;
}


/**
 * Initialises resource section if its not initialised already.
 */
void MachineResourceManager::initResourceSection() {
    // init resource section if needed and add new resource element.
    if (resourceSection_ == NULL) {
        resourceSection_ = dynamic_cast<ResourceSection*>(
            Section::createSection(Section::ST_MR));

        assert(resourceSection_ != NULL);

        resourceSection_->setName(stringToChunk(""));
        resourceSection_->setASpace(undefinedAddressSpace());
        resourceSection_->setLink(strings_);

        // add all busses at once
        Machine::BusNavigator busNavi = adf_.busNavigator();

        for (int i = 0; i < busNavi.count(); i++) {
            Bus* bus = busNavi.item(i);

            ResourceElement* newRes = new ResourceElement();

            newRes->setId(i + 1);
            newRes->setType(ResourceElement::MRT_BUS);
            newRes->setName(stringToChunk(bus->name()));

            resourceSection_->addElement(newRes);
        }

        tpef_.addSection(resourceSection_);
    }
}


/**
 * Adds resource element to TPEF.
 */
void
MachineResourceManager::addResourceElement(ResourceElement* resource) {
    initResourceSection();
    assert(resourceSection_ != NULL);

#if 0
    // Valuable debug info... about resources that are added to 
    // resource section
    std::cerr << "Added resource element: " 
              << strings_->chunk2String(resource->name())
              << "\t" << (int)resource->type()
              << "\t" << (int)resource->id()
              << std::endl;
#endif

    resourceSection_->addElement(resource);
}


/**
 * Returns TPEF resource ID of function unit.
 *
 * If resource is not already in TPEF method creates
 * resource and adds it.
 *
 * @param unit Function unit whose ID is needed.
 * @return TPEF resource ID of function unit.
 */
UValue
MachineResourceManager::functionUnitID(FunctionUnit* unit) {

    if (!MapTools::containsKey(functionUnitIDs_, unit)) {
        // create resource element and add its unit id to map
        lastFunctionUnitID_++;

        ResourceElement* newRes = new ResourceElement();

        newRes->setId(lastFunctionUnitID_);
        newRes->setType(ResourceElement::MRT_UNIT);
        newRes->setName(stringToChunk(unit->name()));

        addResourceElement(newRes);

        functionUnitIDs_[unit] = lastFunctionUnitID_;
    }

    assert(MapTools::containsKey(functionUnitIDs_, unit));

    return functionUnitIDs_[unit];
}


/**
 * Returns TPEF resource ID corresponding to a register file.
 *
 * If there is no resource entry in TPEF for the given register file, this
 * method creates it, assigns a new unique ID code to it and adds it to the
 * pool of managed TPEF resources.
 *
 * @param rf A register file.
 * @return TPEF resource ID of the register file.
 */
UValue
MachineResourceManager::registerFileID(BaseRegisterFile* rf) {

    if (!MapTools::containsKey(registerFileIDs_, rf)) {
        // create resource element and add its unit id to map
        lastRegisterFileID_++;

        ResourceElement* newRes = new ResourceElement();

        newRes->setId(lastRegisterFileID_);

        if (dynamic_cast<ImmediateUnit*>(rf) == NULL) {
            newRes->setType(ResourceElement::MRT_RF);
        } else {
            newRes->setType(ResourceElement::MRT_IMM);
        }

        newRes->setName(stringToChunk(rf->name()));
        addResourceElement(newRes);

        registerFileIDs_[rf] = lastRegisterFileID_;
    }

    assert(MapTools::containsKey(registerFileIDs_, rf));

    return registerFileIDs_[rf];
}

/**
 * Returns TPEF resource ID of operand or port.
 *
 * If resource is not already in TPEF method creates
 * resource and adds it.
 *
 * @param opPortString TPEF resource string of port or operand.
 * @param port Port that is connected with requested resource in ADF.
 * @return TPEF resource ID of port or operand.
 */
UValue
MachineResourceManager::opOrPortID(
    std::string opOrPortString, const TTAMachine::Port* port) {

    if (!MapTools::containsKey(opOrPortIDs_, opOrPortString)) {
        // create resource element and add its unit id to map
        lastOpOrSpecRegisterID_++;
        UValue generatedID = lastOpOrSpecRegisterID_;

        ResourceElement* newRes = new ResourceElement();

        // check if string is port or operand by string format
        if (opOrPortString.find('.') == std::string::npos) {

            if (dynamic_cast<const SpecialRegisterPort*>(port) != NULL) {
                newRes->setType(ResourceElement::MRT_SR);
            } else {
                newRes->setType(ResourceElement::MRT_PORT);
            }

        } else {
            newRes->setType(ResourceElement::MRT_OP);
        }

        newRes->setName(stringToChunk(opOrPortString));
        newRes->setId(generatedID);
        addResourceElement(newRes);
        opOrPortIDs_[opOrPortString] = generatedID;
    }

    return opOrPortIDs_[opOrPortString];
}

/**
 * Returns debug string for request type for error message generation.
 *
 * @param type Request type.
 * @return Debug string for request type.
 */
std::string
MachineResourceManager::requestTypeString(RequestType type) const {
    switch (type) {
    case RQST_READ:      return "read";
    case RQST_WRITE:     return "write";
    case RQST_GUARD:     return "guard";
    case RQST_INVGUARD:  return "inverted guard";
    default:             return "invalid request type";
    }
}

/**
 * Finds resource for fu.port or fu.port.operation term.
 *
 * Helper method for resourceID method.
 *
 * @param term Requested term.
 * @return TPEF resource information of requested term.
 * @exception IllegalMachine There was problems finding resource from ADF.
 */
MachineResourceManager::ResourceID
MachineResourceManager::functionUnitPortResource(const RegisterTerm& term)
    throw (IllegalMachine) {

    ResourceID newResID;

    Machine::FunctionUnitNavigator fuNavi =
        adf_.functionUnitNavigator();

    FunctionUnit* fu = NULL;

    if (fuNavi.hasItem(term.fuTerm.part1)) {
        fu = fuNavi.item(term.fuTerm.part1);

    } else if (adf_.controlUnit()->name() == term.fuTerm.part1) {
        // NOTE: it would be easier, if control unit would be just
        // same that any fu and it would exist in fuNavigator.
        fu = adf_.controlUnit();

    } else {
        throw IllegalMachine(
            __FILE__, __LINE__, __func__,
            "Can't find function unit from machine: " +
            term.fuTerm.part1);
    }

    assert(fu != NULL);

    // find out port
    if (!fu->hasPort(term.fuTerm.part2)) {
        throw IllegalMachine(
            __FILE__, __LINE__, __func__,
            "Can't find port: " +
            term.fuTerm.part2);
    }

    Port* port = fu->port(term.fuTerm.part2);

    std::string regString;

    if (term.fuTerm.part3Used) {
        // find out operand
        if (!fu->hasOperation(term.fuTerm.part3)) {
            throw IllegalMachine(
                __FILE__, __LINE__, __func__,
                "Operation " + term.fuTerm.part3 +
                " is not found from fu: " +
                term.fuTerm.part1);
        }

        HWOperation* oper = fu->operation(term.fuTerm.part3);

        TTAMachine::FUPort* fuPort =
            dynamic_cast<TTAMachine::FUPort*>(port);

        try {
            regString =
                term.fuTerm.part3 + "." +
                Conversion::toString(oper->io(*fuPort));

        } catch (InstanceNotFound& e) {

            throw IllegalMachine(
                __FILE__, __LINE__, __func__,
                "Operation " + term.fuTerm.part3 +
                " is not bound to port: " +
                term.fuTerm.part2);
        }

    } else {
        // plain port reference
        regString = term.fuTerm.part2;
    }

    newResID.width = port->width();
    newResID.unit = functionUnitID(fu);
    newResID.index = opOrPortID(regString, port);
    newResID.type = MoveElement::MF_UNIT;

    return newResID;
}

/**
 * Finds resource for fu.operation.index, rf.index or rf.port.index term.
 *
 * Helper method for resourceID method.
 *
 * @param currentLine Line number information for warning messages.
 * @param term Requested term.
 * @param slotNumber Number of move slot, that was used.
 * @param type Request type.
 * @param resourceKeyString Resource key string for error messages.
 * @return TPEF resource information of requested term.
 * @exception IllegalMachine There was problems finding resource from ADF.
 */
MachineResourceManager::ResourceID
MachineResourceManager::indexResource(
    UValue currentLine, const RegisterTerm& term,
    UValue slotNumber, RequestType type, std::string& resourceKeyString)
    throw (IllegalMachine) {

    ResourceID newResID;

    Machine::RegisterFileNavigator rfNavi =
        adf_.registerFileNavigator();

    Machine::ImmediateUnitNavigator immNavi =
        adf_.immediateUnitNavigator();

    Machine::FunctionUnitNavigator fuNavi =
        adf_.functionUnitNavigator();

    BaseRegisterFile* rf = NULL;
    FunctionUnit* fu = NULL;

    if (immNavi.hasItem(term.indexTerm.part1)) {
        rf = immNavi.item(term.indexTerm.part1);
    }

    // If the a unit is found in both immediate unit and register file
    // navigators, the unit from the register file navigator is selected
    // and a warning is displayed.
    if (rfNavi.hasItem(term.indexTerm.part1)) {
        if (rf != NULL) {
            std::string warning = "Unit name '";
            warning += term.indexTerm.part1;
            warning += "' is ambiguous. Because of disambiguity rules, '";
            warning += term.indexTerm.part1;
            warning += "' is selected to refer the register file insread of";
            warning += " the immediate unit.";
            parent_->addWarning(currentLine, warning);
        }
        rf = rfNavi.item(term.indexTerm.part1);
    }

    // if normal fu or gcu...
    if (fuNavi.hasItem(term.indexTerm.part1)) {
        fu = fuNavi.item(term.indexTerm.part1);
    } else {
        // NOTE: GCU  isn't found by name with FunctionUnitNavigator,
        // but it must be requested explicitely
        fu = adf_.controlUnit();
        if (fu->name() != term.indexTerm.part1) {
            fu = NULL;
        }
    }

    if (term.indexTerm.part2Used) {
        if (rf != NULL && fu != NULL) {
            std::string warning = "Unit name '";
            warning += term.indexTerm.part1;
            warning += "' is ambiguous.";
            parent_->addWarning(currentLine, warning);
        }
        // rf.port.index or fu.operation.index reference
        newResID = rFPortOrFUIndexReference(
            fu, rf, currentLine, term, slotNumber, type, resourceKeyString);

    } else {
        // this must be rf.index reference
        newResID = registerFileIndexReference(
            rf, term, slotNumber, type, resourceKeyString);
    }

    return newResID;
}


/**
 * Finds resource for fu.operation.index or rf.port.index term.
 *
 * Helper method for indexResource method.
 *
 * @param fu Function unit of term, might be NULL.
 * @param rf Register file of term, might be NULL.
 * @param currentLine Line number information for warning messages.
 * @param term Requested term.
 * @param slotNumber Number of move slot, that was used.
 * @param type Request type.
 * @param resourceKeyString Resource key string for error messages.
 * @return TPEF resource information of requested term.
 * @exception IllegalMachine There was problems finding resource from ADF.
 */
MachineResourceManager::ResourceID
MachineResourceManager::rFPortOrFUIndexReference(
    FunctionUnit* fu,
    BaseRegisterFile* rf,
    UValue currentLine,
    const RegisterTerm& term,
    UValue slotNumber,
    RequestType type,
    std::string& resourceKeyString)
    throw (IllegalMachine) {

    ResourceID newResID;

    // NOTE: disambiguation rule says that rf.port.index has higher
    // priority than fu.operation.index ...
    if (rf != NULL) {

        // check defined if port can be used for current
        // direction and bus
        Bus* bus = NULL;
        try {
            bus = adf_.busNavigator().item(slotNumber);
        } catch (OutOfRange &e) {
            std::stringstream newErrorMsg;
            newErrorMsg 
                << "Too few busses for all moves in instruction."
                << std::endl;

            IllegalMachine error(
                    __FILE__, __LINE__, __func__,
                    newErrorMsg.str() + e.errorMessage());
            error.setCause(e);
            throw error;
        }
        bool isPossible = false;

        switch (type) {
        case RQST_READ:
        case RQST_WRITE: {
            for (int i = 0; i < rf->portCount(); i++) {
                Port* port = rf->port(i);

                Socket* sock =
                    (type == RQST_WRITE) ?
                    (port->inputSocket()) :
                    (port->outputSocket());

                if (sock != NULL && sock->isConnectedTo(*bus)) {
                    isPossible = true;

                    // we never store information that which port of RF
                    // was used for reading or writing, since TPEF doesn't
                    // support storing that data :(
                    if (port->name() != term.indexTerm.part2) {

                        parent_->addWarning(
                            currentLine,
                            "Selected different port for move"
                            "that was given by user.");
                    }

                    newResID.width = port->width();
                    break;
                }
            }

        } break;

        case RQST_INVGUARD:
        case RQST_GUARD: {
            bool isInverted =
                (type == RQST_GUARD) ?
                (false) : (true);

            for (int i = 0; i < bus->guardCount(); i++) {
                RegisterGuard* guard =
                    dynamic_cast<RegisterGuard*>(bus->guard(i));

                if (guard != NULL) {
                    if (guard->registerFile() == rf &&
                        guard->registerIndex() ==
                        static_cast<int>(term.indexTerm.index) &&
                        guard->isInverted() == isInverted) {

                        isPossible = true;
                        break;
                    }
                }
            }

        } break;

        default:
            abortWithError("Unknown request type: " +
                           Conversion::toString(type));
        }

        // if request is possible
        if (isPossible) {
            newResID.unit = registerFileID(rf);
            newResID.index = term.indexTerm.index;

            if (dynamic_cast<ImmediateUnit*>(rf) == NULL) {
                newResID.type =  MoveElement::MF_RF;
            } else {
                newResID.type =  MoveElement::MF_IMM;
            }

        } else {
            rf = NULL;
        }
    }

    if (rf == NULL && fu != NULL) {
        //  check if operation is found from fu
        if (fu->hasOperation(term.indexTerm.part2)) {

            // check that requested port is possible
            HWOperation* oper =
                fu->operation(term.indexTerm.part2);


            Port* port = oper->port(term.indexTerm.index);

            if (port == NULL) {
                throw IllegalMachine(
                    __FILE__, __LINE__, __func__,
                    "Can't find operand: " +
                    term.indexTerm.part2 + "." +
                    Conversion::toString(term.indexTerm.index));
            }

            Bus* bus = NULL;
            try { 
                bus = adf_.busNavigator().item(slotNumber);
            } catch (OutOfRange &e) {
                std::stringstream newErrorMsg;
                newErrorMsg 
                    << "Too few busses for all moves in instruction."
                    << std::endl;

                IllegalMachine error(
                    __FILE__, __LINE__, __func__,
                    newErrorMsg.str() + e.errorMessage());
                error.setCause(e);
                throw error;
            }
            bool isPossible = false;

            switch (type) {
            case RQST_WRITE:
            case RQST_READ: {
                Socket* sock =
                    (type == RQST_WRITE) ?
                    (port->inputSocket()) :
                    (port->outputSocket());

                if (sock != NULL && sock->isConnectedTo(*bus)) {
                    isPossible = true;
                    break;
                }
            } break;

            case RQST_GUARD:
            case RQST_INVGUARD: {
                bool isInverted =
                    (type == RQST_GUARD) ?
                    (false) : (true);

                for (int i = 0; i < bus->guardCount(); i++) {
                    PortGuard* guard =
                        dynamic_cast<PortGuard*>(bus->guard(i));

                    if (guard != NULL) {
                        if (guard->port() == port &&
                            guard->isInverted() == isInverted) {

                            isPossible = true;
                            break;
                        }
                    }
                }
            } break;

            default:
                abortWithError("Unknown request type: " +
                               Conversion::toString(type));
            }

            std::string operationRegisterStr =
                term.indexTerm.part2 + "." +
                Conversion::toString(term.indexTerm.index);

            if (isPossible) {
                newResID.width = port->width();
                newResID.unit = functionUnitID(fu);
                newResID.index = opOrPortID(operationRegisterStr, port);
                newResID.type =  MoveElement::MF_UNIT;

            } else {
                fu = NULL;
            }

        } else {
            throw IllegalMachine(
                __FILE__, __LINE__, __func__,
                "Can't find operation: " + term.indexTerm.part2);
        }
    }

    // if both tests failed
    if (rf == NULL && fu == NULL) {
        throw IllegalMachine(
            __FILE__, __LINE__, __func__,
            "Can't find connection for " + requestTypeString(type) +
            "fu term: " + resourceKeyString);
    }

    return newResID;
}


/**
 * Finds resource for rf.index term.
 *
 * Helper method for indexResource method.
 *
 * @param rf Register file of term.
 * @param term Requested term.
 * @param slotNumber Number of move slot, that was used.
 * @param type Request type.
 * @param resourceKeyString Resource key string for error messages.
 * @return TPEF resource information of requested term.
 * @exception IllegalMachine There was problems finding resource from ADF.
 */
MachineResourceManager::ResourceID
MachineResourceManager::registerFileIndexReference(
    BaseRegisterFile* rf,
    const RegisterTerm& term,
    UValue slotNumber,
    RequestType type,
    std::string& resourceKeyString)
    throw (IllegalMachine) {

    ResourceID newResID;
    // this is normal rf reference unless register is immediate unit...
    newResID.type = MoveElement::MF_RF;

    if (rf == NULL) {
        throw IllegalMachine(
            __FILE__, __LINE__, __func__,
            "Can't find connection for " +  requestTypeString(type) +
            " rf.index term: " + resourceKeyString);
    }

    // check that there is enough registers in file
    if(static_cast<UValue>(rf->numberOfRegisters()) <= term.indexTerm.index) {

        throw IllegalMachine(
            __FILE__, __LINE__, __func__,
            "Not enough registers in registerfile.");
    }

    // check defined if port can be used for current direction
    // and bus
    Bus* bus = adf_.busNavigator().item(slotNumber);
    bool isPossible = false;

    switch (type) {

    case RQST_READ:
    case RQST_WRITE: {
        ImmediateUnit* immUnit = dynamic_cast<ImmediateUnit*>(rf);

        // Code section creator checks in the end if it's really possible...
        if (immUnit != NULL) {
            isPossible = true;
            newResID.type = MoveElement::MF_IMM;
            break;
        }

        for (int i = 0; i < rf->portCount(); i++) {
            Port* port = rf->port(i);

            Socket* sock =
                (type == RQST_WRITE) ?
                (port->inputSocket()) :
                (port->outputSocket());

            if (sock != NULL && sock->isConnectedTo(*bus)) {
                isPossible = true;
                break;
            }
        }

    } break;

    case RQST_INVGUARD:
    case RQST_GUARD: {
        bool invertedRequested = (type == RQST_INVGUARD);

        for (int i = 0; i < bus->guardCount(); i++) {
            RegisterGuard* guard =
                dynamic_cast<RegisterGuard*>(bus->guard(i));

            if (guard != NULL) {
                if (guard->registerFile() == rf &&
                    guard->registerIndex() ==
                    static_cast<int>(term.indexTerm.index) &&
                    guard->isInverted() == invertedRequested) {

                    isPossible = true;
                    break;
                }
            }
        }

    } break;

    default:
        abortWithError("Unknown request type: " +
                       Conversion::toString(type));
    }

    if (isPossible) {
        newResID.width = rf->width();
        newResID.unit = registerFileID(rf);
        newResID.index = term.indexTerm.index;

    } else {
        throw IllegalMachine(
            __FILE__, __LINE__, __func__,
            "Can't find connection for " + requestTypeString(type) +
            " term: " + resourceKeyString);
    }

    return newResID;
}
