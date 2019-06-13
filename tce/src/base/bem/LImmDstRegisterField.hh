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
    LImmDstRegisterField(int width, BinaryEncoding& parent);
    LImmDstRegisterField(const ObjectState* state, BinaryEncoding& parent);
    virtual ~LImmDstRegisterField();

    BinaryEncoding* parent() const;

    void addDestination(
        const std::string& instructionTemplate,
        const std::string& immediateUnit);

    int instructionTemplateCount() const;
    std::string instructionTemplate(int index) const;
    bool usedByInstructionTemplate(
        const std::string& instructionTemplate) const;
    std::string immediateUnit(const std::string& instructionTemplate) const;

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

    void loadState(const ObjectState* state);

    /// Width of the field.
    int width_;
    /// Maps instruction templates to immediate units
    StringMap destinationMap_;
};

#endif
