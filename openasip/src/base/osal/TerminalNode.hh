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
 * @file TerminalNode.hh
 *
 * Declaration of TerminalNode class.
 *
 * Represents input and output values of operation, 
 * which can be reconnected to other operations.
 *
 * @author Mikael Lepistö 2007 (mikael.lepisto-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_TERMINAL_NODE_HH
#define TTA_TERMINAL_NODE_HH

#include "OperationDAGNode.hh"

/**
 * Input and output node of the operation DAG.
 *
 * A MoveNode represents one input or output value of 
 * an operation.
 */
class TerminalNode : public OperationDAGNode {
public:

    TerminalNode(int operandIndex);
    TerminalNode(const TerminalNode& other);

    virtual ~TerminalNode() {}

    virtual GraphNode* clone() const; 

    virtual int operandIndex() const;
    
    virtual std::string toString() const;

private:
    int operandIndex_;
};

#endif
