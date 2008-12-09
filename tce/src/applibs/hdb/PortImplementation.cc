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
 * @file PortImplementation.cc
 *
 * Implementation of PortImplementation class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#include "PortImplementation.hh"

namespace HDB {

/**
 * The constructor.
 *
 * @param name Name of the port.
 * @param loadPort Name of the load enable port.
 */
PortImplementation::PortImplementation(
    const std::string& name,
    const std::string& loadPort) :
    name_(name), loadPort_(loadPort) {
}


/**
 * The destructor.
 */
PortImplementation::~PortImplementation() {
}


/**
 * Sets the name of the port.
 *
 * @param name The new name.
 */
void
PortImplementation::setName(const std::string& name) {
    name_ = name;
}


/**
 * Returns the name of the port.
 *
 * @return The name of the port.
 */
std::string
PortImplementation::name() const {
    return name_;
}


/**
 * Sets the name of the load enable port.
 *
 * @param name The new name.
 */
void
PortImplementation::setLoadPort(const std::string& name) {
    loadPort_ = name;
}


/**
 * Returns the name of the load enable port.
 *
 * @return The name of the load enable port.
 */
std::string
PortImplementation::loadPort() const {
    return loadPort_;
}
}
