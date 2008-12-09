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
 * @file BlockImplementationFile.cc
 *
 * Implementation of BlockImplementationFile class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#include "BlockImplementationFile.hh"

namespace HDB {

/**
 * The constructor.
 *
 * @param pathToFile Path to the file.
 * @param format Format of the file.
 */
BlockImplementationFile::BlockImplementationFile(
    const std::string& pathToFile,
    Format format) :
    file_(pathToFile), format_(format) {
}


/**
 * The destructor.
 */
BlockImplementationFile::~BlockImplementationFile() {
}


/**
 * Returns path to the file.
 */
std::string
BlockImplementationFile::pathToFile() const {
    return file_;
}


/**
 * Returns the format of the file.
 */
BlockImplementationFile::Format
BlockImplementationFile::format() const {
    return format_;
}


/**
 * Sets the file path.
 *
 * @param pathToFile The new path.
 */
void
BlockImplementationFile::setPathToFile(const std::string& pathToFile) {
    file_ = pathToFile;
}


/**
 * Sets the format of the file.
 *
 * @param format The new format.
 */
void
BlockImplementationFile::setFormat(Format format) {
    format_ = format;
}
}
