/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file ADFSerializerTextGenerator.cc
 *
 * Implementation of ADFSerializerTextGenerator class.
 *
 * @author Lasse Laasonen 2004 (lasse.laasonen@tut.fi)
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


