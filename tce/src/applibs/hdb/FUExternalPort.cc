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
 * @file FUExternalPort.cc
 *
 * Implementation of FUExternalPort class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#include "FUExternalPort.hh"
#include "FUImplementation.hh"
#include "ContainerTools.hh"

namespace HDB {

/**
 * The constructor.
 *
 * Registers the port automatically to the given FUImplementation instance.
 *
 * @param name Name of the port.
 * @param direction Direction of the port.
 * @param widthFormula The formula for the width of the port.
 * @param description Description of the port.
 * @param parent The parent FUImplementation instance.
 */
FUExternalPort::FUExternalPort(
    const std::string& name,
    Direction direction,
    const std::string& widthFormula,
    const std::string& description,
    FUImplementation& parent) :
    ExternalPort(name, direction, widthFormula, description) {

    parent.addExternalPort(this);
}


/**
 * The destructor.
 */
FUExternalPort::~FUExternalPort() {
}

}
