/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file ProGeTestBenchGenerator.hh
 *
 * Declaration of ProGeTestBenchGenerator class.
 *
 * @author Esa Määttä 2007 (esa.maatta@tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROGE_TEST_BENCH_GENERATOR_HH
#define TTA_PROGE_TEST_BANCH_GENERATOR_HH

#include "Machine.hh"
#include "Exception.hh"
#include "MachineImplementation.hh"
#include "FUImplementationLocation.hh"

#include <string>

/**
 * Class for test bench generating objects.
 *
 * Base class for script generating.
 */
class ProGeTestBenchGenerator {
public:
    ProGeTestBenchGenerator();
    virtual ~ProGeTestBenchGenerator();

    void generate(
        const TTAMachine::Machine& mach,
        const IDF::MachineImplementation& implementation,
        const std::string& dstDirectory,
        const std::string& progeOutDir)
        throw (IOException, OutOfRange, InvalidName, InvalidData);

private:
    void copyTestBenchFiles(const std::string& dstDirectory);
    void createFile(const std::string& fileName)
        throw (IOException);
    std::string getSignalMapping(
        const std::string& fuName,
        const std::string& epName,
        bool widthIsOne,
        const std::string& memoryName,
        const std::string& memoryLine)
        throw (InvalidName);
    void createTBConstFile(
        std::string dstDirectory,
        const std::string dataWidth = "",
        const std::string addrWidth = "");
    void createProcArchVhdl(
        const std::string& dstDirectory,
        const std::string& topLevelVhdl,
        const std::string& signalMappings)
        throw (IOException);

};

#endif

