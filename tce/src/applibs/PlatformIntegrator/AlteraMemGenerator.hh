/*
    Copyright (c) 2002-2011 Tampere University.

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
 * @file AlteraMemGenerator.hh
 *
 * Declaration of AlteraMemGenerator class.
 *
 * @author Otto Esko 2011 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_ALTERA_MEM_GENERATOR_HH
#define TTA_ALTERA_MEM_GENERATOR_HH

#include <iostream>
#include <vector>
#include "MemoryGenerator.hh"
#include "TCEString.hh"

class PlatformIntegrator;

class AlteraMemGenerator : public MemoryGenerator {
public:

    AlteraMemGenerator(
        int memMauWidth,
        int widthInMaus,
        int addrWidth,
        TCEString initFile,
        const PlatformIntegrator* integrator,
        std::ostream& warningStream,
        std::ostream& errorStream);
    
    virtual ~AlteraMemGenerator();

    virtual void addMemory(
        const ProGe::NetlistBlock& ttaCore,
        ProGe::Netlist& netlist,
        int memIndex);
    
protected:

    std::vector<TCEString> instantiateAlteraTemplate(
        const TCEString& templateFile,
        const TCEString& outputPath) const;

    virtual void addGenerics(
        ProGe::NetlistBlock& topBlock,
        const TCEString& addrWidth,
        const TCEString& dataWidth,
        int memIndex);

    static const TCEString INIT_FILE_G;
    static const TCEString DEV_FAMILY_G;
    static const TCEString ADDRW_G;
    static const TCEString DATAW_G;
    
};

#endif
