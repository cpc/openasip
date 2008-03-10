/**
 * @file SoftwareBypasser.hh
 *
 * Declaration of SoftwareBypasser interface.
 *
 * @author Pekka J‰‰skel‰inen 2007 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_SOFTWARE_BYPASSER_HH
#define TTA_SOFTWARE_BYPASSER_HH

#include "HelperSchedulerModule.hh"

class MoveNodeGroup;
class DataDependenceGraph;
class ResourceManager;
class MoveNodeSelector;

/**
 * Classes that implement this interface are able to software bypass
 * moves.
 */
class SoftwareBypasser {
public:
    SoftwareBypasser();
    virtual ~SoftwareBypasser();

    virtual int bypass(
        MoveNodeGroup& candidates,
        DataDependenceGraph& ddg,
        ResourceManager& rm) = 0;
    virtual void removeBypass(
        MoveNodeGroup& candidates,
        DataDependenceGraph& ddg,
        ResourceManager& rm) = 0;
    virtual int removeDeadResults(
        MoveNodeGroup& candidates,
        DataDependenceGraph& ddg,
        ResourceManager& rm) = 0;

    virtual void setSelector(MoveNodeSelector* selector);
};

#endif


