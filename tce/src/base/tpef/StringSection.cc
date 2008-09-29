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
 * @file StringSection.cc
 *
 * Definition of StringSection class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme-no.spam-tut.fi)
 * @author Mikael Lepistö 2004 (tmlepist-no.spam-cs.tut.fi)
 * @note reviewed 17 October 2003 by am, pj, rm, kl
 *
 * @note rating: yellow
 */

#include "StringSection.hh"

namespace TPEF {

using std::string;

// registers section prototype to the base class
StringSection StringSection::proto_(true);

/**
 * Constructor.
 *
 * @param init True if registeration is wanted.
 */
StringSection::StringSection(bool init) : DataSection(false) {
    if (init) {
        Section::registerSection(this);
    }

    unsetFlagVLen();
    unsetFlagNoBits();
}

/**
 * Destructor.
 */
StringSection::~StringSection() {
}

/**
 * Returns the string of the given chunk.
 *
 * @param chunk Chunk where string starts.
 * @exception UnexpectedValue If no terminating zero is found.
 */
string
StringSection::chunk2String(const Chunk* chunk) const
    throw (UnexpectedValue) {

    unsigned int offset = chunk->offset();
    assert(offset <= length());

    string result = "";
    while (byte(offset) != 0) {
        // if we are reading the last byte in section and it is not zero,
        // we have an exception
        if (offset == length() - 1) {
            throw UnexpectedValue(
                __FILE__, __LINE__, __func__,
                "No terminating zero found!");
        }
        result.append(1, byte(offset));
        offset++;
    }

    return result;
}

/**
 * Checks if string is already found from section and returns chunk 
 * pointing to it.
 *
 * If string is not already found from section, it is added to end, 
 * before returning chunk.
 *
 * @param str String to find from section.
 * @return Chunk pointing to requested string.
 */
Chunk*
StringSection::string2Chunk(const std::string &str) {

    // needed first byte
    if (length() == 0) {
        addByte(0);
    }

    for (int i = 0;
         i < static_cast<int>(length()) -
             static_cast<int>(str.length()); i++) {

        Word charsToMatch = str.length() + 1;

        while (charsToMatch) {
            charsToMatch--;

            if (byte(i + charsToMatch) != str.c_str()[charsToMatch]) {
                charsToMatch = 1;
                break;
            }
        }

        // we found matching string
        if (charsToMatch == 0) {
            return chunk(i);
        }
    }

    // didn't find matching string, add new one
    SectionOffset returnOffset = length();

    for (Word i = 0; i < str.size(); i++) {
        addByte(str[i]);
    }
    addByte(0);

    return chunk(returnOffset);
}

/**
 * Returns the type of section.
 *
 * @return Type of section.
 */
Section::SectionType
StringSection::type() const {
    return ST_STRTAB;
}

/**
 * Creates an instance of the class.
 *
 * @return Newly created section.
 */
Section*
StringSection::clone() const {
    return new StringSection(false);
}

}
