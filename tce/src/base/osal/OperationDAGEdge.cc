/**
 * @file OperationDAGEdge.cc
 *
 * Implementation of operation directed acyclic graph edge.
 *
 * @author Mikael Lepistö 2007 (mikael.lepisto@tut.fi)
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

std::string
OperationDAGEdge::toString() const {
    return Conversion::toString(srcOperand_) + "," +
        Conversion::toString(dstOperand_);
}

