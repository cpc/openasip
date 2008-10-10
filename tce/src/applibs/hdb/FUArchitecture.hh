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
 * @file FUArchitecture.hh
 *
 * Declaration of FUArchitecture class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_FU_ARCHITECTURE_HH
#define TTA_FU_ARCHITECTURE_HH

#include <string>
#include <set>
#include <map>
#include <vector>

#include "HWBlockArchitecture.hh"
#include "HDBTypes.hh"
#include "Exception.hh"

namespace TTAMachine {
    class FunctionUnit;
    class PipelineElement;
}

namespace HDB {

/**
 * Represents architecture of an FU in HDB.
 */
class FUArchitecture : public HWBlockArchitecture {
public:
    FUArchitecture(TTAMachine::FunctionUnit* fu);
    FUArchitecture(const FUArchitecture& o);
    virtual ~FUArchitecture();

    bool hasParameterizedWidth(const std::string& port) const;
    void setParameterizedWidth(const std::string& port);
    bool hasGuardSupport(const std::string& port) const;
    void setGuardSupport(const std::string& port);

    TTAMachine::FunctionUnit& architecture() const;
    HDB::Direction portDirection(const std::string& port) const
        throw (InstanceNotFound, InvalidData);
    
    bool operator==(const FUArchitecture& rightHand) const;
private:
    /// Struct PipelineElementUsage
    struct PipelineElementUsage {
        std::set<const TTAMachine::PipelineElement*> usage1;
        std::set<const TTAMachine::PipelineElement*> usage2;
    };

    /// typedef for PipelineElemetnUsageTable
    typedef std::vector<PipelineElementUsage> PipelineElementUsageTable;

    /// Set type for port names.
    typedef std::set<std::string> PortSet;

    /// The function unit.
    TTAMachine::FunctionUnit* fu_;
    /// Parameterized ports.
    PortSet parameterizedPorts_;
    /// Port that support guard.
    PortSet guardedPorts_;
};
}

#endif
