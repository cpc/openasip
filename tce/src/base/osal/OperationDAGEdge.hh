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
 * @file OperationDAGEdge.hh
 *
 * Declaration of operation directed acyclic graph edge.
 *
 * @author Mikael Lepistö 2007 (mikael.lepisto-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_OPERATION_DAG_EDGE_HH
#define TTA_OPERATION_DAG_EDGE_HH

#include "GraphEdge.hh"

class OperationDAGEdge : public GraphEdge {
public:
    OperationDAGEdge(int srcOperand, int dstOperand);
    OperationDAGEdge(const OperationDAGEdge& other);
    virtual ~OperationDAGEdge() {} 
    
    virtual GraphEdge* clone() const;
    
    int srcOperand() const;
    int dstOperand() const;

    TCEString toString() const;    

private:
    int srcOperand_;
    int dstOperand_;
};

#endif
