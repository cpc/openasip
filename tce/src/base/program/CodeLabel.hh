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
 * @file CodeLabel.hh
 *
 * Declaration of CodeLabel class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_CODE_LABEL_HH
#define TTA_CODE_LABEL_HH

#include <string>
#include "Label.hh"
#include "Exception.hh"
#include "InstructionReference.hh"
#include "InstructionReferenceManager.hh"

namespace TTAProgram {

class InstructionReference;
class Procedure;

/**
 * A symbolic name that stands for a location in the program.
 */
class CodeLabel : public Label {
public:
    CodeLabel(const InstructionReference& ins, std::string name)
        throw (IllegalRegistration);
    CodeLabel(const Procedure& proc)
        throw (IllegalRegistration);

    virtual ~CodeLabel();

    virtual Address address() const;

    const InstructionReference instructionReference() const
        throw (IllegalRegistration);
    const Procedure& procedure() const throw (IllegalRegistration);

private:
    /// Reference to instruction corresponding to this label.
    const InstructionReference ins_;
    /// Procedure corresponding to this label.
    const Procedure* proc_;
};

}

#endif
