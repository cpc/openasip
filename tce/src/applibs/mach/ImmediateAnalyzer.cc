/*
 Copyright (c) 2002-2016 Tampere University.

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
/*
 * @file ImmediateAnalyzer.cc
 *
 * Implementation of ImmediateAnalyzer class.
 *
 * Created on: 8.3.2016
 * @author Henry Linjamäki 2016 (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#include "ImmediateAnalyzer.hh"

#include <memory>
#include <utility>
#include <set>

#include "ImmInfo.hh"

#include "Machine.hh"
#include "FunctionUnit.hh"
#include "ControlUnit.hh"
#include "HWOperation.hh"
#include "MachineConnectivityCheck.hh"
#include "FUPort.hh"

#include "TCEString.hh"

using namespace TTAMachine;


/**
 * Collects short immediate capabilities of the machine.
 *
 * @param mach The machine.
 */
ImmInfo*
ImmediateAnalyzer::analyze(const TTAMachine::Machine& mach) {
    std::unique_ptr<ImmInfo> result(new ImmInfo);

    for (const FunctionUnit* fu : mach.functionUnitNavigator()) {
        analyzeImmediateCapabilitiesForFU(*fu, *result);
    }

    analyzeImmediateCapabilitiesForFU(*mach.controlUnit(), *result);
    // ^ todo: fix this. operations of CU are not getting included in analysis.

    //todo: remove duplicates (entries with identical (key, value pairs)).


    return result.release();
}


/**
 * Collects short immediate capabilities for each operation in the FU.
 *
 * @param fu The function unit.
 * @param result The short immediate analysis result object where the
 *               capabilities are inserted into.
 */
void
ImmediateAnalyzer::analyzeImmediateCapabilitiesForFU(
    const TTAMachine::FunctionUnit& fu, ImmInfo& result) {

    for (int i = 0; i < fu.operationCount(); i++) {
        HWOperation& hwop = *fu.operation(i);
        analyzeImmediateCapabilitiesForOperation(hwop, result);
    }
}


/**
 * Collects immediate transport capabilities for the operation.
 *
 * @param hwop The operation.
 * @param result The short immediate analysis result object where the
 *               capabilities are inserted into.
 */
void
ImmediateAnalyzer::analyzeImmediateCapabilitiesForOperation(
    const TTAMachine::HWOperation& hwop, ImmInfo& result) {

    using std::make_pair;

    for (int i = 0; i < hwop.operandCount(); i++) {
        assert(hwop.port(i+1) != nullptr);
        const FUPort& boundPort = *hwop.port(i+1);
    const Machine& mach = *boundPort.parentUnit()->machine();
        std::set<const TTAMachine::Bus*> connectedBuses =
            MachineConnectivityCheck::connectedSourceBuses(boundPort);
        for (const Bus* bus : connectedBuses) {
            if (bus->immediateWidth() < 1) {
                continue;
            }
            result.insert(
                make_pair(
                    make_pair(TCEString::toUpper(hwop.name()), i+1),
                    ImmInfoValue(bus->immediateWidth(), bus->signExtends())));
        }

        for (const ImmediateUnit* iu : mach.immediateUnitNavigator()) {
            if (MachineConnectivityCheck::isConnected(*iu, boundPort)) {
                for (auto& it : mach.instructionTemplateNavigator()) {
                    int supportedWidth = it->supportedWidth(*iu);
                    if (supportedWidth < 1) {
                        continue;
                    }

                    result.insert(
                        make_pair(
                            make_pair(TCEString::toUpper(hwop.name()), i+1),
                            ImmInfoValue(supportedWidth, iu->signExtends())));
                }
            }
        }
    }
}



