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
 * @file ImmediateControlField.hh
 *
 * Declaration of ImmediateControlField class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_IMMEDIATE_CONTROL_FIELD_HH
#define TTA_IMMEDIATE_CONTROL_FIELD_HH

#include <string>
#include <map>

#include "InstructionField.hh"
#include "Exception.hh"
#include "Serializable.hh"

class BinaryEncoding;
class MoveSlot;

/**
 * ImmediateControlField is a specialised InstructionField and represents the
 * field of the TTA instruction that contains the long immediate control tag.
 *
 * This tag defines the template of the instruction. A TTA instruction
 * template specifies which move slots are used to program data
 * transports and which are used to encode immediate bits. An
 * ImmediateControlField object can never exist alone. It is always
 * registered to a BinaryEncoding object.
 */
class ImmediateControlField : public InstructionField {
public:
    ImmediateControlField(BinaryEncoding& parent)
	throw (ObjectAlreadyExists);
    ImmediateControlField(const ObjectState* state, BinaryEncoding& parent)
	throw (ObjectStateLoadingException, ObjectAlreadyExists);
    virtual ~ImmediateControlField();

    BinaryEncoding* parent() const;

    int templateCount() const;
    std::string instructionTemplate(int index) const
	throw (OutOfRange);

    bool hasTemplateEncoding(const std::string& name) const;
    unsigned int templateEncoding(const std::string& name) const
	throw (InstanceNotFound);

    void addTemplateEncoding(const std::string& name, unsigned int encoding)
	throw (ObjectAlreadyExists);
    void removeTemplateEncoding(const std::string& name);

    // virtual methods derived from InstructionField
    virtual int width() const;
    virtual int childFieldCount() const;
    virtual InstructionField& childField(int) const
	throw (OutOfRange);

    // methods inherited from Serializable interface
    virtual void loadState(const ObjectState* state)
	throw (ObjectStateLoadingException);
    virtual ObjectState* saveState() const;

    /// ObjectState name for immediate control field.
    static const std::string OSNAME_IMM_CONTROL_FIELD;
    /// ObjectState name for a template mapping.
    static const std::string OSNAME_TEMPLATE_MAP;
    /// ObjectState attribute key for the name of the instruction template.
    static const std::string OSKEY_TEMPLATE_NAME;
    /// ObjectState attribute key for the encoding of the instruction
    /// template.
    static const std::string OSKEY_ENCODING;

private:
    /// A map type for binary encodings.
    typedef std::map<std::string, unsigned int> EncodingMap;

    void clearTemplateEncodings();

    /// Binary encodings for instruction templates.
    EncodingMap templates_;
};

#endif
