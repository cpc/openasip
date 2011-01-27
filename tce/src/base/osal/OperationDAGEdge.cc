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
/**
 * @file OperationDAGEdge.cc
 *
 * Implementation of operation directed acyclic graph edge.
 *
 * @author Mikael Lepistö 2007 (mikael.lepisto-no.spam-tut.fi)
 * @note rating: red
 */

#include "OperationDAGEdge.hh"
#include "Conversion.hh"

OperationDAGEdge::OperationDAGEdge(int srcOperand, int dstOperand) :
    GraphEdge(), srcOperand_(srcOperand), dstOperand_(dstOperand) {}

OperationDAGEdge::OperationDAGEdge(const OperationDAGEdge& other) :
    GraphEdge(other), srcOperand_(other.srcOperand()), 
    dstOperand_(other.dstOperand()) {}

GraphEdge* 
OperationDAGEdge::clone() const {
    return new OperationDAGEdge(*this);
}

int
OperationDAGEdge::srcOperand() const {
    return srcOperand_;
}

int
OperationDAGEdge::dstOperand() const {
    return dstOperand_;
}

TCEString
OperationDAGEdge::toString() const {
    return Conversion::toString(srcOperand_) + "," +
        Conversion::toString(dstOperand_);
}

