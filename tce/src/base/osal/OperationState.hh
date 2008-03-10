/**
 * @file OperationState.hh
 *
 * Declaration of OperationState class.
 *
 * @author Pekka J‰‰skel‰inen 2004 (pekka.jaaskelainen@tut.fi)
 * @note rating: red
 * @note reviewed 7 September 2004 by pj, jn, jm, ao
 */

#ifndef TTA_OPERATION_STATE_HH
#define TTA_OPERATION_STATE_HH

#include <string>
#include <vector>

class OperationContext;
class SimValue;

/**
 * OperationState is the base class of all operation state classes.
 *
 * Operation state classes store state information specific to a family of
 * operations. 
 */
class OperationState {
public:
    OperationState();
    virtual ~OperationState();

    virtual std::string name() = 0;
    virtual bool isAvailable(const OperationContext& context) const;
    virtual void advanceClock(OperationContext& context);
};

//////////////////////////////////////////////////////////////////////////////
// NullOperationState
//////////////////////////////////////////////////////////////////////////////

/**
 * Singleton class that is used to represent a null value for operation state.
 */
class NullOperationState : public OperationState {
public:
    static NullOperationState* instance();

    virtual std::string name();
    virtual bool isAvailable(const OperationContext& context) const;
    virtual void advanceClock(OperationContext& context);

private:
    static NullOperationState* instance_;
};

#include "OperationState.icc"

#endif
