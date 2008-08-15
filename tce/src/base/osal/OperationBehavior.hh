/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file OperationBehavior.hh
 *
 * Declaration of OperationBehavior class.
 *
 * @author Pekka J‰‰skel‰inen 2004 (pekka.jaaskelainen@tut.fi)
 * @note This file is used in compiled simulation. Keep dependencies *clean*
 * @note rating: red
 * @note reviewed 19 August 2004 by pj, jn, ao, ac
 */


#ifndef TTA_OPERATION_BEHAVIOR_HH
#define TTA_OPERATION_BEHAVIOR_HH

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

    virtual const char* stateName() const;

    virtual bool canBeSimulated() const;

    virtual void writeOutput(const char* text) const;

    OperationBehavior();
    virtual ~OperationBehavior();
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
    static NullOperationBehavior& instance() { return instance_; }
    
    virtual bool simulateTrigger(
        SimValue**,
        OperationContext& context) const;
    virtual bool lateResult(
        SimValue**,
        OperationContext& context) const;
    
protected:
    NullOperationBehavior() { }

private: 
    static NullOperationBehavior instance_;

};

#endif
