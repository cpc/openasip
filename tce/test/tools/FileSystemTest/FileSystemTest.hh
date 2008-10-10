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
 * @file FileSystemTest.hh
 *
 * A test suite for FileSystem class
 *
 * @author Pekka J‰‰skel‰inen 2004 (poj-no.spam-iki.fi)
 */

#ifndef TTA_FILESYSTEM_TEST_HH
#define TTA_FILESYSTEM_TEST_HH

#include <TestSuite.h>
#include <string>
#include <vector>
#include <unistd.h>

#include "FileSystem.hh"
#include "ContainerTools.hh"
#include "tce_config.h"

using std::string;
using std::vector;

/**
 * Implements the tests needed to verify correct operation of FileSystem 
 * class.
 */
class FileSystemTest : public CxxTest::TestSuite {
public:
    void testFileIsWritable();
    void testFileIsDirectory();
    void testDirectoryOfPath();
    void testExpandTilde();
    void testGlobPath();
    void testIsPath();  
    void testFileOfPath();
    void testCurrentWorkingDir();
    void testFileExtensionAndBody();
    void testCreationAndRemoval();
    void testCopy();
    void testToAbsolutePath();
    void testFindFileInSearchPaths();
    void testDirectoryContents();

private:
    /// Existing, writable file name.
    static const string writableFile_;

    /// Non-existing file name in existing path with write access.
    static const string nonExistingWritableFile_;

    /// Non-existing file name in non-existing path.
    static const string nonExistingFile_;
};

const string FileSystemTest::writableFile_ =
"data" + FileSystem::DIRECTORY_SEPARATOR + "dummy";
const string FileSystemTest::nonExistingFile_ =
"foo" + FileSystem::DIRECTORY_SEPARATOR + "bar";
const string FileSystemTest::nonExistingWritableFile_ =
"data" + FileSystem::DIRECTORY_SEPARATOR + "dummy2";

/**
 * Tests that file is considered writable if there exists a file that
 * user has write access to or new file can be created to the path.
 */
void
FileSystemTest::testFileIsWritable() {
    TS_ASSERT(FileSystem::fileIsWritable(writableFile_));
    TS_ASSERT(!FileSystem::fileIsWritable(nonExistingWritableFile_));
    TS_ASSERT(FileSystem::fileIsCreatable(nonExistingWritableFile_));
    TS_ASSERT(!FileSystem::fileIsCreatable(""));
    TS_ASSERT(!FileSystem::fileIsWritable(nonExistingFile_));

}

/**
 * Tests that checking whether file is a directory works.
 */
void
FileSystemTest::testFileIsDirectory() {
    TS_ASSERT(FileSystem::fileIsDirectory("data"));
    TS_ASSERT(!FileSystem::fileIsDirectory("FileSystemTest.hh"));
}

/**
 * Test that parsing directory from path name works. 
 */
void 
FileSystemTest::testDirectoryOfPath() {
    string DS = FileSystem::DIRECTORY_SEPARATOR;
    string path1 = "foo" + DS + "bar";
    string path2 = "foo" + DS + "bar" + DS + "foo";
    TS_ASSERT_EQUALS(FileSystem::directoryOfPath(path2), path1);
    TS_ASSERT_EQUALS(FileSystem::directoryOfPath(path1), "foo");
    TS_ASSERT_EQUALS(FileSystem::directoryOfPath("foo"), ".");
    
}

/**
 * Tests that tilde expansion works.
 */
void
FileSystemTest::testExpandTilde() {

    string DS = FileSystem::DIRECTORY_SEPARATOR;
    string test1 = "~" + DS + "foo";
    string correct1 = FileSystem::homeDirectory() + DS + "foo";

    string test2 = "~jussi";

    string test3 = "foo~";

    TS_ASSERT_EQUALS(FileSystem::expandTilde(test1), correct1);
    TS_ASSERT_EQUALS(FileSystem::expandTilde(test2), test2);
    TS_ASSERT_EQUALS(FileSystem::expandTilde(test3), test3);
}

/**
 * Tests that globbing path works.
 */
void
FileSystemTest::testGlobPath() {
    vector<string> filenames;
    string DS = FileSystem::DIRECTORY_SEPARATOR;
    string path = "data" + DS + "*.dd";
    string fileName = "data" + DS + "dummy.dd";
    string notFound = "data" + DS + "notfound";
    FileSystem::globPath(path, filenames);
    TS_ASSERT_EQUALS(static_cast<int>(filenames.size()), 1);
    TS_ASSERT_EQUALS(filenames[0], fileName);

    filenames.clear();
    FileSystem::globPath(notFound, filenames);
    TS_ASSERT_EQUALS(static_cast<int>(filenames.size()), 0);
}

/**
 * Test that different kinds of path strings are regocnized.
 */
void
FileSystemTest::testIsPath() {
    string DS = FileSystem::DIRECTORY_SEPARATOR;
    string path1 = DS;
    string path2 = DS + "foo" + DS + "bar";
    string path3 = "foo";
    string path4 = "foo" + DS + "bar";
    TS_ASSERT(FileSystem::isAbsolutePath(path2) == true);
    TS_ASSERT(FileSystem::isAbsolutePath(path1) == true);
    TS_ASSERT(FileSystem::isAbsolutePath(path3) == false);
    TS_ASSERT(FileSystem::isAbsolutePath(path4) == false);

    TS_ASSERT(FileSystem::isRelativePath(path4) == true);
    
    // this can be considered a file name instead of a relative path:
    
    TS_ASSERT(FileSystem::isRelativePath(path3) == true);
    TS_ASSERT(FileSystem::isRelativePath(path2) == false);

    TS_ASSERT(FileSystem::isPath("") == false);
}

/**
 * Test that files are correctly fetched from paths.
 */
void
FileSystemTest::testFileOfPath() {
    string DS = FileSystem::DIRECTORY_SEPARATOR;
    string path1 = DS + "foo" + DS + "bar";
    string path2 = path1 + DS + "foo"; 
    TS_ASSERT_EQUALS(FileSystem::fileOfPath(path2), "foo");
    TS_ASSERT_EQUALS(FileSystem::fileOfPath(path1), "bar");
    TS_ASSERT_EQUALS(FileSystem::fileOfPath("foo"), "foo");
}

/**
 * Tests that current working dir is found correctly.
 */
void
FileSystemTest::testCurrentWorkingDir() {
    char* workingDir = getcwd(NULL, 0);
    TS_ASSERT_EQUALS(FileSystem::currentWorkingDir(), workingDir);
    free(workingDir);
}

/**
 * Tests that fileExtension() and fileNameBody() work.
 */
void 
FileSystemTest::testFileExtensionAndBody() {
    TS_ASSERT_EQUALS(FileSystem::fileExtension("foo.rmm"), ".rmm");
    TS_ASSERT_EQUALS(FileSystem::fileExtension("foo"), "");
    TS_ASSERT_EQUALS(FileSystem::fileNameBody("foo.rmm"), "foo");
    TS_ASSERT_EQUALS(FileSystem::fileNameBody("foo"), "foo");
}

/**
 * Test that file creation and removal works.
 */
void
FileSystemTest::testCreationAndRemoval() {
    string DS = FileSystem::DIRECTORY_SEPARATOR;
    string path = FileSystem::currentWorkingDir() + DS + "bar";
    string file = path + DS + "foo";
    string weirdPath = path + DS + ".." + DS + "bar";
    TS_ASSERT_EQUALS(FileSystem::fileExists(path), false);
    TS_ASSERT_EQUALS(FileSystem::fileExists(file), false);
    FileSystem::createDirectory(path);
    FileSystem::createFile(file);
    TS_ASSERT_EQUALS(FileSystem::fileExists(path), true);
    TS_ASSERT_EQUALS(FileSystem::fileExists(file), true);
    TS_ASSERT(FileSystem::fileExists(weirdPath));
    FileSystem::removeFileOrDirectory(file);
    FileSystem::removeFileOrDirectory(path);
    TS_ASSERT_EQUALS(FileSystem::removeFileOrDirectory(nonExistingFile_),
                     false);
    TS_ASSERT_EQUALS(FileSystem::fileExists(path), false);
    TS_ASSERT_EQUALS(FileSystem::fileExists(file), false);
}

/**
 * Test that file copying works.
 */
void
FileSystemTest::testCopy() {
    string DS = FileSystem::DIRECTORY_SEPARATOR;
    string source = FileSystem::currentWorkingDir() + DS + writableFile_;
    string target = FileSystem::currentWorkingDir() + DS + "data" +
        DS + "foo";

    TS_ASSERT_EQUALS(FileSystem::fileExists(target), false);
    
    FileSystem::copy(source, target);

    TS_ASSERT_EQUALS(FileSystem::fileExists(target), true);

    FileSystem::removeFileOrDirectory(target);
}


/**
 * Tests converting relative path to absolute path.
 */
void
FileSystemTest::testToAbsolutePath() {
    string cwd = FileSystem::currentWorkingDir();
    string DS = FileSystem::DIRECTORY_SEPARATOR;
    string relPath1 = "." + DS + "foo" + DS + "bar";
    string relPath2 = ".." + DS + "foo" + DS + "bar.ext";
    string absPath = DS + "foo" + DS + "bar" + DS + ".." + DS + "foo" + DS +
        "foo";
    TS_ASSERT_EQUALS(
        FileSystem::absolutePathOf(relPath1), cwd + DS + "foo" + DS + "bar");
    TS_ASSERT_EQUALS(
        FileSystem::absolutePathOf(relPath2), 
        cwd.substr(0, cwd.length() - 15) + DS + "foo" + DS + "bar.ext");
    TS_ASSERT_EQUALS(
        FileSystem::absolutePathOf(absPath), 
        DS + "foo" + DS + "foo" + DS + "foo");
}


/**
 * Tests finding a file from the given search paths.
 */
void
FileSystemTest::testFindFileInSearchPaths() {
    string cwd = FileSystem::currentWorkingDir();
    string DS = FileSystem::DIRECTORY_SEPARATOR;
    std::vector<string> paths;
    TS_ASSERT_THROWS(
        FileSystem::findFileInSearchPaths(paths, writableFile_), 
        FileNotFound);
    paths.push_back(".");
    string writableFilePath = "./" + writableFile_;
    TS_ASSERT_EQUALS(
        FileSystem::findFileInSearchPaths(paths, writableFile_), 
        cwd + DS + writableFile_);
    TS_ASSERT_THROWS(
        FileSystem::findFileInSearchPaths(paths, "dummy"), FileNotFound);
    paths.push_back("data");
    TS_ASSERT_EQUALS(
        FileSystem::findFileInSearchPaths(paths, "dummy"),
        cwd + "/data/dummy");    
}


/**
 * Tests obtaining the directory contents.
 */
void
FileSystemTest::testDirectoryContents() {

    std::vector<std::string> thisContents = 
        FileSystem::directoryContents("");
    TS_ASSERT(
        ContainerTools::containsValue(
            thisContents, FileSystem::absolutePathOf("FileSystemTest.hh")));
    TS_ASSERT(
        ContainerTools::containsValue(
            thisContents, FileSystem::absolutePathOf("data")));

    std::vector<std::string> dataContents = 
        FileSystem::directoryContents("data");
    TS_ASSERT(
        ContainerTools::containsValue(
            dataContents, FileSystem::absolutePathOf("data/dummy")));
    TS_ASSERT(
        ContainerTools::containsValue(
            dataContents, FileSystem::absolutePathOf("data/dummy.dd")));

    TS_ASSERT_THROWS(
        FileSystem::directoryContents("./foobar"), FileNotFound);
}

#endif
