/*
 Copyright (c) 2002-2015 Tampere University.

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
/*
 * @file GlobalPackage.cc
 *
 * Implementation of GlobalPackage class.
 *
 * Created on: 9.6.2015
 * @author Henry Linjamäki 2015 (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#include "GlobalPackage.hh"

namespace ProGe {

GlobalPackage::GlobalPackage(const std::string& entityName)
    : entityName_(entityName) {}

GlobalPackage::~GlobalPackage() {}

/**
 * Return name of this package.
 */
const std::string
GlobalPackage::name() const {
    return entityName_ + "_globals";
}

/**
 * Returns name of the constant that defines address width of instruction
 * block visible outside the core.
 */
const std::string
GlobalPackage::fetchBlockAddressWidth() const {
    return "IMEMADDRWIDTH";
}

/**
 * Returns name of the constant that defines instruction block width visible
 * outside the core.
 */
const std::string
GlobalPackage::fetchBlockDataWidth() const {
    return "IMEMDATAWIDTH";
}

/**
 * Returns name of the constant that defines instruction block width in MAUs.
 */
const std::string
GlobalPackage::fetchBlockMAUWidth() const {
    return "IMEMWIDTHINMAUS";
}

/**
 * Return name of the constant that defines raw width of instruction.
 */
const std::string
GlobalPackage::instructionDataWidth() const {
    return "INSTRUCTIONWIDTH";
}

// todo package writer

} /* namespace ProGe */
