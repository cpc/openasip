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
