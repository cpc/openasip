/*
    Copyright (c) 2002-2009 Tampere University.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
 */
/**
 * @file OperationBehavior.hh
 *
 * Declaration of OperationBehavior class.
 *
 * @author Pekka J‰‰skel‰inen 2004 (pekka.jaaskelainen-no.spam-tut.fi)
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
