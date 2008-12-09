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
 * @file ADFSerializerTextGenerator.cc
 *
 * Implementation of ADFSerializerTextGenerator class.
 *
 * @author Lasse Laasonen 2004 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#include "ADFSerializerTextGenerator.hh"

/**
 * Constructor.
 */
ADFSerializerTextGenerator::ADFSerializerTextGenerator() :
    TextGenerator() {

    addText(TXT_MULTIPLE_DESTINATIONS_IN_TEMPLATE_SLOT,
            "The slot '%1%' of instruction template '%2%' cannot write to "
            "two destination units. The same slot exists in the "
            "declarations of immediate units '%3%' and '%4%'.");
    addText(TXT_SAME_TEMPLATE_SLOT,
            "Template slot with bus '%1%' is declared two times in "
            "the declaration of immediate unit '%2%', template '%3%'.");
    addText(TXT_IT_EMPTY_AND_NON_EMPTY,
            "Instruction template '%1%' is declared empty in non-empty in "
            "different immediate unit declarations.");
    addText(TXT_EMPTY_IT_NOT_DECLARED,
            "The machine contains an empty instruction template that is not "
            "declared in the declaration of immediate unit '%1%'.");
}


/**
 * Destructor.
 */
ADFSerializerTextGenerator::~ADFSerializerTextGenerator() {
}


