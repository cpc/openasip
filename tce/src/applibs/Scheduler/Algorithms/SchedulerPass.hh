/**
 * @file SchedulerPass.hh
 *
 * Declaration of SchedulerPass interface.
 *
 * @author Pekka J‰‰skel‰inen 2007 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_SCHEDULER_PASS_HH
#define TTA_SCHEDULER_PASS_HH

class InterPassData;

#include <string>

/**
 * Interface for scheduler passes with different scopes.
 */
class SchedulerPass {
public:
    SchedulerPass(InterPassData& data);
    virtual ~SchedulerPass() {};

    InterPassData& interPassData();

    virtual std::string shortDescription() const = 0;
    virtual std::string longDescription() const;
private:
    /// arbitrary data stored by different passes for inter-pass communication
    InterPassData* data_;
};
#endif
