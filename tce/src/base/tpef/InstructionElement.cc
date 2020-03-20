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
 * @file InstructionElement.cc
 *
 * Non-inline definitions of InstructionElement class.
 *
 * @author Mikael Lepistö 2003 (tmlepist-no.spam-cs.tut.fi)
 *
 * @note rating: yellow
 */

#include "InstructionElement.hh"

namespace TPEF {

//////////////////////////////////////////////////////////////////////////////
// InstructionAnnotation definition.
//////////////////////////////////////////////////////////////////////////////

/// Maximum number of bytes in annotation.
const size_t InstructionAnnotation::MAX_ANNOTATION_BYTES = 127;

/**
 * Constructor.
 *
 * @param id The id of the annotation.
 * @param payload The payload data as a char vector.
 */
InstructionAnnotation::InstructionAnnotation(
    Word id, const std::vector<Byte>& payload) : 
    id_(id), payLoad_(payload) {
}


/**
 * Constructor
 */
InstructionAnnotation::InstructionAnnotation(Word anId) : id_(anId) {
}

/**
 * Destructor
 */
InstructionAnnotation::~InstructionAnnotation() {
}

//////////////////////////////////////////////////////////////////////////////
// InstructionElement definition.
//////////////////////////////////////////////////////////////////////////////

/**
 * Constructor
 */
InstructionElement::InstructionElement(bool isMove) :
    SectionElement(), begin_(false), isMove_(isMove) {
}

/**
 * Destructor
 */
InstructionElement::~InstructionElement() {
    for (Word i = 0; i < annotationCount(); i++) {
        delete annotation(i);
    }
}

}
