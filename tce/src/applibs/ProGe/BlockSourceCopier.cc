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
 * @file BlockSourceCopier.cc
 *
 * Implementation of BlockSourceCopier class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#include <string>
#include <vector>
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

using namespace IDF;
using namespace HDB;
using std::string;
using std::vector;

namespace ProGe {

/**
 * The constructor.
 */
BlockSourceCopier::BlockSourceCopier(
    const IDF::MachineImplementation& implementation) :
    implementation_(implementation) {
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
 * @param dstDirectory The destination "root" directory.
 * @exception IOException If some file cannot be copied or HDB cannot be
 *                        opened.
 */
void
BlockSourceCopier::copy(const std::string& dstDirectory)
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
    string sourceFile;
    string dstFile;
    if (implementation_.hasDecompressorFile()) {
        sourceFile = implementation_.decompressorFile();
        string file = FileSystem::fileOfPath(sourceFile);
        dstFile = decompressorTargetDir + DS + file;
    } else {
        sourceFile = Environment::dataDirPath("ProGe") + DS +
            "decompressor.vhdl";
        string file = FileSystem::fileOfPath(sourceFile);
        dstFile = decompressorTargetDir + DS + file;
    }

    boost::format unableToCopy("Unable to copy file %1% to %2%");
    FileSystem::copy(sourceFile, dstFile);

    // copy ifetch unit
    string ifetchTargetDir = decompressorTargetDir;
    assert(FileSystem::fileExists(ifetchTargetDir));
    string ifetchSrcFile = Environment::dataDirPath("ProGe") + DS +
        "ifetch.vhdl";
    string ifetchDstFile = ifetchTargetDir + DS + FileSystem::fileOfPath(
        ifetchSrcFile);
    FileSystem::copy(ifetchSrcFile, ifetchDstFile);

    // copy opcodes package
    string opcodesTargetDir = decompressorTargetDir;
    assert(FileSystem::fileExists(opcodesTargetDir));
    string opcodesSrcFile = Environment::dataDirPath("ProGe") + DS + 
        "opcodes_pkg.vhdl";
    string opcodesDstFile = opcodesTargetDir + DS + 
        FileSystem::fileOfPath(opcodesSrcFile);
    FileSystem::copy(opcodesSrcFile, opcodesDstFile);
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

        string absoluteFile = 
            FileSystem::findFileInSearchPaths(modulePaths, file.pathToFile());

        if (!isCopied(absoluteFile)) {
            string fileName = FileSystem::fileOfPath(absoluteFile);
            string targetFile;
            string DS = FileSystem::DIRECTORY_SEPARATOR;

            switch (file.format()) {
            case BlockImplementationFile::VHDL:
                targetFile = dstDirectory + DS + "vhdl" + DS + fileName;
                break;
            default: 
                assert(false);
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
