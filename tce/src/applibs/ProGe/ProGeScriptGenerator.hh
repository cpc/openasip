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
 * @file ProGeScriptGenerator.hh
 *
 * Declaration of ProGeScriptGenerator class.
 *
 * @author Esa Määttä 2007 (esa.maatta-no.spam-tut.fi)
 * @author Vinogradov Viacheslav(added Verilog generating) 2012 
 * @note rating: red
 */

#ifndef TTA_PROGE_SCRIPT_GENERATOR_HH
#define TTA_PROGE_SCRIPT_GENERATOR_HH

#include <list>
#include "Exception.hh"
#include "ProGeTypes.hh"

namespace IDF {
    class MachineImplementation;
}

/**
 * Class for script generating objects.
 *
 * Base class for script generating.
 */
class ProGeScriptGenerator {
public:
    ProGeScriptGenerator(
        const ProGe::HDL language,
        const IDF::MachineImplementation& idf,
        const std::string& dstDir,
        const std::string& progeOutDir,
        const std::string& sharedOutDir,
        const std::string& testBenchDir,
        const std::string& toplevelEntity);

    virtual ~ProGeScriptGenerator();

    void generateAll();

    void generateModsimCompile();

    void generateGhdlCompile();

    void generateIverilogCompile();

    void generateModsimSimulate();

    void generateGhdlSimulate();

    void generateIverilogSimulate();

private:
    void generateStart(
        std::ostream& stream);
    void createExecutableFile(const std::string& fileName);
    void outputScriptCommands(
        std::ostream& stream,
        const std::list<std::string>& files,
        const std::string& cmdPrefix,
        const std::string& cmdPostfix);
    template<typename T>
    void findFiles(
        const std::string& perlre,
        T files,
        std::list<std::string>& found);
    template<typename T>
    void findFiles(const std::string& perlre, T& files);
    void findText(
        const std::string& perlre,
        const unsigned int& matchRegion,
        const std::string& fileName,
        std::list<std::string>& found);
    void getBlockOrder(
        std::list<std::string>& order);
    void sortFilesFirst(
        std::list<std::string>& toSort,
        std::list<std::string>& acSort);
    void sortFilesLast(
        std::list<std::string>& toSort,
        std::list<std::string>& acSort);		
    template <typename CONT>
    void uniqueFileNames(
        CONT& files,
        const std::string& rootDirectory);
    void prefixStrings(
        std::list<std::string>& tlist, 
        const std::string& prefix,
        int start = 0,
        int end = -1);
    void fetchFiles();
    void prepareFiles();
    
    // destination directory where scripts are generated.
    std::string dstDir_;
    // directory where processor generators output vhdl files are
    std::string progeOutDir_;
    // directory where processor generators shared HDL files are
    std::string sharedOutDir_;
    // directories where to read test bench vhdl files.
    std::string testBenchDir_;

    // lists for file names
    std::list<std::string> vhdlFiles_;
    std::list<std::string> gcuicFiles_;
    std::list<std::string> testBenchFiles_;

    // directory used by ghdl/modelsim as work directory when compiling
    const std::string workDir_;
    const std::string vhdlDir_;
    const std::string verDir_;
    const std::string gcuicDir_;
    const std::string tbDir_;

    // file names for scripts to be generated
    const std::string modsimCompileScriptName_;
    const std::string ghdlCompileScriptName_;
    const std::string iverilogCompileScriptName_;
    const std::string modsimSimulateScriptName_;
    const std::string ghdlSimulateScriptName_;
    const std::string iverilogSimulateScriptName_;

    // test bench name 
    const std::string testbenchName_;
    const std::string toplevelEntity_;
    const IDF::MachineImplementation& idf_;
    const ProGe::HDL language_;
};

#include "ProGeScriptGenerator.icc"

#endif

