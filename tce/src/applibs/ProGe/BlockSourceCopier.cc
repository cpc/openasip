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
 * @file BlockSourceCopier.cc
 *
 * Implementation of BlockSourceCopier class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @author Pekka J‰‰skel‰inen 2011
 * @author Vinogradov Viacheslav(added Verilog generating) 2012
 * @note rating: red
 */

#include <string>
#include <vector>
#include <fstream>
#include <boost/format.hpp>

#include "BlockSourceCopier.hh"
#include "MachineImplementation.hh"
#include "HDBManager.hh"
#include "HDBRegistry.hh"
#include "FUEntry.hh"
#include "RFEntry.hh"
#include "FUImplementation.hh"
#include "RFImplementation.hh"
#include "FileSystem.hh"
#include "AssocTools.hh"
#include "HDLTemplateInstantiator.hh"


using namespace IDF;
using namespace HDB;
using std::string;
using std::vector;

static const std::string UTILITY_VHDL_FILE = "tce_util_pkg.vhdl";
static const std::string UTILITY_VERILOG_FILE = "tce_util_pkg.vh";

namespace ProGe {

/**
 * The constructor.
 */
BlockSourceCopier::BlockSourceCopier(
    const IDF::MachineImplementation& implementation,
    TCEString entityStr,
    const HDL language):
    implementation_(implementation), entityStr_(entityStr), language_(language){
}


/**
 * The destructor.
 */
BlockSourceCopier::~BlockSourceCopier() {
}


/**
 * Copies the block definition files of the blocks given in IDF to
 * proper subdirectories of the given directory.
 *
 * This method copies the files that can and are potentially shared by
 * multiple TTAs in the same system design. That is, the FU, RF and IU
 * implementations.
 *
 * @param dstDirectory The destination "root" directory.
 * @exception IOException If some file cannot be copied or HDB cannot be
 *                        opened.
 */
void
BlockSourceCopier::copyShared(const std::string& dstDirectory)
    throw (IOException) {

    // copy FU files
    for (int i = 0; i < implementation_.fuImplementationCount(); i++) {
        FUImplementationLocation& fuImpl =
            implementation_.fuImplementation(i);
        string hdbFile = fuImpl.hdbFile();
        int id = fuImpl.id();
        HDBManager& manager = HDBRegistry::instance().hdb(hdbFile);
        FUEntry* entry = manager.fuByEntryID(id);
        assert(entry->hasImplementation());
        FUImplementation& impl = entry->implementation();
        copyFiles(impl, hdbFile, dstDirectory);
        delete entry;
    }

    // copy RF files
    for (int i = 0; i < implementation_.rfImplementationCount(); i++) {
        RFImplementationLocation& rfImpl =
            implementation_.rfImplementation(i);
        copyBaseRFFiles(rfImpl, dstDirectory);
    }

    // copy IU files
    for (int i = 0; i < implementation_.iuImplementationCount(); i++) {
        RFImplementationLocation& rfImpl =
            implementation_.iuImplementation(i);
        copyBaseRFFiles(rfImpl, dstDirectory);
    }

    const string DS = FileSystem::DIRECTORY_SEPARATOR;
    string sourceDir = Environment::dataDirPath("ProGe");
    // copy the utility VHDL or Verilog files
    FileSystem::copy(
        sourceDir + DS + ((language_==VHDL)?UTILITY_VHDL_FILE:UTILITY_VERILOG_FILE),
        dstDirectory + DS + ((language_==VHDL)?"vhdl":"verilog") + DS + ((language_==VHDL)?UTILITY_VHDL_FILE:UTILITY_VERILOG_FILE));
}

/**
 * Copies the block definition files of the blocks given in IDF to
 * proper subdirectories of the given directory.
 *
 * This method copies the processor-specific files that are not reused
 * between multiple TTAs.
 *
 * @param dstDirectory The destination "root" directory.
 * @exception IOException If some file cannot be copied or HDB cannot be
 *                        opened.
 */
void
BlockSourceCopier::copyProcessorSpecific(const std::string& dstDirectory)
    throw (IOException) {

    // copy decompressor file
    const string DS = FileSystem::DIRECTORY_SEPARATOR;
    string decompressorTargetDir = dstDirectory + DS + "gcu_ic";
    if (!FileSystem::fileExists(decompressorTargetDir)) {
        if (!FileSystem::createDirectory(decompressorTargetDir)) {
            string errorMsg = "Unable to create directory " +
                decompressorTargetDir;
            throw IOException(__FILE__, __LINE__, __func__, errorMsg);
        }
    }

    HDLTemplateInstantiator inst;
    inst.setEntityString(entityStr_);

    string sourceFile;
    string dstFile;
    if (implementation_.hasDecompressorFile()) {
        sourceFile = implementation_.decompressorFile();
        string file = FileSystem::fileOfPath(sourceFile);
        dstFile = decompressorTargetDir + DS + file;
        FileSystem::copy(sourceFile, dstFile);
        } else {
        sourceFile = Environment::dataDirPath("ProGe") + DS +
            ((language_==Verilog)?"idecompressor.v.tmpl":"idecompressor.vhdl.tmpl");
        string file = ((language_==Verilog)?"idecompressor.v":"idecompressor.vhdl");
        dstFile = decompressorTargetDir + DS + file;
        inst.instantiateTemplateFile(sourceFile, dstFile);
    }

    // copy ifetch unit
    string ifetchTargetDir = decompressorTargetDir;
    assert(FileSystem::fileExists(ifetchTargetDir));
    string ifetchSrcFile = Environment::dataDirPath("ProGe") + DS +
        ((language_==Verilog)?"ifetch.v.tmpl":"ifetch.vhdl.tmpl");
    string ifetchDstFile = 
        ifetchTargetDir + DS + ((language_==Verilog)?"ifetch.v":"ifetch.vhdl");

     if (!FileSystem::fileExists(ifetchDstFile)) {
        inst.instantiateTemplateFile(ifetchSrcFile, ifetchDstFile);
    }

    // copy opcodes package
    string opcodesTargetDir = decompressorTargetDir;
    assert(FileSystem::fileExists(opcodesTargetDir));
    string opcodesSrcFile = 
        Environment::dataDirPath("ProGe") + DS + 
        ((language_==Verilog)?"gcu_opcodes_pkg.vh.tmpl":"gcu_opcodes_pkg.vhdl.tmpl");
    string opcodesDstFile = opcodesTargetDir + DS + 
        ((language_==Verilog)?"gcu_opcodes_pkg.vh":"gcu_opcodes_pkg.vhdl");
    inst.instantiateTemplateFile(opcodesSrcFile, opcodesDstFile);
}

/**
 * Copies given template file to given directory and instantiates it, ie.
 * removes the .tmpl from the filename and converts it to .vhdl while
 * replacing occurances of "ENTITY_STR" with entityStr_.
 *
 * @param srcFile The location and name of the .tmpl file to copy
 * @param dstDirectory The directory to copy to and instantiate in.
 * @param newName New name for the file. If "0", only ".tmpl" is removed.
 */
void
BlockSourceCopier::instantiateHDLTemplate(
    const std::string& srcFile,
    const std::string& dstDirectory,
    std::string newName)
    throw (IOException) {

    const string DS = FileSystem::DIRECTORY_SEPARATOR;

    if (!FileSystem::fileExists(srcFile)) {
        string errorMsg = "Source file " + srcFile + " not found.";
        throw IOException(__FILE__, __LINE__, __func__, errorMsg);
    }

    if (!FileSystem::fileExists(dstDirectory)) {
        if (!FileSystem::createDirectory(dstDirectory)) {
            string errorMsg = "Unable to create directory " +
                dstDirectory;
            throw IOException(__FILE__, __LINE__, __func__, errorMsg);
        }
    }

    string source = FileSystem::fileOfPath(srcFile);
    string dstFile;

    if (newName == "0") {
        dstFile = source.erase(source.find(".tmpl", 0), string::npos);
    } else {
        dstFile = newName;
    }

    HDLTemplateInstantiator inst;
    inst.setEntityString(entityStr_);
    inst.instantiateTemplateFile(srcFile, dstDirectory + DS + dstFile);
}

/**
 * Copies the block definition files of the given RF implementation to the
 * proper subdirectories of the given directory.
 *
 * @param implementation The location of the RF implementation.
 * @param dstDirectory The destination "root" directory.
 * @exception IOException If some file cannot be copied or HDB cannot be
 *                        opened.
 */
void
BlockSourceCopier::copyBaseRFFiles(
    const IDF::RFImplementationLocation& implementation,
    const std::string& dstDirectory)
    throw (IOException) {

    string hdbFile = implementation.hdbFile();
    int id = implementation.id();
    HDBManager& manager = HDBRegistry::instance().hdb(hdbFile);
    RFEntry* entry = manager.rfByEntryID(id);
    assert(entry->hasImplementation());
    RFImplementation& impl = entry->implementation();
    copyFiles(impl, hdbFile, dstDirectory);
    delete entry;
}


/**
 * Copies the block definition files of the given HW block implementation to
 * the proper subdirectories of the given directory.
 *
 * @param implementation The block implementation.
 * @param hdbFile The HDB file that contains the block.
 * @param dstDirectory The destination "root" directory.
 * @exception UnreachableStream If some file cannot be copied to the
 *                              destination directory.
 * @exception FileNotFound If the file referred to in HDB is not found.
 */
void
BlockSourceCopier::copyFiles(
    const HDB::HWBlockImplementation& implementation,
    const std::string& hdbFile,
    const std::string& dstDirectory)
    throw (UnreachableStream, FileNotFound) {

    for (int i = 0; i < implementation.implementationFileCount(); i++) {
        BlockImplementationFile& file = implementation.file(i);
        vector<string> modulePaths = Environment::vhdlPaths(hdbFile);

        string absoluteFile;
		try {
			absoluteFile = FileSystem::findFileInSearchPaths(modulePaths, file.pathToFile());
        } catch (const Exception& e) {
            string errorMsg = "Unable to find file mentioned in HDB->" + file.pathToFile() + ":\n";
            errorMsg += e.errorMessage();
            throw FileNotFound(__FILE__, __LINE__, __func__, errorMsg);
        }

        if (!isCopied(absoluteFile)) {
            string fileName = FileSystem::fileOfPath(absoluteFile);
            string targetDir, targetFile;
            string DS = FileSystem::DIRECTORY_SEPARATOR;

            if(language_== VHDL && (ProGe::HDL)file.format()==VHDL) {
                targetDir = dstDirectory + DS + "vhdl";
            } else
            if(language_== Verilog && (ProGe::HDL) file.format()==Verilog) {
                targetDir = dstDirectory + DS + "verilog";
            } else {
                setCopied(absoluteFile);
                continue;//next file for check
            }
            
            targetFile = targetDir + DS + fileName;
            
            if (!FileSystem::fileExists(targetDir)) {
                bool directoryCreated = 
                    FileSystem::createDirectory(targetDir);
                if (!directoryCreated) {
                    string errorMsg = "Unable to create directory " +
                        targetDir  + ".";
                    throw IOException(__FILE__, __LINE__, __func__, errorMsg);
                }
            }

            try {
                FileSystem::copy(absoluteFile, targetFile);
            } catch (const Exception& e) {
                string errorMsg = "Unable to copy file " + targetFile + ":";
                errorMsg += e.errorMessage();

                throw UnreachableStream(
                    __FILE__, __LINE__, __func__, errorMsg);
            }
            setCopied(absoluteFile);
        }
    }
}


/**
 * Marks the file as copied.
 *
 * @param file The file.
 */
void
BlockSourceCopier::setCopied(const std::string& file) {
    copiedFiles_.insert(file);
}



/**
 * Tells whether the given file is copied already.
 *
 * @param file The file.
 * @return True if the file is copied, otherwise false.
 */
bool
BlockSourceCopier::isCopied(const std::string& file) const {
    return AssocTools::containsKey(copiedFiles_, file);
}
}
