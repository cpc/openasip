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
 * @file MachineResourceManager.hh
 *
 * Declaration of MachineResourceManager class.
 *
 * @author Mikael Lepistö 2005 (tmlepist-no.spam-cs.tut.fi)
 * @note rating: yellow
 */

#ifndef TCEASM_RESOURCE_MANAGER_HH
#define TCEASM_RESOURCE_MANAGER_HH

#include <map>
#include <string>

#include "ParserStructs.hh"
#include "MoveElement.hh"
#include "Exception.hh"

namespace TPEF {
    class Binary;
    class ResourceElement;
    class ASpaceElement;
    class ASpaceSection;
    class StringSection;
    class NullSection;
    class ResourceSection;
    class Section;
    class Chunk;
}

namespace TTAMachine {
    class Machine;
    class FunctionUnit;
    class RegisterFile;
    class BaseRegisterFile;
    class Port;
}

class AssemblyParserDiagnostic;

/**
 * Provides all TPEF and machine resources needed by CodeSectionCreator
 * and DataSectionCreator.
 *
 * Search resources from Machine and returns corresponding TPEF resources.
 * Class also provides easy way to get common TPEF resource like strings,
 * null section and required null elements. After all needed resources are
 * returned the Binary that was given for MachineResourceManager will contain
 * valid TPEF with NullSection, ASpaceSection, StringSection
 * and ResourceSection.
 */
class MachineResourceManager {
public:
    /**
     * Type of resource id request.
     */
    enum RequestType {
        RQST_READ,     ///< Register of port for reading.
        RQST_WRITE,    ///< Register or port for writing.
        RQST_GUARD,    ///< Register or port guard.
        RQST_INVGUARD  ///< Inverted register or port guard.
    };

    /**
     * Result type when getting resource.
     */
    struct ResourceID {
        ResourceID() :
            type(TPEF::MoveElement::MF_NULL),
            unit(0), index(0), width(0) {
        }

        /// Resource type.
        TPEF::MoveElement::FieldType type;
        /// TPEF Resource unit id.
        UValue unit;
        /// TPEF Resource operand id or register file index.
        UValue index;
        /// Width of accessed port or other resource. 
        UValue width;
    };

    MachineResourceManager(
        TPEF::Binary &tpef, const TTAMachine::Machine &adf,
        AssemblyParserDiagnostic* parent_);

    TPEF::Section* nullSection();

    TPEF::StringSection* stringSection();

    TPEF::ResourceSection* resourceSection();

    TPEF::ASpaceElement *findDataAddressSpace(std::string name);

    UValue findBusWidth(UValue slotNumber);

    TPEF::ASpaceElement* undefinedAddressSpace();

    TPEF::ASpaceElement* codeAddressSpace();

    TPEF::Chunk* stringToChunk(const std::string aStr);

    ResourceID &resourceID(
        UValue currentLine, const RegisterTerm &term, UValue slotNumber,
        RequestType type);

private:
    /**
     * Cache element to see if same kind of resource reading has been
     * resolved earlier during the compilation
     */
    struct ResourceKey {
        /// Key string for resource.
        std::string keyString;  
        /// Bus that was used.
        UValue slotNumber;      
        /// Was resource read or written or was it guard.
        RequestType type;       

        bool operator<(const ResourceKey &comp) const {

            if (keyString < comp.keyString) {
                return true;
            }

            if (keyString > comp.keyString) {
                return false;
            }

            if (slotNumber < comp.slotNumber) {
                return true;
            }

            if (slotNumber > comp.slotNumber) {
                return false;
            }

            if (type < comp.type) {
                return true;
            }

            return false;
        }
    };

    void initResourceSection();

    void addResourceElement(TPEF::ResourceElement *resource);

    UValue registerFileID(TTAMachine::BaseRegisterFile *rf);

    UValue functionUnitID(TTAMachine::FunctionUnit *unit);

    UValue opOrPortID(
        std::string opOrPortString, const TTAMachine::Port *port);

    std::string requestTypeString(RequestType type) const;

    ResourceID functionUnitPortResource(const RegisterTerm &term);

    ResourceID indexResource(
        UValue currentLine, const RegisterTerm &term, UValue slotNumber,
        RequestType type, std::string &resourceKeyString);

    ResourceID rFPortOrFUIndexReference(
        TTAMachine::FunctionUnit *fu, TTAMachine::BaseRegisterFile *rf,
        UValue currentLine, const RegisterTerm &term, UValue slotNumber,
        RequestType type, std::string &resourceKeyString);

    ResourceID registerFileIndexReference(
        TTAMachine::BaseRegisterFile *rf, const RegisterTerm &term,
        UValue slotNumber, RequestType type, std::string &resourceKeyString);

    /// Binary where all used resources are added.
    TPEF::Binary  &tpef_;

    /// Machine where manager tries to find used resources.
    const TTAMachine::Machine &adf_;

    /// The address space section of TPEF.
    TPEF::ASpaceSection *aSpaceSection_;

    /// The string section of TPEF.
    TPEF::StringSection *strings_;

    /// The undefined address space element of TPEF.
    TPEF::ASpaceElement *undefASpace_;

    /// The instruction address space element of TPEF.
    TPEF::ASpaceElement *codeASpace_;
    /// Bookkeeping for already requested address spaces.
    std::map<std::string, TPEF::ASpaceElement*> addressSpaces_;

    /// The null section of TPEF.
    TPEF::NullSection *nullSection_;

    /// The resource section of TPEF.
    TPEF::ResourceSection *resourceSection_;
    /// Bookkeeping for already requested resources.
    std::map<ResourceKey, ResourceID> resourceMap_;

    /// For generating function unit resource ids.
    UValue lastFunctionUnitID_;
    /// Bookkeeping of already added function units.
    std::map<TTAMachine::FunctionUnit*, UValue> functionUnitIDs_;

    /// For generating register file resource ids.
    UValue lastRegisterFileID_;
    /// Bookkeeping of already added register files.
    std::map<TTAMachine::BaseRegisterFile*, UValue> registerFileIDs_;

    /// For generating shared ids for ports, operations or special registers.
    UValue lastOpOrSpecRegisterID_;
    /// Bookkeeping of already added ports, operations and special registers.
    std::map<std::string, UValue> opOrPortIDs_;

    /// Assembler root class for adding warnings.
    AssemblyParserDiagnostic* parent_;
};

#endif
