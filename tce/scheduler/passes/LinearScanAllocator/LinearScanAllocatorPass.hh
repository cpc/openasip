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
 * @file LinearScanAllocatorPass.hh
 *
 * Declaration of LinearScanAllocator class.
 * 
 * Linear scan register allocator
 *
 * @author Heikki Kultala 2006 (heikki.kultala@tut.fi)
 * @note rating: red
 */

#ifndef TTA_LINEAR_SCAN_ALLOCATOR_PASS_HH
#define TTA_LINEAR_SCAN_ALLOCATOR_PASS_HH

#include <map>
#include <vector>
#include <set>
#include <list>

#include "LinearScanAllocatorCore.hh"
#include "MoveNode.hh"
#include "StartableSchedulerModule.hh"

using std::map;
using std::vector;
using std::set;

namespace TTAProgram {
    class Move;
    class Procedure;
    class TerminalRegister;
}

class MoveNode;
class RegisterMap;
class StackManager;
class DataDependenceGraph;
class DataDependenceEdge;
class ControlFlowGraph;

class LinearScanAllocatorPass : public StartableSchedulerModule {
public:
    LinearScanAllocatorPass();
    virtual ~LinearScanAllocatorPass();
    virtual void start()        
        throw (Exception);
    bool needsProgram() const { return true; }
    bool needsTarget() const { return true; }
    virtual std::string shortDescription() const;
    virtual std::string longDescription() const;

private:
    LinearScanAllocatorCore allocatorCore_;
};



#endif
