/**
 * @file BlockImplementationFile.hh
 *
 * Declaration of BlockImplementationFile class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_BLOCK_IMPLEMENTATION_FILE_HH
#define TTA_BLOCK_IMPLEMENTATION_FILE_HH

#include <string>

namespace HDB {

/**
 * Represents a file that contains implementation for a block in HDB.
 */
class BlockImplementationFile {
public:
    /// Format of the file.
    enum Format {
        VHDL ///< VHDL file.
    };

    BlockImplementationFile(const std::string& pathToFile, Format format);
    virtual ~BlockImplementationFile();
    
    std::string pathToFile() const;
    Format format() const;

    void setPathToFile(const std::string& pathToFile);
    void setFormat(Format format);

private:
    /// The file.
    std::string file_;
    /// Format of the file.
    Format format_;
};
}

#endif
