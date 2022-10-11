/*
    Copyright (c) 2002-2017 Tampere University.

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
 * @file DummyMemGenerator.hh
 *
 * Declaration of DummyMemGenerator class.
 *
 * @author Otto Esko 2011 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_DUMMY_MEM_GENERATOR_HH
#define TTA_DUMMY_MEM_GENERATOR_HH

#include <iostream>
#include <vector>
#include "MemoryGenerator.hh"
#include "TCEString.hh"

class PlatformIntegrator;

class DummyMemGenerator : public MemoryGenerator {
public:

    DummyMemGenerator(
        int memMauWidth,
        int widthInMaus,
        int addrWidth,
        const PlatformIntegrator* integrator,
        std::ostream& warningStream,
        std::ostream& errorStream);

    virtual ~DummyMemGenerator();

    virtual bool generatesComponentHdlFile() const {return false;};
    virtual bool checkFuPort(
        const std::string,
        std::vector<TCEString>&) const {return true;};

    virtual std::vector<TCEString> generateComponentFile(TCEString);

    void addMemory(const ProGe::NetlistBlock&, ProGe::NetlistBlock&,
                   int, int) {}
protected:

    virtual TCEString moduleName() const {return "";};

    virtual TCEString instanceName(int, int) const {return "";};

};

#endif
