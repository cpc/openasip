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
 * @file ProgramDependenceEdge.hh
 *
 * Declaration of prototype of graph-based program representation:
 * declaration of the program dependence edge.
 *
 * @author Vladimir Guzma 2006 (vladimir.guzma-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROGRAM_DEPENDENCE_EDGE_HH
#define TTA_PROGRAM_DEPENDENCE_EDGE_HH

#include "GraphEdge.hh"
#include "ControlDependenceEdge.hh"
#include "DataDependenceEdge.hh"

class ProgramDependenceEdge : public GraphEdge {
public:
    ProgramDependenceEdge(
        ControlDependenceEdge& cEdge);

    ProgramDependenceEdge(
        DataDependenceEdge& cEdge);

    virtual ~ProgramDependenceEdge();

    bool isControlDependence() const;
    bool isDataDependence() const ;
    ControlDependenceEdge& controlDependenceEdge();
    DataDependenceEdge& dataDependenceEdge();
    TCEString toString() const;
    TCEString dotString() const;
    
private:
    ControlDependenceEdge* cEdge_;
    DataDependenceEdge* dEdge_;
};

#endif
