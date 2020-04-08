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
 * @file ConstantNode.hh
 *
 * Declaration of ConstantNode class.
 *
 * Represents constant internal inputs of in operation DAG
 *
 * @author Mikael Lepistö 2008 (mikael.lepisto-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_CONSTANT_NODE_HH
#define TTA_CONSTANT_NODE_HH

#include "OperationDAGNode.hh"

/**
 * Constant input (not listed in IOs) node of the operation DAG.
 */
class ConstantNode : public OperationDAGNode {
public:

    ConstantNode(long value);

    ConstantNode(const ConstantNode& other);

    virtual ~ConstantNode() {}

    virtual GraphNode* clone() const; 

    virtual long value() const;

    virtual std::string toString() const;

private:
    long value_;
};

#endif
