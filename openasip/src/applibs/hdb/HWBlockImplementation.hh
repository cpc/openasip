/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file HWBlockImplementation.hh
 *
 * Declaration of HWBlockImplementation class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_HW_BLOCK_IMPLEMENTATION_HH
#define TTA_HW_BLOCK_IMPLEMENTATION_HH

#include <string>
#include <vector>

#include "Exception.hh"
#include "DBTypes.hh"

namespace HDB {

class BlockImplementationFile;

/**
 * Base class for FUImplementation and RFImplementation.
 */
class HWBlockImplementation {
public:
    HWBlockImplementation(
        const std::string& moduleName,
        const std::string& clkPort,
        const std::string& rstPort,
        const std::string& glockPort);
    
    HWBlockImplementation(const HWBlockImplementation& b);

    virtual ~HWBlockImplementation();

    bool hasID() const;
    void setID(RowID id);
    RowID id() const;

    void setModuleName(const std::string& name);
    std::string moduleName() const;
    void setClkPort(const std::string& name);
    std::string clkPort() const;
    void setRstPort(const std::string& name);
    std::string rstPort() const;
    void setGlockPort(const std::string& name);
    std::string glockPort() const;

    void addImplementationFile(BlockImplementationFile* file);

    void removeImplementationFile(const BlockImplementationFile& file);
    int implementationFileCount() const;
    BlockImplementationFile& file(int index) const;

private:
    /// Vector type for BlockImplementationFile.
    typedef std::vector<BlockImplementationFile*> FileTable;

    /// Name of the module.
    std::string moduleName_;
    /// Name of the clock port.
    std::string clkPort_;
    /// Name of the reset port.
    std::string rstPort_;
    /// Name of the global lock port.
    std::string glockPort_;
    /// Contains the block implementation files.
    FileTable files_;
    /// Tells whether the implementation has an ID.
    bool hasID_;

    /// ID of the implementation.
    RowID id_;
};
}

#endif
