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
 * @file InterPassDatum.hh
 *
 * Declaration of InterPassDatum.
 *
 * @author Pekka J‰‰skel‰inen 2007 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_INTER_PASS_DATUM_HH
#define TTA_INTER_PASS_DATUM_HH

#include <map>
#include <set>
#include <string>

#include "TCEString.hh"

class InterPassDatum;

/** 
 * A base class for classes storing inter-pass data of any type.
 */
class InterPassDatum {
public:
    virtual ~InterPassDatum() {};

protected:
    // no direct instantiation assumed
    InterPassDatum() {}
};

/** 
 * A templated class for classes storing inter-pass data of any type.
 *
 * Inherits the interface of the given class and the InterPassDatum, thus
 * acts like the given class, but can be stored as inter-pass data. Should
 * make creating new InterPassDatum types easier.
 */
template <typename T>
class SimpleInterPassDatum : public InterPassDatum, public T {
public:
    SimpleInterPassDatum() : InterPassDatum(), T() {}
    virtual ~SimpleInterPassDatum() {};
};

/** 
 * A templated class for storing pointer to inter-pass data
 * 
 * Takes ownership of the data and calls delete for it.
 */
template <typename T>
class SimplePointerInterPassDatum : public InterPassDatum {
public:
    SimplePointerInterPassDatum() : InterPassDatum(), data(NULL) {}
    virtual ~SimplePointerInterPassDatum() { delete data; };

    T* data;
};

/// Datum type for transferring register name as a data.
/// Stack pointer register is saved with key STACK_POINTER by LLVMBackend.cc
typedef SimpleInterPassDatum<
    std::pair<TCEString, unsigned int> > RegDatum;

/// This datum is set in case the pass should only process (or ignore) 
/// certain functions. The key is FUNCTIONS_TO_PROCESS/IGNORE.
typedef SimpleInterPassDatum<std::set<std::string> > FunctionNameList;

#endif
