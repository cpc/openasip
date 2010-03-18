/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file NullGuard.hh
 *
 * Declaration of NullGuard class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_NULL_GUARD_HH
#define TTA_NULL_GUARD_HH

#include "Guard.hh"

namespace TTAMachine {

/**
 * A null version of Guard class.
 */
class NullGuard : public Guard {
public:
    static NullGuard& instance();

    virtual Bus* parentBus() const;
    virtual bool isEqual(const Guard&) const;
    virtual bool isInverted() const;
    virtual bool isMoreRestrictive(const Guard&) const;
    virtual bool isLessRestrictive(const Guard&) const;
    virtual bool isDisjoint(const Guard&) const;
    virtual void copyTo(Bus&) const {
        abortWithError("Should not copy NullGuards!");
    }

    virtual bool isOpposite(const Guard&) const;
    virtual ObjectState* saveState() const;
    virtual void loadState(const ObjectState*)
        throw (ObjectStateLoadingException);

private:
    NullGuard();
    virtual ~NullGuard();

    /// The only instance of NullGuard.
    static NullGuard instance_;
    /// The parent bus.
    static Bus bus_;
};
}

#endif
