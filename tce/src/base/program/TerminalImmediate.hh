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
 * @file TerminalImmediate.hh
 *
 * Declaration of TerminalImmediate class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_TERMINAL_IMMEDIATE_HH
#define TTA_TERMINAL_IMMEDIATE_HH

#include "Terminal.hh"
#include "AnnotatedInstructionElement.hh"

namespace TTAProgram {

/**
 * Represents an inline immediate.
 */
class TerminalImmediate : public Terminal, public AnnotatedInstructionElement {
public:
    TerminalImmediate(SimValue value);
    virtual ~TerminalImmediate();

    virtual bool isImmediate() const;
    virtual SimValue value() const;

    virtual Terminal* copy() const;
    virtual bool equals(const Terminal& other) const;

    virtual TCEString toString() const;

private:
    /// Assignment not allowed.
    TerminalImmediate& operator=(const TerminalImmediate&);
protected:
    /// Value of the inline immediate.
    SimValue value_;
};

}

#endif
