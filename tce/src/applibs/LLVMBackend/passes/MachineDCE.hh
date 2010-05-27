#include "llvm/CodeGen/Passes.h"
#include "llvm/Constant.h"
#include "llvm/Constants.h"
#include "llvm/Instructions.h"
#include "llvm/Function.h"
#include "llvm/Pass.h"
#include "llvm/Type.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/Support/CFG.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Target/TargetInstrInfo.h"
#include "llvm/ADT/DepthFirstIterator.h"

#include "llvm/Analysis/CallGraph.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/Analysis/Passes.h"
#include "llvm/Module.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"

#include "llvm/ADT/StringMap.h"

#include <map>

namespace llvm {    
  struct MachineDCE : public MachineFunctionPass {
    static char ID;
    MachineDCE() : MachineFunctionPass(&ID) {}
      
    typedef std::map<std::string, MachineFunction*> FunctionMap;
    typedef std::set<std::string> UserList;
    typedef std::map<std::string, UserList> UserRelations;
    typedef std::set<const std::string*> AvoidRecursionSet;

    /// Function name to MachineFunction map.
    FunctionMap functionMappings_;

    /// List of users of a symbol.
    UserRelations usersOfValue_;
      
    /// If users are traced to this list it means that function 
    /// cannot be eliminated.
    UserList baseUsers_;

    virtual bool doInitialization(Module &M);
    virtual bool runOnMachineFunction(MachineFunction &F);
    virtual bool doFinalization(Module &M);

    virtual const char *getPassName() const {
        return "TCE deadcode elimination of unused emulation functions";
    }

    bool canFindStart(const std::string& user, AvoidRecursionSet& avoid_recursion);
    void addInitializer(const Constant* init, std::string& name);

  public:
    typedef std::set<std::string> UnusedFunctionsList;
    UnusedFunctionsList removeableFunctions;
  };
}
