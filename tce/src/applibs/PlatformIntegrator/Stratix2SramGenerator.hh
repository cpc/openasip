/*
    Copyright (c) 2002-2010 Tampere University of Technology.

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
 * @file Stratix2SramGenerator.hh
 *
 * Declaration of Stratix2SramGenerator class.
 *
 * @author Otto Esko 2010 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */
#ifndef TTA_STRATIX2_SRAM_GENERATOR_HH
#define TTA_STRATIX2_SRAM_GENERATOR_HH

#include <iostream>
#include <string>
#include <vector>
#include "MemoryGenerator.hh"
#include "PlatformIntegrator.hh"

class Stratix2SramGenerator : public MemoryGenerator {
public:

    Stratix2SramGenerator(
        int memMauWidth,
        int widthInMaus,
        int addrWidth,
        std::string initFile,
        const PlatformIntegrator* integrator,
        std::ostream& warningStream,
        std::ostream& errorStream);

    virtual ~Stratix2SramGenerator();

    virtual void addMemory(ProGe::Netlist& netlist);

    virtual bool generatesComponentHdlFile() const;

    virtual std::vector<std::string>
    generateComponentFile(std::string outputPath);

protected:
    
    virtual std::string moduleName() const;
    
    virtual std::string instanceName() const;
    
};

#endif
