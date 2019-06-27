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
 * @file IUPortCode.hh
 *
 * Declaration if IUPortCode class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_IU_PORT_CODE_HH
#define TTA_IU_PORT_CODE_HH

#include "PortCode.hh"

/**
 * IUPortCode defined a binary encoding for an immediate unit port.
 */
class IUPortCode : public PortCode {
public:
    IUPortCode(
        const std::string& immediateUnit, unsigned int encoding,
        unsigned int extraBits, int indexWidth, SocketCodeTable& parent);
    IUPortCode(
        const std::string& immediateUnit, int indexWidth,
        SocketCodeTable& parent);
    IUPortCode(const ObjectState* state, SocketCodeTable& parent);
    virtual ~IUPortCode();

    virtual ObjectState* saveState() const;

    /// ObjectState name for RF port code.
    static const std::string OSNAME_IU_PORT_CODE;
};

#endif
