/**
 * @file SchedulingScope.hh
 *
 * Declaration of SchedulingScopeSelector interface.
 *
 * @author Pekka J‰‰skel‰inen 2006 (pekka.jaaskelainen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_SCHEDULING_SCOPE_HH
#define TTA_SCHEDULING_SCOPE_HH

/**
 * An interface for different types of scheduling scopes (e.g. Basic Block).
 */
class SchedulingScope {
public:
    SchedulingScope();
    virtual ~SchedulingScope();
};

#endif
