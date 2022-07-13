/*
    Copyright (c) 2002-2021 Tampere University.

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
 * @file OperationTriggeredEncoding.hh
 *
 * Declaration of OperationTriggeredEncoding class.
 *
 * @author Kari Hepola 2021 (kari.hepola@tuni.fi)
 * @note rating: red
 */

#ifndef TTA_OPERATION_TRIGGERED_ENCODING_HH
#define TTA_OPERATION_TRIGGERED_ENCODING_HH

#include "Serializable.hh"
#include <vector>
#include <string>

class InstructionFormat;
class OperationTriggeredField;


class OperationTriggeredEncoding : public Serializable {
public:

    OperationTriggeredEncoding(const std::string& name,
    InstructionFormat& parent);

    OperationTriggeredEncoding(const ObjectState* state,
    InstructionFormat& parent);

    ~OperationTriggeredEncoding();

    std::string name() const;
    void setName(const std::string& name);

    void addField(OperationTriggeredField& field);

    // methods derived from InstructionField
    virtual int childFieldCount() const;
    virtual int width() const;

    // methods inherited from Serializable interface
    virtual void loadState(const ObjectState* state);
    virtual ObjectState* saveState() const;

    static const std::string OSNAME_OTA_ENCODING;
    static const std::string OSKEY_OTA_ENCODING_NAME;


private:

    std::string name_;

    std::vector<OperationTriggeredField*> fields_;

};

#endif
