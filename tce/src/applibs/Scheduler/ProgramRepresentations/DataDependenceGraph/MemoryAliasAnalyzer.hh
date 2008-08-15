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
/*
 * @file MemoryAliasAnalyzer.hh
 *
 * Declaration of Memory Alias Analyzer interface
 *
 * @author Heikki Kultala 2006-2007 (heikki.kultala@tut.fi)
 * @note rating: red
 */

#ifndef TTA_MEMORY_ALIAS_ANALYZER_HH
#define TTA_MEMORY_ALIAS_ANALYZER_HH

namespace TTAProgram {
    class Procedure;
}

class MoveNode;
class DataDependenceGraph;

class MemoryAliasAnalyzer {
public:

    enum AliasingResult { ALIAS_FALSE = 0,
                          ALIAS_TRUE  = 1,
                          ALIAS_UNKNOWN = 2 };

    virtual void initProcedure(TTAProgram::Procedure&) {}

    virtual AliasingResult analyze(
        DataDependenceGraph& ddg, const MoveNode& node1, 
        const MoveNode& node2) = 0;

    /**
     * Checks whether the analyzer knows anything about the address.
     * 
     * ie. if it can return true or false to some query 
     * concerning this address.
     * 
     * @return true if analyzer can know something about the address.
     */
    virtual bool addressTraceable(
        const MoveNode& mn) = 0;

    virtual ~MemoryAliasAnalyzer() {}
};

#endif
