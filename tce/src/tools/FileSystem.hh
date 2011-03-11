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
 * @file FileSystem.hh
 *
 * Declaration of FileSystem class.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pekka.jaaskelainen-no.spam-tut.fi)
 * @author Viljami Korhonen 2007 (viljami.korhonen-no.spam-tut.fi)
 * @author Esa M‰‰tt‰ 2007 (esa.maatta-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_FILESYSTEM_HH
#define TTA_FILESYSTEM_HH

#include <string>
#include <vector>

#define BOOST_FILESYSTEM_VERSION 2

#include <boost/filesystem/path.hpp>

#include <unistd.h> // access()

#include "Application.hh"
#include "Exception.hh"


class Path;


/**
 * Portability layer for platform-dependent functions that access the
 * filesystem.
 */
class FileSystem {
public:

    // file tests
    static bool fileExists(const std::string fileName);
    static bool fileIsWritable(const std::string fileName);
    static bool fileIsReadable(const std::string fileName);
    static bool fileIsExecutable(const std::string fileName);
    static bool fileIsCreatable(const std::string fileName);
    static bool fileIsDirectory(const std::string fileName);

    // path name string parsing functions
    static std::string directoryOfPath(const std::string fileName);
    static std::string fileOfPath(const std::string pathName);
    static bool isAbsolutePath(const std::string& pathName);
    static bool isRelativePath(const std::string& pathName);
    static bool isPath(const std::string& pathName);
    static std::string fileExtension(const std::string& fileName);
    static std::string fileNameBody(const std::string& fileName);
    static std::string absolutePathOf(const std::string& pathName);

    // file system environment functions
    static std::string currentWorkingDir();
    static bool changeWorkingDir(const std::string& path);
    static std::string homeDirectory();

    // file permission changing functions 
    static bool setFileExecutable(const std::string fileName);
    
    // should this be moved to Application instead?
    static bool runShellCommand(const std::string command);

    // path name string expansion functions
    static void globPath(const std::string& pattern,
                         std::vector<std::string>& filenames);
    static std::string expandTilde(const std::string& stringWithTilde);

    static bool createDirectory(const std::string& path);
    static std::string createTempDirectory(const std::string& path="/tmp");
    static bool createFile(const std::string& file);
    static bool removeFileOrDirectory(const std::string& path);

    static void copy(const std::string& source, const std::string& target)
        throw (IOException);

    static std::string findFileInSearchPaths(
        const std::vector<std::string>& searchPaths,
        const std::string& file)
        throw (FileNotFound);

    static std::vector<std::string> directoryContents(
        const std::string& directory,
        const bool absolutePaths = true)
        throw (FileNotFound);
        
    static std::vector<std::string> directorySubTrees(
        const std::string& directory)
        throw (FileNotFound);
        
    static bool findFileInDirectoryTree(
        const Path& startDirectory,
        const std::string& fileName,
        Path& pathFound);

    template <typename STLCONT>
    static bool findFromDirectory(
        const std::string& regex,
        const std::string& directory,
        STLCONT& found)
        throw (FileNotFound); 

    template <typename STLCONT>
    static bool findFromDirectoryRecursive(
            const std::string& regex,
            const std::string& directory,
            STLCONT& found);

    static bool compareFileNames(
        const std::string& first,
        const std::string& second,
        const std::string& rootDirectory);

    static bool relativeDir(
        const std::string& baseDir,
        std::string& toRelDir);

    static bool readBlockFromFile(
        const std::string& sourceFile,
        const std::string& blockStartRE,
        const std::string& blockEndRE,
        std::string& readBlock,
        const bool includeMatchingLines = true);

    static bool appendReplaceFile(
        const std::string& targetFile,
        const std::string& ARStartRE,
        const std::string& writeToFile,
        const std::string& AREndRE = "",
        const bool discardBlockBorder = "true");

    static const std::string DIRECTORY_SEPARATOR;
    static const std::string CURRENT_DIRECTORY;
    static const std::string STRING_WILD_CARD;
};

/**
 * Class for handling paths.
 */
class Path : public boost::filesystem::path {
public:
    explicit Path(const std::string& pathName);
    virtual ~Path();
private:
    static bool check(const std::string& name);
};

#include "FileSystem.icc"

#endif
