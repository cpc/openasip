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
 * @file MemoryGenerator.hh
 *
 * Declaration of MemoryGenerator class.
 *
 * @author Otto Esko 2010 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_MEMORY_GENERATOR_HH
#define TTA_MEMORY_GENERATOR_HH

#include <iostream>
#include <string>
#include <vector>

class PlatformIntegrator;

enum MemType {
    UNKNOWN,
    NONE,
    VHDL_ARRAY,
    ONCHIP,
    SRAM,
    DRAM
};

#include "PlatformIntegrator.hh"

class MemoryGenerator {
public:

    MemoryGenerator(
        int memMauWidth,
        int widthInMaus,
        int addrWidth,
        std::string initFile,
        const PlatformIntegrator* integrator,
        std::ostream& warningStream,
        std::ostream& errorStream);

    virtual ~MemoryGenerator();

    /**
     * Tests that the memory generator is compatible with TTA core.
     * If incompatible, reasons are appended to the reasons vector
     *
     * @param ttaCore TTA toplevel signals
     * @param reasons Reasons why incompatible
     * @return is memory generator compatible with the TTA core
     */
    virtual bool isCompatible(
        const std::vector<std::string>& ttaCore,
        std::vector<std::string>& reasons) = 0;

    virtual void writeComponentDeclaration(std::ostream& stream) = 0;

    virtual void writeComponentInstantiation(
        const std::vector<std::string>& toplevelSignals,
        std::ostream& signalStream,
        std::ostream& signalConnections,
        std::ostream& toplevelInstantiation,
        std::ostream& memInstantiation) = 0;

    virtual bool generatesComponentHdlFile() const = 0;

    virtual std::vector<std::string>
    generateComponentFile(std::string outputPath) = 0;

    int memoryTotalWidth() const;
    
    int memoryMauSize() const;

    int memoryWidthInMaus() const;

    int memoryAddrWidth() const;

    std::string initializationFile() const;

protected:

    const PlatformIntegrator* platformIntegrator() const;

    std::ostream& warningStream();

    std::ostream& errorStream();

    /**
     * Tries to find signal which contains the given name. Returns the index
     * or negative number in case signal was not found. First occurance of the
     * name is returned.
     *
     * @param name Name to be searched
     * @param signals Vector of signals from where the signal is searched
     * @return Index to the vector where the name is found. Negative if not
     * found
     */
    int findSignal(
        const std::string& name,
        const std::vector<std::string>& signals);

private:
    int mauWidth_;
    int widthInMaus_;
    int addrWidth_;

    std::string initFile_;
    
    const PlatformIntegrator* integrator_;

    std::ostream& warningStream_;
    std::ostream& errorStream_;
};

#endif
