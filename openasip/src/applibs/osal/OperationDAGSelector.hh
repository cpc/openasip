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
 * @file OperationDAGSelector.hh
 *
 * Declaration of OperationDAGSelector.
 *
 * @author Mikael Lepistö 2008 (mikael.lepisto-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_OPERATION_DAG_SELECTOR_HH
#define TTA_OPERATION_DAG_SELECTOR_HH

#include <string>
#include <vector>
#include <set>

#include "OperationDAG.hh"
#include "CIStringSet.hh"
#include "TCEString.hh"

class Operation;
class ImmInfo;

/**
 * Class that search DAGs from operation set.
 */
class OperationDAGSelector {
public:
    
    /**
     * STL Container for returning and finding DAGs.
     */
    class OperationDAGList : public std::vector<OperationDAG*> {
    public:
        
        /**
         * Returns DAG that had least nodes.
         *
         * @return DAG that had least nodes.
         */
        OperationDAG& smallestNodeCount() const {

            if (empty()) {
                return OperationDAG::null;
            }

            OperationDAG* leastNodes = *begin();

            for (std::vector<OperationDAG*>::const_iterator i = begin(); 
                 i != end(); i++) {
                
                if ((*i)->nodeCount() < leastNodes->nodeCount()) {
                    leastNodes = *i;
                }
            }
            
            return *leastNodes;
        }        
    };
    struct CaseInsensitiveCmp {
        bool operator()(const TCEString& s1, const TCEString& s2) const {
            return s1.ciEqual(s2);
        }
    };
    typedef TCETools::CIStringSet OperationSet;

    static OperationDAGList findDags(
        const std::string& opName,
        OperationSet opSet,
        const ImmInfo* immInfo = nullptr);
    
    static OperationDAG* createExpandedDAG(
        const Operation& op, OperationSet& opSet);
    
private:
    OperationDAGSelector();
    virtual ~OperationDAGSelector();

    static int countUnknownOperations(
        OperationDAG& dag, OperationSet& opSet);

};

#endif
