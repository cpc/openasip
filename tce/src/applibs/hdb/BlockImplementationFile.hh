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
 * @file BlockImplementationFile.hh
 *
 * Declaration of BlockImplementationFile class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
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
        VHDL, ///< VHDL file.
        Verilog ///< Verilog file.
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
