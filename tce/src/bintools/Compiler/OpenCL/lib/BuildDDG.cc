#include "llvm/BasicBlock.h"
#include "llvm/Function.h"
#include "llvm/Pass.h"

#define BOOST_NO_HASH

#include "boost/graph/graphviz.hpp"
#include <map>
#include <sstream>

#include "config.h"

using namespace llvm;
using namespace boost;

namespace boost {
  enum vertex_label_t {vertex_label };
  enum vertex_comment_t { vertex_comment };

  BOOST_INSTALL_PROPERTY (vertex, label);
  BOOST_INSTALL_PROPERTY (vertex, comment);
}

namespace {

  struct BuildDDG : public BasicBlockPass {

    static char ID;
#ifdef LLVM_2_7
    BuildDDG() : BasicBlockPass (&ID) {}
#else
    BuildDDG() : BasicBlockPass (ID) {}
#endif

    virtual bool runOnBasicBlock (BasicBlock &B) {
      typedef adjacency_list <vecS, vecS, directedS,
        property <vertex_label_t, std::string,
        property <vertex_comment_t, std::string> > > Graph;

      Graph g (0);

      std::map <const Instruction*, Graph::vertex_descriptor> i2v;

      /* Create a vertice for each instruction in the BB.  */
      for (BasicBlock::const_iterator i = B.begin(), e = B.end();
           i != e; ++i)
        {
          i2v[i] = add_vertex (g);

          put (vertex_label, g, i2v[i], i->getOpcodeName());

          std::stringstream s;

// following not used? so commented out.
//	  const Instruction& ins = *i;
//          s << ins;
	  // TODO: THE ABOVE LINE COMMENTED OUT BROKE THIS? HOW TO DO
	  // IN LLVM 2.7????
          put (vertex_comment, g, i2v[i], s.str ());
        }

      /* Now create the links (edges).  */
      for (BasicBlock::const_iterator i = B.begin(), e = B.end();
           i != e; ++i)
        {
#ifdef LLVM_2_7
          for (Instruction::use_const_iterator i2 = i->use_begin(),
#else
          for (Instruction::const_use_iterator i2 = i->use_begin(),
#endif
                 e2 = i->use_end();
               i2 != e2; ++i2)
            {
#ifdef LLVM_2_7
              const Instruction *user = dyn_cast <Instruction> (i2);
#else
              const Instruction *user = dyn_cast <Instruction> (*i2);
#endif
              if (user != NULL)
                {
                  if (user->getParent() == &B)
                    add_edge (i2v[i], i2v[user], g);
                }
            }
        }

      dynamic_properties dp;
      dp.property ("label", get (vertex_label, g));
      dp.property ("comment", get (vertex_comment, g));
      dp.property ("node_id", get (vertex_index, g));

      std::string s (B.getParent()->getNameStr() + B.getNameStr() + ".dot");
      std::ofstream o (s.c_str ());
      write_graphviz (o, g, dp);

      return false;
    }
  };

  char BuildDDG::ID = 0;
  RegisterPass <BuildDDG> X ("build-ddg", "Build (and dump) basic block DDG");
}
