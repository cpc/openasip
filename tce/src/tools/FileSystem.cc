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
 * @file FileSystem.cc
 *
 * Implementation of FileSystem class.
 *
 * @author Pekka J��skel�inen (pekka.jaaskelainen-no.spam-tut.fi)
 * @author Viljami Korhonen 2007 (viljami.korhonen-no.spam-tut.fi)
 * @author Esa M��tt� 2007 (esa.maatta-no.spam-tut.fi)
 * @note rating: red
 */

#include <fstream>
#include <string>
#include <vector>
#include <glob.h>
#include <cerrno>
#include <cstdio>
#include <unistd.h> // for truncate
#include <sys/types.h> // for truncate

/* This must be before any of the boost inclusions, to ensure
 * that the boost filesystem API we get agrees with what is
 * exposed through the header. */
#include "FileSystem.hh"

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/exception.hpp>
#include <boost/regex.hpp>
#include <boost/format.hpp>

#include "MathTools.hh"
#include "tce_config.h"

using std::string;
using std::vector;

const std::string FileSystem::DIRECTORY_SEPARATOR =
string(DIR_SEPARATOR);
const std::string FileSystem::CURRENT_DIRECTORY = ".";
const std::string FileSystem::STRING_WILD_CARD = "*";

using namespace boost::filesystem;

/**
 * Returns the path part of the given file name.
 *
 * @param fileName A file name with a (partial or absolute) path.
 * @return Path part of the file name.
 *
 */
std::string
FileSystem::directoryOfPath(const std::string fileName) {

    Path origString(fileName);
    string file = origString.string();
    Path DS(DIRECTORY_SEPARATOR);
    string::size_type lastPathPos = file.rfind(DS.string());
    if (lastPathPos == string::npos) {
        return CURRENT_DIRECTORY;
    }

    string dirPath = file.substr(0, lastPathPos);
    Path path(dirPath);
    return path.string();
}

/**
 * Returns the file part of the given path name.
 *
 * @param pathName The path name.
 * @return The file part of the path name.
 */
std::string
FileSystem::fileOfPath(const std::string pathName) {
    if (!isPath(pathName)) {
        return pathName;
    }
    Path origString(pathName);
    string path = origString.string();
    Path DS(DIRECTORY_SEPARATOR);
    unsigned int index = path.find_last_of(DIRECTORY_SEPARATOR);
    return path.substr(index + 1);
}

/**
 * Tests if a file or directory can be created by user.
 *
 * The file or directory can be created if:
 * - it does not already exist,
 * - the path where it should be created exists and is writable.
 *
 * @param fileName Name of the file or directory to test.
 * @return True if file or directory can be created, false otherwise.
 */
bool
FileSystem::fileIsCreatable(const std::string fileName) {

    if (fileName == "") {
        return false;
    }

    std::string destPath = directoryOfPath(fileName);

    return !fileExists(fileName) && fileExists(destPath) &&
        fileIsWritable(destPath);
}

/**
 * Returns the current working directory.
 *
 * @return The current working directory or, in case of error, an empty
 *         string.
 */
std::string
FileSystem::currentWorkingDir() {

    // the amount of bytes to stretch the char buffer if the path doesn't
    // fit in it
    const int increment = 100;
    int bufSize = 200;
    char* buf = new char[bufSize];

    // sets errno to ERANGE if buffer was too small
    char* retValue = getcwd(buf, bufSize);

    // grow the buffer until the path fits in it
    while (retValue == NULL && errno == ERANGE) {
        delete[] buf;
        bufSize += increment;
        buf = new char[bufSize];
        retValue = getcwd(buf, bufSize);
    }

    std::string dirName = "";
    if (retValue == NULL) {

        std::string procName = "FileSystem::currentWorkingDir";
        std::string errorMsg = "Current working directory cannot be read.";
        Application::writeToErrorLog(__FILE__, __LINE__, procName, errorMsg);

        assert(errno == EACCES);
    } else {
        dirName = buf;
    }

    delete[] buf;
    Path path(dirName);
    return path.string();
}

/**
 * Changes the working directory to a new one
 * 
 * @param path The new working directory to be set
 * @return true on success, false on error
 */
bool
FileSystem::changeWorkingDir(const std::string& path) {
    return (chdir(path.c_str()) == 0);
}

/**
 * Searches path names that matches the pattern and stores the results
 * in string vector.
 *
 * @param pattern The pattern to be matched.
 * @param filenames Results are stored here.
 */
void
FileSystem::globPath(
    const std::string& pattern,
    std::vector<std::string>& filenames) {

    glob_t globbuf;
    glob(pattern.c_str(), 0, NULL, &globbuf);
    for (size_t i = 0; i < globbuf.gl_pathc; i++) {
        filenames.push_back(globbuf.gl_pathv[i]);
    }
    // reserved memory is freed
    globfree(&globbuf);
}

/**
 * Replaces "~/" with "$HOME/". Doesn't replace ~user.
 *
 * @param stringWithTilde A string containing tilde to expanded.
 * @return The string with tilde expanded.
 */
string
FileSystem::expandTilde(const std::string& stringWithTilde) {
    string withoutTilde = stringWithTilde;
    if (withoutTilde == "~" || withoutTilde.substr(0, 2) == "~/") {
        withoutTilde.erase(0, 1);
        withoutTilde.insert(0, FileSystem::homeDirectory());

    }
    return withoutTilde;
}

/**
 * Checks if the pathName is a string representing an absolute path.
 *
 * @param pathName The investigated path.
 * @return True, if it is an absolute path, false otherwise.
 */
bool
FileSystem::isAbsolutePath(const std::string& pathName) {
    Path path(pathName);
    string pathString = path.string();
    Path DS(DIRECTORY_SEPARATOR);
    return pathString.substr(0, 1) ==
        DS.string() ? true : false;
}

/**
 * Checks if the pathName is a string representing a relative path.
 *
 * Path is relative path, if it is not absolute path and not empty
 * string.
 *
 * @param pathName The investigated path.
 * @return True, if it is a relative path, false otherwise.
 */
bool
FileSystem::isRelativePath(const std::string& pathName) {
    Path path(pathName);
    string pathString = path.string();
    return !isAbsolutePath(pathString) && !pathString.empty();
}

/**
 * Checks if the pathName is a string representing path (relative or absolute).
 *
 * @param pathName The investigated path.
 * @return True, if it is a relative path, false otherwise.
 */
bool
FileSystem::isPath(const std::string& pathName) {
    return (!pathName.empty() &&
            (isRelativePath(pathName) || isAbsolutePath(pathName)));
}

/**
 * Returns the file extension of an file, if anything is found.
 *
 * If not extension is found, an empty string is returned.
 *
 * @param fileName The name of the file.
 * @return The file extension.
 */
string
FileSystem::fileExtension(const std::string& fileName) {
    const Path path(fileName);
    return boost::filesystem::extension(path);
}

/**
 * Returns the name of the file without file extension.
 *
 * @param fileName The name of the file.
 * @return The name of the file without the file extension.
 */
string
FileSystem::fileNameBody(const std::string& fileName) {
    const Path path(fileName);
    return boost::filesystem::basename(path);
}

/**
 * Returns the given path as absolute path.
 *
 * @param pathName The path.
 * @return The absolute path.
 */
string
FileSystem::absolutePathOf(const std::string& pathName) {
    string absolutePath("");
    if (isAbsolutePath(pathName)) {
        absolutePath = pathName;
    } else {
        absolutePath = currentWorkingDir() + DIRECTORY_SEPARATOR + pathName;
    }
    Path path(absolutePath);
    path.normalize();
    return path.string();
}

/** 
 * Grants execute rights for user to a file.
 * 
 * @param fileName File whitch execute rights are to be changed.
 * @return True if operation was succesfull, otherwise false.
 */
bool
FileSystem::setFileExecutable(const std::string fileName) {
    if (fileIsWritable(fileName) && fileIsReadable(fileName)) {
        if (chmod(fileName.c_str(), S_IXUSR | S_IRUSR | S_IWUSR) != 0) {
            return false;
        }
    } else if (fileIsWritable(fileName)) {
        if (chmod(fileName.c_str(), S_IXUSR | S_IWUSR) != 0) {
            return false;
        }
    } else if (fileIsReadable(fileName)) {
        if (chmod(fileName.c_str(), S_IXUSR | S_IRUSR) != 0) {
            return false;
        }
    } else {
        if (chmod(fileName.c_str(), S_IXUSR) != 0) {
            return false;
        }
    }
    return true;
}

/**
 * Creates a directory if it doesn't already exist.
 *
 * All non-existing directories in the path are created.
 *
 * @param path The path of directory.
 * @return True if directory is created, false otherwise.
 */
bool
FileSystem::createDirectory(const std::string& path) {
    if (!isAbsolutePath(path)) {
        return false;
    }
    Path DS(DIRECTORY_SEPARATOR);
    Path orPath(path.substr(1));
    string origPath = orPath.string();
    string currentPath = DS.string();
    while (origPath.size() > 0) {
        string::size_type pos = origPath.find(DS.string());
        if (pos == string::npos) {
            currentPath += origPath;
            origPath = "";
        } else {
            currentPath += origPath.substr(0, pos);
            origPath.replace(0, pos + 1, "");
        }
        try {
            Path dirPath(currentPath);
            if (!boost::filesystem::exists(dirPath)) {
                boost::filesystem::create_directory(dirPath);
            }
        } catch (...) {
            // directory creation failed, probably because of lacking rights
            return false;
        }
        currentPath += DS.string();
    }
    return true;
}

/**
 * Creates a temporary directory to the given path
 * 
 * @param path Path to create the temporary directory in
 * @return Full path to the generated temporary directory. Empty string on error
 */
std::string 
FileSystem::createTempDirectory(const std::string& path) {
    const int RANDOM_CHARS = 10;
    const string DS(DIRECTORY_SEPARATOR);
    string tempDir = path + DS + "tmp_tce_";
    
    for (int i = 0; i < RANDOM_CHARS || fileExists(tempDir); ++i) {
        tempDir += static_cast<char>(MathTools::random('0', '9'));
        tempDir += static_cast<char>(MathTools::random('a', 'z'));
        tempDir += static_cast<char>(MathTools::random('A', 'Z'));
    }
    
    if (!createDirectory (tempDir)) {
        return "";
    } else {
        return tempDir;
    }
}

/**
 * Creates a file if it doesn't already exist.
 *
 * @param file The name of the file.
 * @return True if file is created, false otherwise.
 */
bool
FileSystem::createFile(const std::string& file) {

    Path filePath(file);
    string fileName = filePath.string();

    if (fileExists(file)) {
        return true;
    }

    if (fileIsCreatable(fileName)) {
        FILE* f = fopen(fileName.c_str(), "w");
        fclose(f);
        return true;
    } else {
        return false;
    }
}

/**
 * Removes a file or a directory.
 *
 * @param path The path of a file or a directory.
 * @return True if directory or file is destroyed, false otherwise.
 */
bool
FileSystem::removeFileOrDirectory(const std::string& path) {
    if (fileExists(path)) {
        try {
            Path tcePath(path);
            boost::filesystem::remove_all(tcePath);
            return true;
        } catch (...) {
            // failed to destroy a file or directory
            // probably because of lack of rights.
            return false;
        }
    } else {
        return false;
    }
}

/**
 * Copies source file to target file.
 *
 * Copying can fail for the following reasons:
 * Source file doesn't exist, source is directory, or target string is empty.
 *
 * If target file already exists then it's removed and source file is copied 
 * in its place. If target file is a existing directory copies the file under
 * it without changing the files name.
 *
 * @param source File to be copied.
 * @param target Target of the copy operation.
 * @exception IOException in case the copying failed.
 */
void
FileSystem::copy(const std::string& source, const std::string& target) 
    throw (IOException) {
    namespace fs = boost::filesystem;
    Path sourcePath(source);
    Path targetPath(target);

    try {
        if (fs::exists(targetPath)) {
            if(!fileIsDirectory(target)) {
                fs::remove(targetPath);
            } else {
                fs::path::iterator lastIt = --(sourcePath.end());
                targetPath /= *lastIt;
            }
        }
        fs::copy_file(sourcePath, targetPath);
    } catch (boost::filesystem::filesystem_error e) {
        throw IOException(
            __FILE__, __LINE__, __func__, 
            (boost::format(
                "Unable to copy '%s' to '%s'") %
                sourcePath.string() % targetPath.string()).str());                           
    }
}

/**
 * Searches the given file in the given search paths and returns the
 * absolute path to the file.
 *
 * @param searchPaths The search paths in priority order.
 * @param file The file to search.
 * @return Absolute path to the file.
 * @exception FileNotFound If the file is not found in the search paths.
 */
std::string
FileSystem::findFileInSearchPaths(
    const std::vector<std::string>& searchPaths,
    const std::string& file)
    throw (FileNotFound) {

    string DS = FileSystem::DIRECTORY_SEPARATOR;

    if (isAbsolutePath(file)) {
        return file;
    }

    for (vector<string>::const_iterator iter = searchPaths.begin();
         iter != searchPaths.end(); iter++) {
        string path = *iter;
        string pathToFile = path + DS + file;
        if (fileExists(pathToFile)) {
            return absolutePathOf(pathToFile);
        } 
    }

    string errorMsg = "File " + file + " not found in any search path.";
    throw FileNotFound(__FILE__, __LINE__, __func__, errorMsg);
}

/**
 * Returns the files found in the given directory.
 *
 * @param directory The directory.
 * @return Vector containing the files.
 * @exception FileNotFound If the given directory does not exist.
 */
std::vector<std::string>
FileSystem::directoryContents(
    const std::string& directory,
    const bool absolutePaths) 
    throw (FileNotFound) {

    try {
        std::vector<std::string> contents;
        // default construction yields past the end
        boost::filesystem::directory_iterator end_iter;
        
        if (directory != "") {
            Path path(directory);
            for (boost::filesystem::directory_iterator iter(path); 
                 iter != end_iter; iter++) {
                if (absolutePaths) {
                    contents.push_back(absolutePathOf(iter->path().string()));
                } else {
                    contents.push_back(iter->path().string());
                }

            }
        } else {
            for (boost::filesystem::directory_iterator iter(
                     boost::filesystem::current_path()); 
                 iter != end_iter; iter++) {
                if (absolutePaths) {
                    contents.push_back(absolutePathOf(iter->path().string()));
                } else {
                    contents.push_back(iter->path().string());
                }
            }
        }
        
        return contents;

    } catch (const boost::filesystem::filesystem_error& e) {
        throw FileNotFound(__FILE__, __LINE__, __func__, e.what());
    }
}

/**
 * Returns all the sub-directories found starting from the given directory.
 * Uses recursion to find out all the possible sub-directory levels as well.
 * 
 * @param directory The directory where to search. Not listed in the results!
 * @return Vector containing the sub-directories (and their sub-dirs, and...).
 * @note This function will not search the directories containing "." !
 * @exception FileNotFound If the given directory does not exist.
 */
std::vector<std::string> 
FileSystem::directorySubTrees(const std::string& directory)
    throw (FileNotFound) {
    
    std::vector<std::string> subTrees;
    
    try {    
        directory_iterator end_itr;
    
        for (directory_iterator itr(directory); itr != end_itr; ++itr) {
            if (is_directory(*itr) && exists(*itr) && 
                (*itr).path().string().find(".") == string::npos) {
                subTrees.push_back((*itr).path().string());
                
                std::vector<std::string> subSubTrees = 
                    directorySubTrees((*itr).path().string());
                    
                for (size_t i = 0; i < subSubTrees.size(); ++i) {
                    subTrees.push_back(subSubTrees.at(i));
                }
            }
        }
    } catch (const boost::filesystem::filesystem_error& e) {
        throw FileNotFound(__FILE__, __LINE__, __func__, e.what());
    }        
        
    return subTrees;
}

/**
 * Attempts to find a file from the given directory hierarchy
 *
 * @param startDirectory The directory to start the search.
 * @param fileName The filename to search for.
 * @param pathFound A path to the found file.
 * @return true if the file was found. Otherwise false.
 */
bool 
FileSystem::findFileInDirectoryTree(
    const Path& startDirectory,
    const std::string& fileName,
    Path& pathFound) {

    if (!exists(startDirectory)) {
        return false;
    }
    directory_iterator end_itr;
    
    for (directory_iterator itr(startDirectory); itr != end_itr; ++itr) {
        if (is_directory(*itr)) {
            Path p((*itr).path().string());
            if (findFileInDirectoryTree(p, fileName, pathFound))
                return true;
        }
#if BOOST_VERSION >= 103600
        else if (itr->path().filename() == fileName)
#else
        else if (itr->leaf() == fileName)
#endif
        {
            pathFound = Path((*itr).path().string());
            return true;
        }
    }
    return false;
}

/** 
 * Compares two file names considering path.
 *
 * Compares two strings representing file names that can be with absolut or
 * relative paths. Relative paths are interpreted against given root directory
 * parameter.
 * 
 * @param first File name to be compared.
 * @param second File name to be compared.
 * @param rootDirectory Base dir for relative paths in file names.
 * @return True if file names are same considering path, otherwise false.
 */
bool
FileSystem::compareFileNames(
    const std::string& first,
    const std::string& second,
    const std::string& rootDirectory) {
    
    std::string testRootDir = rootDirectory;
    if (rootDirectory.substr(rootDirectory.size()-1) != DIRECTORY_SEPARATOR) {
        testRootDir += DIRECTORY_SEPARATOR;    
    } 

    if (first == second) {
        return true;
    }

    if (FileSystem::isRelativePath(first) && 
        FileSystem::isRelativePath(second)) {
        return false; 
    } else if (FileSystem::isRelativePath(first)) {
        return ((testRootDir + first) == second) ? true : false;
    } else if (FileSystem::isRelativePath(second)) {
        return ((testRootDir + second) == first) ? true : false;
    }

    return false;
}

/**
 * Creates relative (to given base dir) directory.
 *
 * Both directories given as parameter have to be absolute or false is
 * returned.
 *
 * @param baseDir string representing base directory.
 * @param toRelDir string representing a directory which is changed to
 * relative.
 * @return true if toRelDir is modified as relative directory otherwise false.
 */
bool 
FileSystem::relativeDir(const std::string& baseDir, std::string& toRelDir) {

    namespace fs = boost::filesystem;
    
    Path basePath(baseDir);
    Path toRelPath(toRelDir);
    
    fs::path::iterator dstIt = basePath.begin();
    fs::path::iterator POIt = toRelPath.begin();

    fs::path::iterator dstEndIt = basePath.end();
    fs::path::iterator POEndIt = toRelPath.end();

    unsigned int sameCount = 0;
    for (; dstIt != dstEndIt && POIt != POEndIt && *dstIt == *POIt;
        ++dstIt, ++POIt, ++sameCount) {}

    // both parameter dirs have to be absolute
    // first path part is allways '/'
    if (sameCount < 1) {
        return false;
    }
    
    // if the to be realtive dir is under the base dir
    if (dstIt == dstEndIt) {
        toRelDir.clear();
        while (POIt != POEndIt) {
#if BOOST_FILESYSTEM_VERSION < 3
            toRelDir.append(*POIt++);
#else
            std::string const tmp = POIt->string();
            toRelDir.append(tmp);
            POIt++;
#endif
            if (POIt != POEndIt) {
                toRelDir.append(DIRECTORY_SEPARATOR);
            }
        }    
        return true;
    } else { // if above
        std::string temp;
        while (POIt != POEndIt) {
#if BOOST_FILESYSTEM_VERSION < 3
            toRelDir.append(*POIt++);
#else
            std::string const tmp = POIt->string();
            POIt++;
            temp.append(tmp);
#endif
            if (POIt != POEndIt) {
                temp.append(DIRECTORY_SEPARATOR);
            }
        }

        unsigned int diffCount = 0;
        for (; dstIt != dstEndIt; ++dstIt, ++diffCount) {
        }

        toRelDir.clear();
        for (unsigned int i = 0; diffCount > i; ++i) {
            toRelDir.append("..");
            toRelDir.append(DIRECTORY_SEPARATOR);
        }
        toRelDir.append(temp);
    }

    return true;
}

/**
 * Creates relative path out of given base path if a relative path can be
 * found under any of the search paths. Example: if base path is
 * "/usr/foo/bar/x.hdb" and provided search path is "/usr/foo", the
 * following relative path is formed: "bar/x.hdb".
 *
 * @param searchPaths Relative path to base path is searched under these.
 * @param basePath Path that needs to be changed into a relative path.
 * @param toRelPath Outputs possibly created relative path.
 * @return True if a relative path was created out of base path.
 */
bool 
FileSystem::makeRelativePath(
    const std::vector<std::string>& searchPaths, 
    const std::string& basePath,
    std::string& toRelPath) {    

    if (!isAbsolutePath(basePath)) {
        return false;
    }

    for (unsigned int i = 0; i < searchPaths.size(); ++i) {
        string searchPath = searchPaths.at(i);
        string relativePath = basePath;
        
        // try to find a relative path to the base path under search path
        if (relativeDir(searchPath, relativePath)) {
            string fullPath = searchPath + DIRECTORY_SEPARATOR + relativePath;       
            
            // special case: if provided search path was same as base path
            if (relativePath == "") {
                toRelPath = relativePath;
                return true;
            } else if (isRelativePath(relativePath) && fileExists(fullPath)) {
                if (fullPath.length() == basePath.length()) {
                    toRelPath = relativePath;
                    return true;
                }
            }
        }
    }

    return false;
}

/**
 * Reads a block of text from a file.
 *
 * Start and end of the block are found by matching given regular expressions
 * against whole lines in source file.
 *
 * @param sourceFile File that is read.
 * @param blockStartRE Regex that matches beginning of the block (one whole
 * line) that is going to be read from the source file.
 * @param blockEndRE Regex that matches end of the block (one whole line) that
 * is going to be read from the source file. 
 * @param readBlock Parameter where the read block is to be stored.
 * @param includeMatchingLines Include lines that mark the borders of the
 * block (lines that matched given regular expressions) to the block.
 * @return True if something was stored to readBlock string, otherwise false. 
 */
bool
FileSystem::readBlockFromFile(
    const std::string& sourceFile,
    const std::string& blockStartRE, //reads from file beginning by default
    const std::string& blockEndRE, // reads to file end by default
    std::string& readBlock,
    const bool includeMatchingLines) { //true by default

    const int LINESIZE = 256;
    char line[LINESIZE];
    
    const boost::regex reStart(blockStartRE, 
            boost::regex::perl|boost::regex::icase);
    const boost::regex reEnd(blockEndRE, 
            boost::regex::perl|boost::regex::icase);
    
    string::const_iterator begin;
    string::const_iterator end;
    string stemp;
    std::ifstream ifs(sourceFile.c_str(), std::ifstream::in);
    boost::match_results<string::const_iterator> matches;
    bool outsideBlock = true;
    while (ifs.good()) {

        ifs.getline(line, LINESIZE-1);
        stemp = string(line);
        begin = stemp.begin();
        end = stemp.end();

        // test if found block border
        if (boost::regex_search(begin, end, outsideBlock ? reStart : reEnd)) {
            if (includeMatchingLines) {
               readBlock.append(stemp + "\n");
            } 
            if (!outsideBlock) {
                break;
            } else {
                outsideBlock = false;
            }
        } else if (!outsideBlock) { // if inside block
            readBlock.append(stemp + "\n");
        }
    }

    ifs.close();

    return ifs.good();
}

/** 
 * Appends to specified place in file, can replace a section.
 *
 * Doesn't use temp files so uses lot of memory for big files.
 * 
 * @param targetFile File to be edited. 
 * @param ARStartRE Regex that matches the line where appending starts. 
 * @param writeToFile String that is written to the file. 
 * @param AREndRE Regex that matches last line that is replaced or deleted if
 * writeToFile string contains less lines than is between start and end
 * matches.
 * @param discardBlockBorder If true Regex matching lines are replaced or deleted.
 * 
 * @return True if something was writen to the file, otherwise false.
 */
bool
FileSystem::appendReplaceFile(
    const std::string& targetFile,
    const std::string& ARStartRE,
    const std::string& writeToFile,
    const std::string& AREndRE,
    const bool discardBlockBorder) {

    const int LINESIZE = 1000;
    char line[LINESIZE];
    
    const boost::regex reStart(ARStartRE, 
        boost::regex::perl|boost::regex::icase);
    boost::regex reEnd;
    if (!AREndRE.empty()) {
        reEnd = boost::regex(AREndRE, boost::regex::perl|boost::regex::icase);
    }
    boost::regex re = reStart;
    
    string::const_iterator begin;
    string::const_iterator end;
    string stemp = "";
    string endFile = "";
    int blockStartPos = -1;
    std::fstream fs(targetFile.c_str(), std::fstream::in | std::fstream::out);
    boost::match_results<string::const_iterator> matches;

    bool beforeBlock = true;
    bool afterBlock = false;
    while (fs.good()) {

        fs.getline(line, LINESIZE-1);

        if (fs.eof()) {
            if (blockStartPos != -1) {
                fs.clear();
                fs.seekp(blockStartPos); 
                fs.seekg(blockStartPos); 
            } else {
                // no place of appending was found, ARStartRE didn't match any
                // line
                fs.close();
                return false;
            }

            if (!fs.good()) {
                fs.close();
                return false;
            }

            fs.write(writeToFile.c_str(), writeToFile.length());
            fs.write(endFile.c_str(), endFile.length());

            if (truncate(targetFile.c_str(), fs.tellp()) != 0) {
                abortWithError("truncate failed!");
            }

            break;
        }

        stemp = string(line);
        begin = stemp.begin();
        end = stemp.end();

        // test if found block border
        if (!afterBlock && boost::regex_search(begin, end, re)) {
            if (beforeBlock) {
                // to inside replace block
                beforeBlock = false;
                // if no replace block end then no searching for it
                afterBlock = AREndRE.empty() ? true : afterBlock;

                if (discardBlockBorder) {
                    blockStartPos = static_cast<int>(fs.tellg()) - 
                        static_cast<int>(fs.gcount());
                } else {
                    blockStartPos = fs.tellg();
                }
                re = reEnd;
            } else {
                // to outside replace block
                if (!discardBlockBorder) {
                    endFile.append(stemp + "\n");
                }
                afterBlock = true;
            }
        } else if (afterBlock) { 
            // outside replace block
            endFile.append(stemp + "\n");
        }
    }

    fs.close();
    return true;
}

//////////////////////////////////////////////////////////////////////////////
// Path
//////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 *
 * @param pathName String to be converted to path.
 */
Path::Path(const std::string& pathName) :
    boost::filesystem::path(pathName) {
}

/**
 * Destructor.
 */
Path::~Path() {
}
