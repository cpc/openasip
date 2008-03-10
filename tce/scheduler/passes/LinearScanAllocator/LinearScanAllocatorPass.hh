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
private:
    LinearScanAllocatorCore allocatorCore_;
};



#endif
