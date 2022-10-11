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
 * @file OperationTriggeredField.hh
 *
 * Declaration of OperationTriggeredField class.
 *
 * @author Kari Hepola 2022 (kari.hepola@tuni.fi)
 * @note rating: red
 */


#ifndef TTA_OPERATION_TRIGGERED_FIELD_HH
#define TTA_OPERATION_TRIGGERED_FIELD_HH

#include "Serializable.hh"
#include <vector>
#include <string>

class OperationTriggeredEncoding;

class OperationTriggeredField : public Serializable {
public:

    OperationTriggeredField(const ObjectState* state,
    OperationTriggeredEncoding& parent);

    OperationTriggeredField(OperationTriggeredEncoding& parent, int piece,
    int start, int width);

    int piece() const;
    void setPiece(int piece);
    int start() const;
    void setStart(int start);

    // methods inherited from Serializable interface
    virtual void loadState(const ObjectState* state);
    virtual ObjectState* saveState() const;

    virtual int childFieldCount() const;
    virtual int width() const;
    void setWidth(int width);

    static const std::string OSNAME_FIELD;
    static const std::string OSKEY_PIECE_NAME;
    static const std::string OSKEY_START_NAME;
    static const std::string OSKEY_WIDTH_NAME;

private:
    int piece_;
    int start_;
    int width_;


};

#endif