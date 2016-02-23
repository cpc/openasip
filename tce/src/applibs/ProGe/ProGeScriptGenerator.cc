/*
    Copyright (c) 2002-2011 Tampere University of Technology.

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
 * @author Esa M‰‰tt‰ 2007 (esa.maatta-no.spam-tut.fi)
 * @author Otto Esko 2008 (otto.esko-no.spam-tut.fi)
 * @author Pekka J‰‰skel‰inen 2011
 * @author Vinogradov Viacheslav(added Verilog generating) 2012 
 * @note rating: red
 */

#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <fstream>

#include "CompilerWarnings.hh"
IGNORE_COMPILER_WARNING("-Wkeyword-macro");
#include <boost/regex.hpp>
POP_COMPILER_DIAGS

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

const string MAGICAL_RUNTIME_CONSTANT = "52390ns";

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
    const ProGe::HDL language,
    const IDF::MachineImplementation& idf,
    const std::string& dstDir,
    const std::string& progeOutDir,
    const std::string& sharedOutDir,
    const std::string& testBenchDir,
    const std::string& toplevelEntity = "tta0") :
    dstDir_(dstDir), 
    progeOutDir_(progeOutDir),
    sharedOutDir_(sharedOutDir),
    testBenchDir_(testBenchDir),
    workDir_("work"),
    vhdlDir_("vhdl"),
    verDir_("verilog"),
    gcuicDir_("gcu_ic"),
    tbDir_("tb"),
    modsimCompileScriptName_("modsim_compile.sh"),
    ghdlCompileScriptName_("ghdl_compile.sh"),
    iverilogCompileScriptName_("iverilog_compile.sh"),
    modsimSimulateScriptName_("modsim_simulate.sh"),
    ghdlSimulateScriptName_("ghdl_simulate.sh"),
    iverilogSimulateScriptName_("iverilog_simulate.sh"),
    testbenchName_("testbench"),
    toplevelEntity_(toplevelEntity),
    idf_(idf),
    language_(language){

    fetchFiles();
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
ProGeScriptGenerator::generateAll()
    throw (IOException) {

    generateModsimCompile();
    
    if(language_==VHDL)
        generateGhdlCompile();
    else
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
ProGeScriptGenerator::generateModsimCompile()
    throw (IOException) {

    string dstFile = dstDir_ + FileSystem::DIRECTORY_SEPARATOR +
        modsimCompileScriptName_;

    createExecutableFile(dstFile);

    std::ofstream stream(dstFile.c_str(), std::ofstream::out);
    generateStart(stream);

    stream << "rm -rf " << workDir_ << endl;
    stream << "vlib " << workDir_ << endl;
    stream << "vmap"  << endl;

    stream << endl;
    string program =
    ((language_==VHDL)?"vcom":"vlog +incdir+verilog +incdir+gcu_ic +incdir+tb");
    outputScriptCommands(stream, vhdlFiles_, program,"");

    stream << endl;
    outputScriptCommands(stream, gcuicFiles_, program,"");

    stream << endl;
    outputScriptCommands(stream, testBenchFiles_, program,"");

    stream.close();
}


/** 
 * Generates a script for compilation using ghdl.
 *
 * @exception IOException 
 */
void
ProGeScriptGenerator::generateGhdlCompile()
    throw (IOException) {

    string dstFile = dstDir_ + FileSystem::DIRECTORY_SEPARATOR +
        ghdlCompileScriptName_;

    createExecutableFile(dstFile);

    std::ofstream stream(dstFile.c_str(), std::ofstream::out);
    generateStart(stream);

    stream << "rm -rf " << workDir_ << endl;
    stream << "mkdir -p work" << endl;
    stream << "rm -rf bus.dump" << endl;
    stream << "rm -rf " << testbenchName_ << endl;

    stream << endl;
    string program = "ghdl -i --workdir=" + workDir_;
    outputScriptCommands(stream, vhdlFiles_, program,"");

    stream << endl;
    outputScriptCommands(stream, gcuicFiles_, program,"");

    stream << endl;
    outputScriptCommands(stream, testBenchFiles_, program,"");

    stream << endl;
    // compile command for ghdl
    stream << "ghdl -m --workdir=" << workDir_ 
           << " --ieee=synopsys -fexplicit " << testbenchName_ << endl;

    stream.close();
}

/** 
 * Generates a script for compilation using iVerilog.
 *
 * @exception IOException 
 */
void
ProGeScriptGenerator::generateIverilogCompile()
    throw (IOException) {

    string dstFile = dstDir_ + FileSystem::DIRECTORY_SEPARATOR +
        iverilogCompileScriptName_;

    createExecutableFile(dstFile);

    std::ofstream stream(dstFile.c_str(), std::ofstream::out);
    generateStart(stream);

    stream << "rm -rf " << testbenchName_ << endl
           << endl
           << "iverilog -g2001 -D _IVERILOG_ "
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
ProGeScriptGenerator::generateModsimSimulate()
    throw (IOException) {

    string dstFile = dstDir_ + FileSystem::DIRECTORY_SEPARATOR +
        modsimSimulateScriptName_;

    createExecutableFile(dstFile);

    std::ofstream stream(dstFile.c_str(), std::ofstream::out);
    generateStart(stream);

    stream << "vsim " << testbenchName_ << "  -c -do 'run " 
           << MAGICAL_RUNTIME_CONSTANT << "; exit'" << endl;

    stream.close();
}


/** 
 * Generates a script for simulating using ghdl.
 *
 * @exception IOException 
 */
void
ProGeScriptGenerator::generateGhdlSimulate()
    throw (IOException) {

    string dstFile = dstDir_ + FileSystem::DIRECTORY_SEPARATOR +
        ghdlSimulateScriptName_;

    createExecutableFile(dstFile);

    std::ofstream stream(dstFile.c_str(), std::ofstream::out);
    generateStart(stream);

    stream << "./" << testbenchName_ 
           << " --assert-level=none --stop-time="
           << MAGICAL_RUNTIME_CONSTANT
           << endl;

    stream.close();
}

/** 
 * Generates a script for simulating using iVerilog.
 *
 * @exception IOException 
 */
void
ProGeScriptGenerator::generateIverilogSimulate()
    throw (IOException) {

    string dstFile = dstDir_ + FileSystem::DIRECTORY_SEPARATOR +
        iverilogSimulateScriptName_;

    createExecutableFile(dstFile);

    std::ofstream stream(dstFile.c_str(), std::ofstream::out);
    generateStart(stream);

    stream << "./" << testbenchName_ 
           << " --assert-level=none --stop-time="
           << MAGICAL_RUNTIME_CONSTANT
           << endl;

    stream.close();
}

/**
 * Creates a script file given as parameter and sets permissions.
 *
 * @param Name of the script file to be created
 * @exception IOException
 */
void
ProGeScriptGenerator::createExecutableFile(const std::string& fileName)
    throw (IOException) {

    FileSystem::removeFileOrDirectory(fileName);
    bool isCreated = FileSystem::createFile(fileName);
    if (!isCreated) {
        string errorMsg = "Unable to create file " + fileName;
        throw IOException(__FILE__, __LINE__, __func__, errorMsg);
    }
    FileSystem::setFileExecutable(fileName);
}


/** 
 * Generates the start of the shell script.
 * 
 * @param stream Stream where output is put.
 */
void 
ProGeScriptGenerator::generateStart(std::ostream& stream) {
    stream << "#! /bin/sh" << endl;
    stream << "# This script was automatically generated." << endl << endl;
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
        stream << cmdPrefix << " " << *iter++ << cmdPostfix << endl;
    }
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
            string imemMauPkg = vhdlDir_ + FileSystem::DIRECTORY_SEPARATOR 
                + toplevelEntity_ + "_imem_mau_pkg.vhdl";
            vhdlFiles_.push_back(imemMauPkg);
        }
    }
    std::string sharedDir = 
        sharedOutDir_ + FileSystem::DIRECTORY_SEPARATOR +
        ((language_==VHDL)?vhdlDir_:verDir_);
        
    if (sharedDir != dirName && FileSystem::fileIsDirectory(sharedDir)) {
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
        vhdlFirstOrder.push_back("globals_pkg.vhdl");
        vhdlFirstOrder.push_back("tce_util_pkg.vhdl");
        string paramsPkg = toplevelEntity_ + "_params_pkg.vhdl";
        vhdlFirstOrder.push_back(paramsPkg);
        vhdlFirstOrder.push_back("imem_mau_pkg.vhdl");
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
        testBenchLastOrder.push_back("testbench_constants_pkg.vhdl");
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
}
