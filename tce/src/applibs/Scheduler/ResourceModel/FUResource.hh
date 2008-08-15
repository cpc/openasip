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
 * @file FUResource.hh
 *
 * Declaration of prototype of Resource Model:
 * declaration of the abstract FUResource class.
 *
 * @author Vladimir Guzma 2006 (vladimir.guzma@tut.fi)
 * @note rating: red
 */

#ifndef TTA_FURESOURCE_HH
#define TTA_FURESOURCE_HH

#include<string>

#include "SchedulingResource.hh"

/**
 * An interface for scheduling FU resources of Resource Model
 *
 */

class FUResource : public SchedulingResource {
public:
    FUResource(const std::string& name);
    virtual ~FUResource();

    virtual bool isInUse(const int cycle) const;
    virtual bool isAvailable(const int cycle) const;
    virtual bool canAssign(const int cycle, const MoveNode& node) const;
    virtual void assign(const int cycle, MoveNode& node)
        throw (Exception);
    virtual void unassign(const int cycle, MoveNode& node)
        throw (Exception);

private:
    // Copying forbidden
    FUResource(const FUResource&);
    // Assignment forbidden
    FUResource& operator=(const FUResource&);
};

#endif
