/**
 * @file GuardInverterModule.hh
 *
 * Declaration of GuardInverterModule
 *
 * Can convert xor 1 -> jump to !jump,removing the xor instruction.
 *
 * @author Heikki Kultala 2009 (hkultala@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_PRE_OPTIMIZER_MODULE_HH
#define TTA_PRE_OPTIMIZER_MODULE_HH

#include "StartableSchedulerModule.hh"

class PreOptimizer;

/**
 * module for ddg-based pre-optimizer.
 */
class PreOptimizerModule : public StartableSchedulerModule {
public:
    virtual ~PreOptimizerModule() {}

    virtual void start()        
        throw (Exception);
    bool needsProgram() const { return true; }
    bool needsTarget() const { return true; } // really would not need

    virtual std::string shortDescription() const;
    virtual std::string longDescription() const;
private:
};


#endif
