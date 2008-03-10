/**
 * @file TCESubtarget.h
 *
 * Declaration of TCESubtarget class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2007 (vjaaskel@cs.tut.fi)
 */

#ifndef TTA_TCE_SUBTARGET_H
#define TTA_TCE_SUBTARGET_H

#include <string>
#include "llvm/Target/TargetSubtarget.h"

namespace llvm {

    class Module;

    /**
     * Currently there is only one generic universal machine target,
     * so this class doesn't cotain any relevant information.
     */
    class TCESubtarget : public TargetSubtarget {
    public:
        TCESubtarget(const Module& m, const std::string& fs);
        std::string pluginFileName();
    private:
        std::string pluginFile_;

    };
}

#endif
