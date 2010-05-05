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
 * @file DisassemblyRegister.cc
 *
 * Implementation of DisassemblyRegister class.
 *
 * @author Veli-Pekka Jääskeläinen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @author Heikki Kultala 2009 (hkultala-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "DisassemblyRegister.hh"
#include "Conversion.hh"
#include "RegisterFile.hh"
#include "Terminal.hh"

/**
 * The constructor.
 *
 * @param rfName Name of the register file.
 * @param index Index of the register in the register file.
 */
DisassemblyRegister::DisassemblyRegister(std::string rfName, Word index) :
    DisassemblyElement(),
    rfName_(rfName),
    index_(index) {
}


/**
 * The destructor.
 */
DisassemblyRegister::~DisassemblyRegister() {
}



/**
 * Returns disassembly of the register file.
 *
 * @return Disassembly of the register as a string.
 */
std::string
DisassemblyRegister::toString() const {
    return registerName(rfName_, index_);
}

/**
 * Returns a name of the register.
 *
 * Does the int -> string conversion itself, because
 * Conversion::toString() uses stringstream which is slow.
 */
TCEString
DisassemblyRegister::registerName(
    const TCEString& rfName, int index, char delim) {
    char buf[12];
    char *ptr = buf + sizeof(buf);
    *--ptr = 0; // last to terminating zero.
    // conversion from 2 to 10 base
    do {
        *--ptr = (index%10) + 48; // 48 is conversion to ascii.
        index /= 10;
    } while (index);
    *--ptr = delim;
    return rfName + ptr;
}

/**
 * Returns name of a register.
 */
TCEString
DisassemblyRegister::registerName(
    const TTAMachine::RegisterFile& rf, int index, char delim) {
    return registerName(rf.name(), index, delim);
}

/**
 * Returns name of a register which is in a terminalregister.
 */
TCEString 
DisassemblyRegister::registerName(
    const TTAProgram::Terminal& term) {
    return registerName(term.registerFile(), term.index());
}
