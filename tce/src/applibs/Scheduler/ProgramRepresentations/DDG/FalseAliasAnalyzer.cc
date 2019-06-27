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
 * @file FalseAliasAnalyzer.cc
 *
 * Implementation of FalseAliasAnalyzer class.
 * 
 * @author Heikki Kultala 2007 (heikki.kultala-no.spam-tut.fi)
 * @note rating: red
 */

#include "FalseAliasAnalyzer.hh"

/**
 * Checks whether can say something about a memory address. 
 *
 * @return always returns true
 */
bool 
FalseAliasAnalyzer::isAddressTraceable(
    DataDependenceGraph&, const ProgramOperation&) {
    return true;
}

/**
 * checks whether mem adderesses alias. As this is fake, always returns false.
 * @return always ALIAS_FALSE
 */
MemoryAliasAnalyzer::AliasingResult
FalseAliasAnalyzer::analyze(
    DataDependenceGraph&, const ProgramOperation&, const ProgramOperation&) {
    return ALIAS_FALSE;
}

/**
 * Desctructor
 */
FalseAliasAnalyzer::~FalseAliasAnalyzer() {}
