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
/*
 * @file MemoryAliasAnalyzer.hh
 *
 * Declaration of Memory Alias Analyzer interface
 *
 * @author Heikki Kultala 2006-2009 (heikki.kultala-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_MEMORY_ALIAS_ANALYZER_HH
#define TTA_MEMORY_ALIAS_ANALYZER_HH

namespace TTAProgram {
    class Procedure;
}

class ProgramOperation;
class DataDependenceGraph;
class Operation;
class MoveNode;

class MemoryAliasAnalyzer {
public:

    enum AliasingResult { ALIAS_FALSE = 0,
                          ALIAS_TRUE  = 1,
                          ALIAS_UNKNOWN = 2 };

    virtual void initProcedure(TTAProgram::Procedure&) {}

    virtual AliasingResult analyze(
        DataDependenceGraph& ddg, const ProgramOperation& pop1, 
        const ProgramOperation& pop2) = 0;

    /**
     * Checks whether the analyzer knows anything about the address.
     * 
     * ie. if it can return true or false to some query 
     * concerning this address.
     * 
     * @return true if analyzer can know something about the address.
     */
    virtual bool isAddressTraceable(
        DataDependenceGraph& ddg,
        const ProgramOperation& pop) = 0;

    virtual ~MemoryAliasAnalyzer() {}
protected:
    AliasingResult compareIndeces(
        int index1, 
        int index2, 
        const ProgramOperation& pop1, 
        const ProgramOperation& pop2);
    const MoveNode* addressOperandMove(const ProgramOperation&po);

    struct TwoPartAddressOperandDetection {
        enum OffsetOperation { NOT_FOUND, ADD, SUB };
        OffsetOperation offsetOperation;
        int operand1;
        int operand2;
        TwoPartAddressOperandDetection() : 
            offsetOperation(NOT_FOUND), operand1(0), operand2(0) {}

        void clear() {
            offsetOperation = NOT_FOUND;
            operand1 = 0;
            operand2 = 0;
        }
    };

    TwoPartAddressOperandDetection findTwoPartAddressOperands(
        const ProgramOperation& po);

private:
    unsigned int mausOfOperation(const Operation& op);
};

#endif
