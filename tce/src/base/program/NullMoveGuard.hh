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
 * @file NullMoveGuard.hh
 *
 * Declaration of NullMoveGuard class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#ifndef TTA_NULL_MOVEGUARD_HH
#define TTA_NULL_MOVEGUARD_HH

#include "MoveGuard.hh"

namespace TTAMachine {
    class Guard;
}

namespace TTAProgram {

/**
 * A singleton class that represents a null guard.
 *
 * Calling any method causes the program to abort.
 */
class NullMoveGuard : public MoveGuard {
public:
    virtual ~NullMoveGuard();
    static NullMoveGuard& instance();

    bool isUnconditional() const;
    bool isInverted() const;
    TTAMachine::Guard& guard() const;

protected:
    NullMoveGuard();

private:
    /// Copying not allowed.
    NullMoveGuard(const NullMoveGuard&);
    /// Assignment not allowed.
    NullMoveGuard& operator=(const NullMoveGuard&);

    /// Unique instance of NullMoveGuard.
    static NullMoveGuard instance_;
};

}

#endif
