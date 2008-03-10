/**
 * @file BlockImplementationFile.cc
 *
 * Implementation of BlockImplementationFile class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#include "BlockImplementationFile.hh"

namespace HDB {

/**
 * The constructor.
 *
 * @param pathToFile Path to the file.
 * @param format Format of the file.
 */
BlockImplementationFile::BlockImplementationFile(
    const std::string& pathToFile,
    Format format) :
    file_(pathToFile), format_(format) {
}


/**
 * The destructor.
 */
BlockImplementationFile::~BlockImplementationFile() {
}


/**
 * Returns path to the file.
 */
std::string
BlockImplementationFile::pathToFile() const {
    return file_;
}


/**
 * Returns the format of the file.
 */
BlockImplementationFile::Format
BlockImplementationFile::format() const {
    return format_;
}


/**
 * Sets the file path.
 *
 * @param pathToFile The new path.
 */
void
BlockImplementationFile::setPathToFile(const std::string& pathToFile) {
    file_ = pathToFile;
}


/**
 * Sets the format of the file.
 *
 * @param format The new format.
 */
void
BlockImplementationFile::setFormat(Format format) {
    format_ = format;
}
}
