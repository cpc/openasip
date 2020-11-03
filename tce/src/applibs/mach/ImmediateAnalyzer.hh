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
 * @file ImmediateAnalyzer.hh
 *
 * Declaration of ImmediateAnalyzer class.
 *
 * Created on: 8.3.2016
 * @author Henry Linjamäki 2016 (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef IMMEDIATEANALYZER_HH
#define IMMEDIATEANALYZER_HH

namespace TTAMachine {
    class Machine;
    class FunctionUnit;
    class HWOperation;
}

class ImmInfo;

/*
 * Class that analyzes a machine for short immediate capabilities.
 *
 * TODO: result return desc
 */
class ImmediateAnalyzer {
public:
    ImmediateAnalyzer() = default;
    virtual ~ImmediateAnalyzer() = default;

    static ImmInfo* analyze(const TTAMachine::Machine& mach);

private:

    static void analyzeImmediateCapabilitiesForFU(
        const TTAMachine::FunctionUnit& fu,
        ImmInfo& result);

    static void analyzeImmediateCapabilitiesForOperation(
        const TTAMachine::HWOperation& hwop,
        ImmInfo& result);
};

#endif /* IMMEDIATEANALYZER_HH */
