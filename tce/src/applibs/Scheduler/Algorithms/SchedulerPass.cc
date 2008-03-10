/**
 * @file SchedulerPass.cc
 *
 * Definition of SchedulerPass class.
 *
 * @author Pekka J‰‰skel‰inen 2006 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "SchedulerPass.hh"

/**
 * Constructor.
 *
 * @param data The container that stores data passed between scheduler passes.
 */
SchedulerPass::SchedulerPass(InterPassData& data) : data_(&data) {
}

/**
 * Returns the inter-pass data container that can be used by the pass.
 *
 * The object is a generic container for data passed between
 * scheduler passes.
 *
 * @param data The data object.
 */
InterPassData&
SchedulerPass::interPassData() {
    return *data_;
}

/**
 * A short description of the pass, usually the optimization name,
 * such as "basic block scheduler".
 *
 * @return The description as a string.
 */   
std::string
SchedulerPass::shortDescription() const {
    return "";
}

/**
 * Optional longer description of the pass.
 *
 * This description can include usage instructions, details of choice of
 * algorithmic details, etc.
 *
 * @return The description as a string.
 */
std::string
SchedulerPass::longDescription() const {
    return "";
}
