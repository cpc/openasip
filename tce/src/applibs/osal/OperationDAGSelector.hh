/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
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
#include "Operation.hh"

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
    
    /// @todo this should be gotten from tta machine applib, machine function
    /// for requesting native opset of a machine.
    typedef std::set<std::string> OperationSet;

    static OperationDAGList findDags(
        const std::string& opName, OperationSet opSet);
    
    static OperationDAG* createExpandedDAG(
        const Operation& op, OperationSet& opSet);
    
    static OperationSet llvmRequiredOpset();

private:
    OperationDAGSelector();
    virtual ~OperationDAGSelector();

    static int countUnknownOperations(
        OperationDAG& dag, OperationSet& opSet);

};

#endif
