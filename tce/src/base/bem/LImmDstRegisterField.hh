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
 * @file LImmDstRegisterField.hh
 *
 * Declaration of LImmDstRegisterField class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_LIMM_DST_REGISTER_FIELD_HH
#define TTA_LIMM_DST_REGISTER_FIELD_HH

#include <string>
#include <map>

#include "InstructionField.hh"

class BinaryEncoding;

/**
 * Represets the instruction field that contains the register index
 * where a long immediate is going to be written.
 */
class LImmDstRegisterField : public InstructionField {
public:
    LImmDstRegisterField(int width, BinaryEncoding& parent)
        throw (OutOfRange);
    LImmDstRegisterField(const ObjectState* state, BinaryEncoding& parent)
        throw (ObjectStateLoadingException);
    virtual ~LImmDstRegisterField();

    BinaryEncoding* parent() const;

    void addDestination(
        const std::string& instructionTemplate,
        const std::string& immediateUnit)
        throw (NotAvailable);

    int instructionTemplateCount() const;
    std::string instructionTemplate(int index) const
        throw (OutOfRange);
    bool usedByInstructionTemplate(
        const std::string& instructionTemplate) const;
    std::string immediateUnit(const std::string& instructionTemplate) const
        throw (NotAvailable);

    virtual int width() const;
    virtual int childFieldCount() const;
    virtual ObjectState* saveState() const;

    /// ObjectState name for long immediate destination register field.
    static const std::string OSNAME_LIMM_DST_REGISTER_FIELD;
    /// ObjectState attribute key for the width of the field.
    static const std::string OSKEY_WIDTH;
    /// ObjectState name for a immediate unit destination.
    static const std::string OSNAME_IU_DESTINATION;
    /// ObjectState attribute key for the name of the instruction template.
    static const std::string OSKEY_ITEMP;
    /// ObjectState attribute key for the name of the immediate unit.
    static const std::string OSKEY_DST_IU;

private:
    /// Map type for strings.
    typedef std::map<std::string, std::string> StringMap;

    void loadState(const ObjectState* state)
        throw (ObjectStateLoadingException);

    /// Width of the field.
    int width_;
    /// Maps instruction templates to immediate units
    StringMap destinationMap_;
};

#endif
