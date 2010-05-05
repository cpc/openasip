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
 * @file ProgramAnnotation.cc
 *
 * Implementation of ProgramAnnotation class.
 *
 * @author Pekka J‰‰skel‰inen 2006 (pekka.jaaskelainen-no.spam-tut.fi)
 * @note rating: red
 */

#include <string>
#include "ProgramAnnotation.hh"
#include "Conversion.hh"

namespace TTAProgram {

/////////////////////////////////////////////////////////////////////////////
// ProgramAnnotation
/////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 *
 * @param id The id of the annotation (a 24-bit value).
 * @param data The payload data as a string.
 */
ProgramAnnotation::ProgramAnnotation(Id id, const std::string& data) :
    id_(id) {
    setStringValue(data);
}

ProgramAnnotation::ProgramAnnotation(Id id, int value) :
    id_(id) {
    setIntValue(value);
}

/**
 * Constructor.
 *
 * @param id The id of the annotation (a 24-bit value).
 * @param data The payload data as a byte vector.
 */
ProgramAnnotation::ProgramAnnotation(
    Id id, const std::vector<Byte>& payload) :
    id_(id), payload_(payload) {
}

/**
 * Destructor.
 */
ProgramAnnotation::~ProgramAnnotation() {
}

/**
 * The payload data as a std::string.
 *
 * @returns The data as a std::string.
 */
std::string
ProgramAnnotation::stringValue() const {
    return std::string(payload_.begin(), payload_.end());
}

int
ProgramAnnotation::intValue() const {
    return Conversion::toInt(stringValue());
}

void
ProgramAnnotation::setIntValue(int value) {
    std::string data = Conversion::toString(value);
    payload_ = std::vector<Byte>(data.begin(), data.end());
}

/**
 * Returns the id of the annotation.
 *
 * @return the id of the annotation.
 */
ProgramAnnotation::Id
ProgramAnnotation::id() const {
    return id_;
}

/**
 * Return the payload data as a non-mutable char vector.
 *
 * @return The payload data.
 */
const std::vector<Byte>&
ProgramAnnotation::payload() const {
    return payload_;
}

} // namespace TTAProgram
