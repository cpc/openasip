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
 * @file ProGeScriptGenerator.cc
 *
 * Implementation of ProGeScriptGenerator class.
 *
 * @author Esa Määttä 2007 (esa.maatta-no.spam-tut.fi)
 * @note rating: red
 */

#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <fstream>

#include <boost/regex.hpp>

#include "ProGeScriptGenerator.hh"
#include "HDBManager.hh"
#include "CachedHDBManager.hh"
#include "HDBRegistry.hh"
#include "FileSystem.hh"
//#include "IDFSerializer.hh"

using std::string;
using std::endl;
using std::list;


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
    const std::string& dstDir,
    const std::string& progeOutDir,
    const std::string& testBenchDir) :
    dstDir_(dstDir), 
    progeOutDir_(progeOutDir),
    testBenchDir_(testBenchDir),
    workDir_("work"),
    vhdlDir_("vhdl"),
    gcuicDir_("gcu_ic"),
    modsimCompileScriptName_("modsim_compile.sh"),
    ghdlCompileScriptName_("ghdl_compile.sh"),
    modsimSimulateScriptName_("modsim_simulate.sh"),
    ghdlSimulateScriptName_("ghdl_simulate.sh"),
    testbenchName_("testbench") {
        
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
    generateGhdlCompile();

    generateModsimSimulate();
    generateGhdlSimulate();
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
    string program = "vcom";
    outputScriptCommands(stream, vhdlFiles_, program);

    stream << endl;
    outputScriptCommands(stream, gcuicFiles_, program);

    stream << endl;
    outputScriptCommands(stream, testBenchFiles_, program);

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
    outputScriptCommands(stream, vhdlFiles_, program);

    stream << endl;
    outputScriptCommands(stream, gcuicFiles_, program);

    stream << endl;
    outputScriptCommands(stream, testBenchFiles_, program);

    stream << endl;
    // compile command for ghdl
    stream << "ghdl -m --workdir=" << workDir_ 
           << " --ieee=synopsys -fexplicit " << testbenchName_ << endl;

    stream.close();
}


/** 
 * Generates a script for simulating using modelsims vsim. 
 */
void
ProGeScriptGenerator::generateModsimSimulate()
    throw (IOException) {

    string dstFile = dstDir_ + FileSystem::DIRECTORY_SEPARATOR +
        modsimCompileScriptName_;

    createExecutableFile(dstFile);

    std::ofstream stream(dstFile.c_str(), std::ofstream::out);
    generateStart(stream);

    stream << "vsim " << testbenchName_ << "  -c -do 'run; exit'" << endl;

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
           << " --assert-level=none --stop-time=52390ns"
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
 */
void
ProGeScriptGenerator::outputScriptCommands( 
    std::ostream& stream,
    const std::list<std::string>& files,
    const std::string& cmdPrefix) {

    list<string>::const_iterator iter = files.begin();
    while (iter != files.end()) {
        stream << cmdPrefix << " " << *iter++ << endl;
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
 * Sorts file in one list according to other list, only relative order matters
 * (which entry comes first). Algorithm used does relative sort between two
 * lists, the other is sorted according to the other. 
 * 
 * @param toSort List to be sorted. 
 * @param acSort List where reference order is taken from.
 */
void
ProGeScriptGenerator::sortFiles(
    std::list<std::string>& toSort,
    std::list<std::string>& acSort) {

    typedef std::list<std::string>::iterator listStrIt;

    listStrIt itAc1 = acSort.begin();
    listStrIt itAc2;
    listStrIt itTo;
    listStrIt itTo2;
    listStrIt itAcTest = --acSort.end();

    string temp;
    bool comp_itTo_itAc1;
    bool comp_itTo_itAc2;

    // Ok this algorithm is quite horrible..
    while (itAc1 != itAcTest) {
        (itAc2 = itAc1)++;
        while (itAc2 != acSort.end()) {
            // now check list to be sorted
            itTo = toSort.begin();
            while (itTo != toSort.end()) {
                comp_itTo_itAc1 = 
                    FileSystem::compareFileNames(*itTo, *itAc1, dstDir_);
                comp_itTo_itAc2 = 
                    FileSystem::compareFileNames(*itTo, *itAc2, dstDir_);
                if (comp_itTo_itAc1 || comp_itTo_itAc2) {
                    if (comp_itTo_itAc1) {
                        // order was ok!
                        break;
                    } else { // found itAc2 first
                        (itTo2 = itTo)++;
                        while (itTo2 != toSort.end()) {
                            if (FileSystem::compareFileNames(*itTo2, *itAc1,
                                dstDir_)) {
                                // now change itTo2 and itTo places                            
                                temp = *itTo;
                                *itTo = *itTo2;
                                *itTo2 = temp;
                                break;
                            }
                            ++itTo2;
                        }
                        break;
                    }
                }
                ++itTo;
            }
            ++itAc2;
        }
        ++itAc1;
    }
}


/** 
 * Gets compilation order for vhdl files from IDF/HDB files.
 * 
 * @param idfFile IDF file to open and search for all HDB files.
 * @param order List of file names is relative compilation order.
 */
void
ProGeScriptGenerator::getBlockOrder(
    const std::string& idfFile,
    std::list<std::string>& order) {

    // TODO: parse IDF file using IDFSerializer class if possible.
    // finds hdb files from IDF file
    string regex = "(.*>)(.*\\.hdb)(<.*)";
    list<string> hdbFiles;
    findText(regex, 2, idfFile, hdbFiles);

    // now remove duplicates from hdbfile list
    uniqueFileNames(hdbFiles, dstDir_);

    list<string>::iterator itl = hdbFiles.begin();
    list<string>::iterator it;
    HDB::CachedHDBManager* hdb = NULL;
    while (itl != hdbFiles.end())
    {
        try {
            hdb = &HDB::HDBRegistry::instance().hdb(*itl++);
        } catch (const Exception& e) {
            // TODO: throw a meaningful exception to stop script generation
            throw e;
        }   

        // append to order list
        list<string> blockSourceFiles = hdb->blockSourceFile();
        it = blockSourceFiles.begin();
        while (it != blockSourceFiles.end()) {
            order.push_back(*it++);
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
    string vhdlRegex = ".*\\.(vhd|vhdl|pkg)$";

    // generate relative paths
    bool absolutePaths = false;
    
    // getting files from project dir
    string dirName = progeOutDir_ + FileSystem::DIRECTORY_SEPARATOR 
        + vhdlDir_;
    if (FileSystem::fileIsDirectory(dirName)) {
        findFiles(vhdlRegex, 
            FileSystem::directoryContents(dirName, absolutePaths),
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

    list<string> rightOrder;
    rightOrder.push_back("opcodes_pkg.vhdl");
    rightOrder.push_back("decoder.vhdl");
    rightOrder.push_back("decompressor.vhdl");
    rightOrder.push_back("ifetch.vhdl");
    rightOrder.push_back("highest_pkg.vhdl");
    rightOrder.push_back("ic.vhdl");

    prefixStrings(rightOrder, string(progeOutDir_ + 
        FileSystem::DIRECTORY_SEPARATOR + gcuicDir_ + 
        FileSystem::DIRECTORY_SEPARATOR));
    
    gcuicFiles_.sort(); // is this necessary?
    sortFiles(gcuicFiles_, rightOrder); 

    // TODO: read hdb files from idf and sort according to them.
    // TODO: if not found throw exception.
    list<string> idfFiles;
    FileSystem::findFromDirectory(".*\\.idf$", dstDir_, idfFiles);

    list<string> vhdlRightOrder;
    vhdlRightOrder.push_back("globals_pkg.vhdl");
    vhdlRightOrder.push_back("toplevel_params_pkg.vhdl");
    if (!idfFiles.empty()) {
        getBlockOrder(idfFiles.front(), vhdlRightOrder);
        prefixStrings(vhdlRightOrder, 
            string(progeOutDir_ + 
                FileSystem::DIRECTORY_SEPARATOR + vhdlDir_ + 
                FileSystem::DIRECTORY_SEPARATOR));
        sortFiles(vhdlFiles_, vhdlRightOrder); 
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


