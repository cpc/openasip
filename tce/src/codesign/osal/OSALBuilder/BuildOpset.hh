/**
 * @file BuildOpset.hh
 *
 * Declaration of classes needed by build_opset.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_BUILD_OPSET_HH
#define TTA_BUILD_OPSET_HH

#include <string>
#include "CmdLineOptions.hh"

/**
 * Class that handles options passed to build_opset.
 */
class BuildOpsetOptions : public CmdLineOptions {
public:
    BuildOpsetOptions();
    virtual ~BuildOpsetOptions();
    virtual void printVersion() const;

    std::string install() const;
    std::string sourceDir() const;
    bool ignore() const;
};

#endif
