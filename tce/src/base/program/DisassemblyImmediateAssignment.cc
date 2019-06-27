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
 * @file DisassemblyImmediateAssignment.cc
 *
 * Implementation of DisassemblyImmediateAssignment class.
 *
 * @author Veli-Pekka J��skel�inen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "DisassemblyImmediateAssignment.hh"
#include "Conversion.hh"

/**
 * The Constructor.
 *
 * Creates disassembly of an immediate assignment.
 *
 * @param value Value of the immediate to assign.
 * @param destination The destination register.
 */
DisassemblyImmediateAssignment::DisassemblyImmediateAssignment(
    SimValue value,
    bool sign,
    DisassemblyElement* destination):
    DisassemblyInstructionSlot(),
    value_(value), destination_(destination), hasValue_(true), signed_(sign) {

}

/**
 * The destructor.
 */
DisassemblyImmediateAssignment::~DisassemblyImmediateAssignment() {
    delete destination_;
}


/**
 * Disassembles the immediate assignment.
 *
 * @return Disassembled immediate assignment as a string.
 */
std::string
DisassemblyImmediateAssignment::toString() const {
    std::string disassembly =  "[" + destination_->toString();
    if (hasValue_) {
        if (signed_) {
            disassembly =
                disassembly + "=" + Conversion::toString(value_.sIntWordValue());
        } else {
            disassembly =
                disassembly + "=" + Conversion::toString(value_.uIntWordValue());
        }
    }
    disassembly = disassembly + "]";
    
    for (int i = 0; i < annotationCount(); i++) {
        disassembly += annotation(i).toString();
    }
    
    return disassembly;
}
