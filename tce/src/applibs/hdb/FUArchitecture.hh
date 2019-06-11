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
