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
 * @file OperationSimulator.cc
 *
 * Declaration of OperationSimulator class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_OPERATION_SIMULATOR_HH
#define TTA_OPERATION_SIMULATOR_HH

#include <string>
#include <vector>

#include "Operation.hh"
#include "OperationContext.hh"
#include "DataObject.hh"
#include "SimValue.hh"

/**
 * Contains functionality to simulate operation behavior.
 */
class OperationSimulator {
public:
    static OperationSimulator& instance();

    bool simulateTrigger(
        Operation& op,
        std::vector<DataObject> inputs,
        std::vector<SimValue*>& outputs,
        OperationContext& context,
        unsigned int bitWidth,
        std::string& result);

    bool lateResult(
        Operation& op,
        std::vector<DataObject> inputs,
        std::vector<SimValue*>& outputs,
        OperationContext& context,
        unsigned int bitWidth,
        std::string& result);

    void advanceClock(OperationContext& context);

    bool initializeSimValue(
        std::string value,
        SimValue* sim,
        std::string& result);

private:
    OperationSimulator();
    virtual ~OperationSimulator();

    bool initializeOutputs(
        Operation& op,
        std::vector<DataObject> inputs,
        std::vector<SimValue*>& outpus,
        unsigned int bitWidth,
        std::string& result);

    /// Unique instance.
    static OperationSimulator* instance_;
};

#endif
