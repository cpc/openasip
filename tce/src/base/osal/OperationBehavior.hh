/**
 * @file OperationBehavior.hh
 *
 * Declaration of OperationBehavior class.
 *
 * @author Pekka J‰‰skel‰inen 2004 (pekka.jaaskelainen@tut.fi)
 * @note rating: red
 * @note reviewed 19 August 2004 by pj, jn, ao, ac
 */


#ifndef TTA_OPERATION_BEHAVIOR_HH
#define TTA_OPERATION_BEHAVIOR_HH

#include <vector>
#include <string>
#include <iostream>

#include "Exception.hh"

class SimValue;
class OperationContext;



/**
 * Interface to access the behavior model of TTA operations.
 *
 * This abstract class that is inherited by the custom operation behavior 
 * classes. Instances of user defined OperationBehavior derived classes are 
 * loaded run-time from dynamic library modules (plugins). 
 *
 */
class OperationBehavior {
public:
    virtual bool simulateTrigger(
        SimValue** io,
        OperationContext& context) const = 0;

    virtual void createState(OperationContext& context) const;
    virtual void deleteState(OperationContext& context) const;

    virtual std::string stateName() const;

    virtual bool canBeSimulated() const;

    static std::ostream& outputStream();
    static void setOutputStream(std::ostream& newOutputStream);

    OperationBehavior();
    virtual ~OperationBehavior();

private:
    static std::ostream* outputStream_;
};

///////////////////////////////////////////////////////////////////////////////
// NullOperationBehavior
///////////////////////////////////////////////////////////////////////////////

/**
 * Singleton class to represent an undefined operation behavior.
 *
 * All methods cause program abort with an error log message.
 *
 */
class NullOperationBehavior : public OperationBehavior {
public:
    static NullOperationBehavior& instance();

    virtual bool simulateTrigger(
        SimValue**,
        OperationContext& context) const;
    virtual bool lateResult(
        SimValue**,
        OperationContext& context) const;

    virtual void createState(OperationContext& context) const;
    virtual void deleteState(OperationContext& context) const;

    virtual std::string stateName() const;

    virtual ~NullOperationBehavior();

    virtual bool canBeSimulated() const;

protected:
    NullOperationBehavior();

private: 
    static NullOperationBehavior instance_;

};

#include "OperationBehavior.icc"

#endif
