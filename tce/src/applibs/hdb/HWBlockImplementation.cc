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
 * @file HWBlockImplementation.cc
 *
 * Implementation of HWBlockImplementation class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#include <string>

#include "HWBlockImplementation.hh"
#include "BlockImplementationFile.hh"
#include "SequenceTools.hh"
#include "ContainerTools.hh"

using std::string;

namespace HDB {

/**
 * The constructor.
 *
 * @param moduleName Name of the module.
 * @param clkPort Name of the clock port.
 * @param rstPort Name of the reset port.
 * @param glockPort Name of the global lock port.
 */
HWBlockImplementation::HWBlockImplementation(
    const std::string& moduleName,
    const std::string& clkPort,
    const std::string& rstPort,
    const std::string& glockPort) :
    moduleName_(moduleName), clkPort_(clkPort), rstPort_(rstPort), 
    glockPort_(glockPort) {
}

/**
 * Copy constructor.
 *
 * @param original HWBlock to copy.
 */
HWBlockImplementation::HWBlockImplementation(
    const HWBlockImplementation& original) {

    moduleName_ = original.moduleName();
    clkPort_ = original.clkPort();
    rstPort_ = original.rstPort();
    glockPort_ = original.glockPort();
    hasID_ = original.hasID();

    if (original.hasID()) {
        id_ = original.id();
    } else {
        id_ = -1;
    }

    // Deep copy implementation file list.
    for (int i = 0; i < original.implementationFileCount(); i++) {
        BlockImplementationFile* newFile =
            new BlockImplementationFile(original.file(i));

        addImplementationFile(newFile);
    }
}


/**
 * The destructor.
 */
HWBlockImplementation::~HWBlockImplementation() {
    SequenceTools::deleteAllItems(files_);
}


/**
 * Tells whether the implementation has an ID.
 *
 * @return True if the entry has an ID, otherwise false.
 */
bool
HWBlockImplementation::hasID() const {
    return hasID_;
}


/**
 * Sets the ID for the implementation.
 *
 * @param id The ID to set.
 */
void
HWBlockImplementation::setID(RowID id) {
    hasID_ = true;
    id_ = id;
}


/**
 * Returns the ID of the implementation.
 *
 * @return ID of the implementation.
 */
RowID
HWBlockImplementation::id() const {
    if (!hasID()) {
        throw NotAvailable(__FILE__, __LINE__, __func__);
    } else {
        return id_;
    }
}

/**
 * Sets the module name.
 *
 * @param name Name of the module.
 */
void
HWBlockImplementation::setModuleName(const std::string& name) {
    moduleName_ = name;
}


/**
 * Returns the name of the module.
 *
 * @return The name of the module.
 */
std::string
HWBlockImplementation::moduleName() const {
    return moduleName_;
}


/**
 * Sets the name of the clock port.
 *
 * @param name Name of the port.
 */
void
HWBlockImplementation::setClkPort(const std::string& name) {
    clkPort_ = name;
}


/**
 * Returns the name of the clock signal port.
 *
 * @return The name of the port.
 */
std::string
HWBlockImplementation::clkPort() const {
    return clkPort_;
}


/**
 * Sets the name of the reset port.
 *
 * @param name Name of the port.
 */
void
HWBlockImplementation::setRstPort(const std::string& name) {
    rstPort_ = name;
}


/**
 * Returns the name of the reset port.
 *
 * @return The name of the port.
 */
std::string
HWBlockImplementation::rstPort() const {
    return rstPort_;
}


/**
 * Sets the name of the global lock port.
 *
 * @param name Name of the port.
 */
void
HWBlockImplementation::setGlockPort(const std::string& name) {
    glockPort_ = name;
}


/**
 * Returns the name of the global lock port.
 *
 * @return The name of the port.
 */
std::string
HWBlockImplementation::glockPort() const {
    return glockPort_;
}


/**
 * Adds a new implementation file for the block implementation.
 *
 * @param file The file to add.
 */
void
HWBlockImplementation::addImplementationFile(BlockImplementationFile* file) {
    files_.push_back(file);
}


/**
 * Removes the given block implementation file from the implementation.
 *
 * @param file The file to remove.
 */
void
HWBlockImplementation::removeImplementationFile(
    const BlockImplementationFile& file) {
    ContainerTools::removeValueIfExists(files_, &file);
}


/**
 * Returns the number of files which constitutes the implementation of the 
 * block.
 *
 * @return The number of files.
 */
int
HWBlockImplementation::implementationFileCount() const {
    return files_.size();
}


/**
 * Returns a block implementation file by the given index.
 *
 * @param index The index.
 * @exception OutOfRange If the index is negative or not smaller than the
 *                       number of files.
 */
BlockImplementationFile&
HWBlockImplementation::file(int index) const {
    if (index < 0 || index >= implementationFileCount()) {
        const string procName = "HWBlockImplementation::file";
        throw OutOfRange(__FILE__, __LINE__, procName);
    }

    return *files_[index];
}
}
