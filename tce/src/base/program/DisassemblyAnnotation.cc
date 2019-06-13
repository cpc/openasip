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
 * @file DisassemblyAnnotation.cc
 *
 * Definition of DisassemblyAnnotation class.
 *
 * @author Mikael Lepistö 2007 (tmlepist-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "DisassemblyAnnotation.hh"
#include "Conversion.hh"
#include "StringTools.hh"

using std::string;

/**
 * Constructor.
 *
 * @param id Id of the annotation.
 * @param payload Data for the annotation.
 */
DisassemblyAnnotation::DisassemblyAnnotation(
    Word id, const std::vector<Byte>& payload):
    DisassemblyElement(), id_(id), payload_(payload) {
}

/**
 * Destructor.
 */
DisassemblyAnnotation::~DisassemblyAnnotation() {
}


/**
 * Returns disassembly of the annotation.
 */
string
DisassemblyAnnotation::toString() const {

    std::stringstream retVal;
    retVal << "{" << std::hex << "0x" << id_;    

    for (unsigned int i = 0; i < payload_.size(); i++) {
        retVal << " " << int(payload_[i]);
    }

    retVal << "}";
    return retVal.str();
}
