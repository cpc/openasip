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
 * @file HWBlockImplementation.hh
 *
 * Declaration of HWBlockImplementation class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
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
    RowID id() const
        throw (NotAvailable);

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
    BlockImplementationFile& file(int index) const
        throw (OutOfRange);

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
