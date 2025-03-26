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
 * @file ProGeScriptGenerator.cc
 *
 * Implementation of ProGeScriptGenerator class.
 *
 * @author Esa M��tt� 2007 (esa.maatta-no.spam-tut.fi)
 * @author Otto Esko 2008 (otto.esko-no.spam-tut.fi)
 * @author Pekka J��skel�inen 2011
 * @author Vinogradov Viacheslav(added Verilog generating) 2012 
 * @note rating: red
 */

#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <fstream>
#include <algorithm>
#include <utility>

#include "CompilerWarnings.hh"
IGNORE_CLANG_WARNING("-Wkeyword-macro")
#include <boost/regex.hpp>
POP_CLANG_DIAGS

#include "ProGeScriptGenerator.hh"
#include "HDBManager.hh"
#include "CachedHDBManager.hh"
#include "HDBRegistry.hh"
#include "FileSystem.hh"
#include "MachineImplementation.hh"
#include "UnitImplementationLocation.hh"
#include "FUEntry.hh"
#include "FUImplementation.hh"
#include "RFEntry.hh"
#include "RFImplementation.hh"

using namespace ProGe;

using IDF::MachineImplementation;
using IDF::FUImplementationLocation;
using IDF::RFImplementationLocation;
using std::string;
using std::endl;
using std::list;

// Default HDL simulation length unless another is given
const string MAGICAL_RUNTIME_CONSTANT = "52390";

/**
 * The constructor.
 *
 * Script generating needs a IDF file and hdb files mentioned there. Working
 * directory is assumed to be the destination directory for script files.
 *
 * @param dstDir Directory where to generate scripts.
 * @param progeOutDir Directory where ProGes output vhdl files lie.
 * @param testBenchDir Directory where a test bench files are located.
 * @param projectRoot Directory that is project root, needed if relative dirs
 * wanted to generated scripts. Useful if other dirs given are under this
 * directory.
 */
ProGeScriptGenerator::ProGeScriptGenerator(
    const ProGe::HDL language, const IDF::MachineImplementation& idf,
    const std::string& dstDir, const std::string& progeOutDir,
    const std::string& sharedOutDir, const std::string& testBenchDir,
    const std::string& toplevelEntity = "tta0",
    const std::string& simulationRuntime)
    : dstDir_(dstDir),
      progeOutDir_(progeOutDir),
      sharedOutDir_(sharedOutDir),
      testBenchDir_(testBenchDir),
      workDir_("work"),
      vhdlDir_("vhdl"),
      verDir_("verilog"),
      gcuicDir_("gcu_ic"),
      tbDir_("tb"),
      platformDir_("platform"),
      modsimCompileScriptName_("modsim_compile.sh"),
      ghdlCompileScriptName_("ghdl_compile.sh"),
      iverilogCompileScriptName_("iverilog_compile.sh"),
      modsimSimulateScriptName_("modsim_simulate.sh"),
      ghdlSimulateScriptName_("ghdl_simulate.sh"),
      iverilogSimulateScriptName_("iverilog_simulate.sh"),
      ghdlPlatformCompileScriptName_("ghdl_platform_compile.sh"),
      testbenchName_("testbench"),
      platformTestbenchName_("tta_almaif_tb"),
      toplevelEntity_(toplevelEntity),
      idf_(idf),
      language_(language),
      simulationRuntime_(simulationRuntime) {
    fetchFiles();
    packageFilesFirst();
    prepareFiles();
}

/**
 * The destructor.
 */
ProGeScriptGenerator::~ProGeScriptGenerator() {

}


/** 
 * Generates all scripts to destination dir (dstDir_).
 *
 * @exception IOException 
 */
void
ProGeScriptGenerator::generateAll() {
    generateModsimCompile();

    if (language_ == VHDL) {
        generateGhdlCompile(
            ghdlCompileScriptName_, testbenchName_,
            {vhdlFiles_, gcuicFiles_, testBenchFiles_}, true);
        if (!platformFiles_.empty()) {
            generateGhdlCompile(
                ghdlPlatformCompileScriptName_, platformTestbenchName_,
                {platformFiles_}, false);
        }
    } else
        generateIverilogCompile();

    generateModsimSimulate();
    
    if(language_==VHDL)
        generateGhdlSimulate();
    else
        generateIverilogSimulate();
}

/** 
 * Generates a script for compilation using modelsim.
 *
 * @exception IOException 
 */
void
ProGeScriptGenerator::generateModsimCompile() {
    string dstFile = dstDir_ + FileSystem::DIRECTORY_SEPARATOR +
        modsimCompileScriptName_;
    const string coverageOpt = "+cover=sbcet";

    createExecutableFile(dstFile);

    std::ofstream stream(dstFile.c_str(), std::ofstream::out);
    generateCompileStart(stream);

    stream << "rm -rf " << workDir_ << endl;
    stream << "vlib " << workDir_ << endl;
    stream << "vmap"  << endl;

    stream << "if [ \"$only_add_files\" = \"yes\" ]; then" << endl;
    stream << "    echo \"-a option is not available for modelsim.\"; exit 2;"
           << endl;
    stream << "fi" << endl;

    stream << "if [ \"$enable_coverage\" = \"yes\" ]; then" << endl;
    stream << "    coverage_opt=\"" << coverageOpt << "\"" << endl;
    stream << "fi" << endl;

    stream << endl;
    string coverageOptAssign = " $coverage_opt";
    string program =
    ((language_==VHDL)?
        "vcom": "vlog -sv +define+SIMTIME=" + simulationRuntime_ +
        " +incdir+verilog +incdir+gcu_ic +incdir+tb");
    string exitOnFailure = "|| exit 1";
    string checkSynthesisFLag = (language_==VHDL)?" -check_synthesis":"";

    for (const std::string& file : vhdlFiles_) {
        if (file.find("_pkg.") != std::string::npos) {
            // Do not add coverage option for package files since unused
            // functions in the packages are not excluded and thus spoils
            // code coverage.
            outputScriptCommand(stream, file, program, exitOnFailure);
        } else {
            outputScriptCommand(
                stream, file, program + coverageOptAssign + checkSynthesisFLag,
                exitOnFailure);
        }
    }

    stream << endl;
    for (const std::string& file : gcuicFiles_) {
        if (file.find("_pkg.") != std::string::npos) {
            // Do not add coverage option for package files since unused
            // functions in the packages are not excluded and thus spoils
            // code coverage.
            outputScriptCommand(stream, file,
                program + checkSynthesisFLag, exitOnFailure);
        } else {
            outputScriptCommand(
                stream, file, program + coverageOptAssign + checkSynthesisFLag,
                exitOnFailure);
        }
    }

    stream << endl;
    outputScriptCommands(stream, testBenchFiles_, program, exitOnFailure);

    stream << "exit 0" << endl;
    stream.close();
}

/** 
 * Generates a script for compilation using ghdl.
 *
 * @exception IOException 
 */
void
ProGeScriptGenerator::generateGhdlCompile(
    std::string scriptName, std::string tbName,
    std::vector<std::list<std::string>> filesToCompile,
    bool clearWorkingDir) {
    string dstFile = dstDir_ + FileSystem::DIRECTORY_SEPARATOR + scriptName;

    createExecutableFile(dstFile);

    std::ofstream stream(dstFile.c_str(), std::ofstream::out);
    generateCompileStart(stream);

    if (clearWorkingDir) {
        stream << "rm -rf " << workDir_ << endl;
        stream << "mkdir -p work" << endl;
        stream << "rm -rf bus.dump" << endl;
        stream << "rm -rf " << tbName << endl;
    }

    stream << "if [ \"$enable_coverage\" = \"yes\" ]; then" << endl;
    stream << "    echo \"-c option is not available for ghdl.\"; exit 2;"
           << endl;
    stream << "fi" << endl;

    stream << endl;
    string program = "ghdl -i ${std_version} --workdir=" + workDir_;
    string exitOnFailure = " || exit 1";

    for (auto& files : filesToCompile) {
        outputScriptCommands(stream, files, program, exitOnFailure);
        stream << endl;
    }
    // compile command for ghdl
    stream << "if [ \"$only_add_files\" = \"no\" ]; then" << endl;
    stream << "    ghdl -m ${std_version} -Wno-hide --workdir=" << workDir_
           << " --ieee=synopsys -fexplicit " << tbName << endl;
    stream << "fi" << endl;

    stream << "exit 0" << endl;
    stream.close();
}

/** 
 * Generates a script for compilation using iVerilog.
 *
 * @exception IOException 
 */
void
ProGeScriptGenerator::generateIverilogCompile() {
    string dstFile = dstDir_ + FileSystem::DIRECTORY_SEPARATOR +
        iverilogCompileScriptName_;

    createExecutableFile(dstFile);

    std::ofstream stream(dstFile.c_str(), std::ofstream::out);
    generateCompileStart(stream);

    stream << "if [ \"$enable_coverage\" = \"yes\" ]; then" << endl;
    stream << "    echo \"-c option is not available for iverilog.\"; exit 2;"
           << endl;
    stream << "fi" << endl;

    stream << "rm -rf " << testbenchName_ << endl
           << endl
           << "iverilog -g2012 -D _IVERILOG_ "
           << "-D SIMTIME=" << simulationRuntime_ << " "
           << "-Itb -Iverilog -Igcu_ic ";
    outputScriptCommands(stream, vhdlFiles_, ""," \\");
    outputScriptCommands(stream, gcuicFiles_, ""," \\");
    outputScriptCommands(stream, testBenchFiles_, ""," \\");

    stream << "-s " << testbenchName_ << " \\" << endl;
    stream << "-o " << testbenchName_ << endl;
    stream.close();
}

/** 
 * Generates a script for simulating using modelsims vsim. 
 */
void
ProGeScriptGenerator::generateModsimSimulate() {
    string dstFile = dstDir_ + FileSystem::DIRECTORY_SEPARATOR +
        modsimSimulateScriptName_;

    createExecutableFile(dstFile);

    std::ofstream stream(dstFile.c_str(), std::ofstream::out);
    generateSimulationStart(stream);

    stream << "master_coverage_db=accumulated_coverage.ucdb" << endl;
    stream << "coverage_db=cov000.ucdb" << endl;
    stream << "res_opt=\"-t $sim_res\"";
    stream << endl;
    stream << "coverage_opt=\"\"" << endl;
    stream << "if [ \"$enable_coverage\" = \"yes\" ]; then" << endl;
    stream << "    coverage_opt=\"-coverage\"" << endl;
    stream << "    do_script=\"" << "coverage save -onexit ${coverage_db}; "
           << "run ${runtime} ns; exit" << "\"" << endl;
    stream << "else" << endl;
    stream << "    do_script=\"" << "run ${runtime} ns; exit" << "\"" << endl;
    stream << "fi" << endl;
    stream << endl;
    stream << "vsim " << testbenchName_ << " $res_opt -c $coverage_opt"
           << " -do \"$do_script\"" << endl;
    stream << endl;
    stream << "# merge produced code coverage data into master database."
           << endl;
    stream << "if [ \"$enable_coverage\" = \"yes\" ]; then" << endl;
    stream << "    vcover merge $master_coverage_db "
           << "$master_coverage_db $coverage_db > /dev/null 2>&1" << endl;
    stream << "fi" << endl;

    stream.close();
}

/** 
 * Generates a script for simulating using ghdl.
 *
 * @exception IOException 
 */
void
ProGeScriptGenerator::generateGhdlSimulate() {
    string dstFile = dstDir_ + FileSystem::DIRECTORY_SEPARATOR +
        ghdlSimulateScriptName_;
    createExecutableFile(dstFile);

    std::ofstream stream(dstFile.c_str(), std::ofstream::out);
    generateSimulationStart(stream);

    stream << "if [ \"$enable_coverage\" = \"yes\" ]; then" << endl;
    stream << "    echo \"-c option is not available for ghdl.\"; exit 2;"
           << endl;
    stream << "fi" << endl;
    stream << "if [ -e ${tb_entity} ]; then" << endl
           << "    ./${tb_entity}"
           << " --stop-time=${runtime}ns" << endl
           << "else" << endl
           << "    # Newer GHDL versions does not produce binary." << endl
           << "    ghdl -r ${std_version} --workdir=work --ieee=synopsys "
           << "${tb_entity} --stop-time=${runtime}ns "
           << "--ieee-asserts=disable-at-0 ${generic_list}" << endl
           << "fi" << endl;

    stream.close();
}

/** 
 * Generates a script for simulating using iVerilog.
 *
 * @exception IOException 
 */
void
ProGeScriptGenerator::generateIverilogSimulate() {
    string dstFile = dstDir_ + FileSystem::DIRECTORY_SEPARATOR +
        iverilogSimulateScriptName_;

    createExecutableFile(dstFile);

    std::ofstream stream(dstFile.c_str(), std::ofstream::out);
    generateSimulationStart(stream);

    stream << "if [ \"$enable_coverage\" = \"yes\" ]; then" << endl;
    stream << "    echo \"-c option is not available for iverilog.\"; exit 2;"
           << endl;
    stream << "fi" << endl;

    stream << "./" << testbenchName_
           << " --assert-level=none" << endl;

    stream.close();
}

/**
 * Creates a script file given as parameter and sets permissions.
 *
 * @param Name of the script file to be created
 * @exception IOException
 */
void
ProGeScriptGenerator::createExecutableFile(const std::string& fileName) {
    FileSystem::removeFileOrDirectory(fileName);
    bool isCreated = FileSystem::createFile(fileName);
    if (!isCreated) {
        string errorMsg = "Unable to create file " + fileName;
        throw IOException(__FILE__, __LINE__, __func__, errorMsg);
    }
    FileSystem::setFileExecutable(fileName);
}

/**
 * Generates the start of the shell script for compilation script.
 *
 * @param stream Stream where output is put.
 */
void
ProGeScriptGenerator::generateCompileStart(std::ostream& stream) {
    stream << "#!/bin/bash" << endl;
    stream << "# This script was automatically generated." << endl << endl;

    // Copy common compile start from template
    static const string separator(FileSystem::DIRECTORY_SEPARATOR);
    const string scriptTmpl = Environment::dataDirPath("ProGe") +
        separator + "tb" + separator + "compile.sh.tmpl";

    std::ifstream scriptTmplIn(scriptTmpl.c_str());
    stream << scriptTmplIn.rdbuf();
}

/** 
 * Generates the start of the shell script for simulation script.
 * 
 * The script includes option parsing for simulation controls.
 * Shell variable \"runtime\" holds default simulation time set by this class
 * or user defined time by option.
 *
 * @param stream Stream where output is put.
 */
void 
ProGeScriptGenerator::generateSimulationStart(std::ostream& stream) {
    stream << "#!/bin/bash" << endl;
    stream << "# This script was automatically generated." << endl << endl;
    stream << "DEFAULT_RUN_TIME=" << simulationRuntime_ << endl << endl;

    // Copy rest of the shell script start from template
    static const string separator(FileSystem::DIRECTORY_SEPARATOR);
    const string scriptTmpl = Environment::dataDirPath("ProGe") +
        separator + "tb" + separator + "simulate.sh.tmpl";

    std::ifstream scriptTmplIn(scriptTmpl.c_str());
    stream << scriptTmplIn.rdbuf();
}


/** 
 * Outputs shell commands to stream.
 *
 * Creates script commands using list of files and command prefix and outputs
 * them to the given stream.
 * 
 * @param stream Output stream.
 * @param files List of filenames to use. 
 * @param cmdPrefix Prefix command.
 * @param cmdPostfix Prefix command.
 */
void
ProGeScriptGenerator::outputScriptCommands( 
    std::ostream& stream,
    const std::list<std::string>& files,
    const std::string& cmdPrefix,
    const std::string& cmdPostfix) {

    list<string>::const_iterator iter = files.begin();
    while (iter != files.end()) {
        outputScriptCommand(stream, *iter++, cmdPrefix, cmdPostfix);
    }
}

/**
 * Outputs shell command to stream.
 *
 * Creates script commands using a file and command prefix and outputs
 * them to the given stream.
 *
 * @param stream Output stream.
 * @param files List of filenames to use.
 * @param cmdPrefix Prefix command.
 * @param cmdPostfix Postfix command.
 */
void
ProGeScriptGenerator::outputScriptCommand(
    std::ostream& stream,
    const std::string& file,
    const std::string& cmdPrefix,
    const std::string& cmdPostfix) {

    stream << cmdPrefix << " " << file << " " << cmdPostfix << endl;
}


/** 
 * Regex find from a file. 
 *
 * Finds text matching the given regex from file by line at a time.
 * Case is ignored when interpreting the regex.
 * 
 * @param perlre Perl syntax regular expression.
 * @param matchRegion Region from the match appended to output list.
 * @param fileName Name and path of file name to be opened and read.
 * @param found List where matches are appended.
 */
void
ProGeScriptGenerator::findText( 
    const std::string& perlre,
    const unsigned int& matchRegion,
    const std::string& fileName,
    std::list<std::string>& found) {

    const int LINESIZE = 1000;
    
    const boost::regex re(perlre, 
            boost::regex::perl|boost::regex::icase);
    
    char line[LINESIZE];
    string::const_iterator begin;
    string::const_iterator end;
    string stemp;
    std::ifstream ifs( fileName.c_str() , std::ifstream::in );
    boost::match_results<string::const_iterator> matches;
    while (ifs.good()) {

        ifs.getline(line, LINESIZE-1);
        stemp = string(line);

        begin = stemp.begin();
        end = stemp.end();

        if (boost::regex_search(begin, end, matches, re)) {
            found.push_back(string(matches[matchRegion].first, 
                matches[matchRegion].second));
        }
    }
    ifs.close();
}

 
/** 
 * Relative file name/path sort using a reference.
 *
 * Sorts file in one list according to other list,placing in beginning of
 * list, only relative order matters (which entry comes first). Algorithm
 * used does relative sort between two lists, the other is sorted according
 * to the other. 
 * 
 * @param toSort List to be sorted. 
 * @param acSort List where reference order is taken from.
 */
void
ProGeScriptGenerator::sortFilesFirst(
    std::list<std::string>& toSort,
    std::list<std::string>& acSort) {

    typedef std::list<std::string>::iterator listStrIt;

    listStrIt itAc1 = acSort.begin();
    listStrIt itTo  = toSort.begin();
    listStrIt itTo2;

    while (itAc1 != acSort.end()) {
        // now check list to be sorted
        bool swapped = false;
        itTo2 = itTo;
        while (itTo2 != toSort.end()) {
            if (FileSystem::compareFileNames(*itTo2, *itAc1, dstDir_)) {
                // now change itTo2 and itTo places                            
                string temp = *itTo;
                *itTo = *itTo2;
                *itTo2 = temp;
                swapped = true;
                break;
                }
            ++itTo2;
        }
        if (swapped) {
            ++itTo;
        }
        ++itAc1;
    }
}


/** 
 * Relative file name/path sort using a reference.
 *
 * Sorts file in one list according to other list, placing in end of list,
 * only relative order matters (which entry comes first). Algorithm used
 * does relative sort between two lists, the other is sorted according to
 * the other. 
 * 
 * @param toSort List to be sorted. 
 * @param acSort List where reference order is taken from.
 */
void
ProGeScriptGenerator::sortFilesLast(
    std::list<std::string>& toSort,
    std::list<std::string>& acSort) {
    typedef std::list<std::string>::iterator listStrIt;
    typedef std::list<std::string>::reverse_iterator  rlistStrIt;

    listStrIt itAc1 = acSort.begin();
    rlistStrIt itTo    = toSort.rbegin();
    rlistStrIt itTo2;

    while (itAc1 != acSort.end()) {
        // now check list to be sorted
        bool swapped = false;
        itTo2 = itTo;
        while (itTo2 != toSort.rend()) {
            if (FileSystem::compareFileNames(*itTo2, *itAc1,dstDir_)) {
                // now change itTo2 and itTo places                            
                string temp  = *itTo;
                *itTo = *itTo2;
                *itTo2 = temp;
                swapped = true;
                break;
                }
            ++itTo2;
        }
        if (swapped) {
            ++itTo;
        }
        ++itAc1;
    }
}

/** 
 * Gets compilation order for vhdl files from IDF/HDB files.
 * 
 * @param order List of file names is relative compilation order.
 */
void
ProGeScriptGenerator::getBlockOrder(std::list<std::string>& order) {

    std::set<string> uniqueFiles;    

    // FU implementation HDL files
    for (int i = 0; i < idf_.fuImplementationCount(); i++) {
        const FUImplementationLocation& fuLoc = idf_.fuImplementation(i);
        string hdbFile = fuLoc.hdbFile();
        HDB::CachedHDBManager& hdb =
            HDB::HDBRegistry::instance().hdb(hdbFile);
        HDB::FUEntry* fu = hdb.fuByEntryID(fuLoc.id());
        const HDB::FUImplementation& fuImpl = fu->implementation();
        for (int j = 0; j < fuImpl.implementationFileCount(); j++) {
            string file = FileSystem::fileOfPath(fuImpl.file(j).pathToFile());
            if (uniqueFiles.find(file) == uniqueFiles.end()) {
                order.push_back(file);
                uniqueFiles.insert(file);
            }
        }
    }

    // RF implementation HDL files
    for (int i = 0; i < idf_.rfImplementationCount(); i++) {
        const RFImplementationLocation& rfLoc = idf_.rfImplementation(i);
        string hdbFile = rfLoc.hdbFile();
        HDB::CachedHDBManager& hdb =
            HDB::HDBRegistry::instance().hdb(hdbFile);
        HDB::RFEntry* rf = hdb.rfByEntryID(rfLoc.id());
        const HDB::RFImplementation& rfImpl = rf->implementation();
        for (int j = 0; j < rfImpl.implementationFileCount(); j++) {
            string file = FileSystem::fileOfPath(rfImpl.file(j).pathToFile());
            if (uniqueFiles.find(file) == uniqueFiles.end()) {
                order.push_back(file);
                uniqueFiles.insert(file);
            }
        }
    }
}


/** 
 * Prefixes strings in a container with a string within a range.
 * 
 * @param tlist Container of strings.
 * @param prefix Prefix to be added to strings in container. 
 * @param start Starting location in a container, 0 is the first.
 * @param end Ending location in a container, size-1 is the last.
 */
void
ProGeScriptGenerator::prefixStrings(
    std::list<std::string>& tlist, 
    const std::string& prefix,
    int start,
    int end) {
    
    if (end == -1 || end >= static_cast<int>(tlist.size())) {
        end = tlist.size() - 1;
    }

    list<string>::iterator itl = tlist.begin();
    for (int c = 0; c <= end; ++c, ++itl) {
        if (c >= start) {
            *itl = prefix + *itl; 
        }
    }
}


/** 
 * Gets file names from project directory.
 */
void
ProGeScriptGenerator::fetchFiles() {
    
    // files that match are accepted
    string vhdlRegex =
    ((language_==VHDL)?".*\\.(vhd|vhdl|pkg)$":".*\\.(v)$");

    // generate relative paths
    bool absolutePaths = false;
    
    // getting files from project dir
    string dirName = progeOutDir_ + FileSystem::DIRECTORY_SEPARATOR 
        + ((language_==VHDL)?vhdlDir_:verDir_);
    if (FileSystem::fileIsDirectory(dirName)) {
        findFiles(vhdlRegex, 
            FileSystem::directoryContents(dirName, absolutePaths),
            vhdlFiles_);
        // add the toplevelEntity + _imem_mau_pkg.vhdl to vhdlFiles_.
        // It is generated by PIG so it is not yet present.
        if(language_==VHDL){
            string imemMauPkg = dirName + FileSystem::DIRECTORY_SEPARATOR
                + toplevelEntity_ + "_imem_mau_pkg.vhdl";
            vhdlFiles_.push_back(imemMauPkg);
        }
    }
    std::string DS = FileSystem::DIRECTORY_SEPARATOR;

    std::string sharedDir = 
        sharedOutDir_ + FileSystem::DIRECTORY_SEPARATOR +
        ((language_==VHDL)?vhdlDir_:verDir_);
        
    if (sharedDir != FileSystem::absolutePathOf(dirName)
        && FileSystem::fileIsDirectory(sharedDir)) {
        findFiles(
            vhdlRegex, 
            FileSystem::directoryContents(sharedDir, absolutePaths),
            vhdlFiles_);
    }    

    dirName = progeOutDir_ + FileSystem::DIRECTORY_SEPARATOR + gcuicDir_;
    if (FileSystem::fileIsDirectory(dirName)) {
        findFiles(vhdlRegex, 
            FileSystem::directoryContents(dirName, absolutePaths),
            gcuicFiles_);
    }

    if (FileSystem::fileIsDirectory(testBenchDir_)) {
        FileSystem::findFromDirectoryRecursive(vhdlRegex, testBenchDir_,
            testBenchFiles_);
    } else {
        findFiles(vhdlRegex, 
            FileSystem::directoryContents(dstDir_, absolutePaths),
            testBenchFiles_);
    }

    dirName = progeOutDir_ + DS + platformDir_;
    if (FileSystem::fileIsDirectory(dirName)) {
        findFiles(
            vhdlRegex, FileSystem::directoryContents(dirName, absolutePaths),
            platformFiles_);
    }
}

/**
 * Reorders filename lists by arranging packages (*_pkg*) to the beginning of
 * the lists.
 */
void
ProGeScriptGenerator::packageFilesFirst() {
    auto packageComp = [](const string& str1, const string& str2) -> bool {
        bool str1IsPkg = (str1.find("_pkg") != string::npos);
        bool str2IsPkg = (str2.find("_pkg") != string::npos);
        if (str1IsPkg && !str2IsPkg) {
            return true;
        } else if (!str1IsPkg && str2IsPkg) {
            return false;
        } else {
            return str1 < str2;
        }
    };

    vhdlFiles_.sort(packageComp);
    gcuicFiles_.sort(packageComp);
    testBenchFiles_.sort(packageComp);
}

/** 
 * Prepares filename lists, generally sorts them.
 */
void
ProGeScriptGenerator::prepareFiles() {

    const string DS = FileSystem::DIRECTORY_SEPARATOR;
    if(language_==VHDL){
        string gcuIcDirName = progeOutDir_ + DS + gcuicDir_ + DS;
        list<string> gcuicFirstOrder;
        gcuicFirstOrder.push_back("gcu_opcodes_pkg.vhdl");
        prefixStrings(gcuicFirstOrder, gcuIcDirName);
        sortFilesFirst(gcuicFiles_, gcuicFirstOrder); 

        list<string> gcuicLastOrder;
        gcuicLastOrder.push_back("ic.vhdl");
        prefixStrings(gcuicLastOrder, gcuIcDirName);
        sortFilesLast(gcuicFiles_, gcuicLastOrder);

        string vhdlDirName = progeOutDir_ + DS + vhdlDir_ + DS;
        list<string> vhdlFirstOrder;
        vhdlFirstOrder.push_back("tce_util_pkg.vhdl");
        vhdlFirstOrder.push_back(toplevelEntity_ + "_imem_mau_pkg.vhdl");
        vhdlFirstOrder.push_back(toplevelEntity_ + "_globals_pkg.vhdl");
        vhdlFirstOrder.push_back(toplevelEntity_ + "_params_pkg.vhdl");
        // add FU and RF files in correct order
        getBlockOrder(vhdlFirstOrder);
        prefixStrings(vhdlFirstOrder, vhdlDirName);
        sortFilesFirst(vhdlFiles_, vhdlFirstOrder);
        
        list<string> vhdlLastOrder;
        string toplevelFile = toplevelEntity_ + ".vhdl";
        vhdlLastOrder.push_back(toplevelFile);
        prefixStrings(vhdlLastOrder, vhdlDirName);
        sortFilesLast(vhdlFiles_, vhdlLastOrder);
        
        string tbDirName = progeOutDir_ + DS + tbDir_ + DS;
        list<string> testBenchLastOrder;
        testBenchLastOrder.push_back("testbench_cfg.vhdl");
        testBenchLastOrder.push_back("testbench.vhdl");
        testBenchLastOrder.push_back("proc_arch.vhdl");
        testBenchLastOrder.push_back("proc_ent.vhdl");
        prefixStrings(testBenchLastOrder, tbDirName);
        sortFilesLast(testBenchFiles_, testBenchLastOrder);
    } else {
    //nothing to do here
    }
    // make dirs relative to dstDir_
    list<string>::iterator itl;
    itl = vhdlFiles_.begin();
    while (itl != vhdlFiles_.end()) {
        FileSystem::relativeDir(dstDir_, *itl++);
    }
    itl = gcuicFiles_.begin();
    while (itl != gcuicFiles_.end()) {
        FileSystem::relativeDir(dstDir_, *itl++);
    }
    itl = testBenchFiles_.begin();
    while (itl != testBenchFiles_.end()) {
        FileSystem::relativeDir(dstDir_, *itl++);
    }
    itl = platformFiles_.begin();
    while (itl != platformFiles_.end()) {
        FileSystem::relativeDir(dstDir_, *itl++);
    }
}
