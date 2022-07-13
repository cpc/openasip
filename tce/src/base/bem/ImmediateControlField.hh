/*
    Copyright (c) 2002-2014 Tampere University.

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
 * @file ImmediateControlField.hh
 *
 * Declaration of ImmediateControlField class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @author Pekka Jääskeläinen 2014
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
    ImmediateControlField(BinaryEncoding& parent);
    ImmediateControlField(const ObjectState* state, BinaryEncoding& parent);
    virtual ~ImmediateControlField();

    BinaryEncoding* parent() const;

    int templateCount() const;
    std::string instructionTemplate(int index) const;

    bool hasTemplateEncoding(const std::string& name) const;
    unsigned int templateEncoding(const std::string& name) const;

    void addTemplateEncoding(const std::string& name, unsigned int encoding);
    void removeTemplateEncoding(const std::string& name);

    // virtual methods derived from InstructionField
    virtual int width() const;
    virtual int childFieldCount() const;
    virtual InstructionField& childField(int) const;

    // methods inherited from Serializable interface
    virtual void loadState(const ObjectState* state);
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
