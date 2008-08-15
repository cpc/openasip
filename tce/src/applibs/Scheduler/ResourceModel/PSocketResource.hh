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
 * @file PSocketResource.hh
 *
 * Declaration of prototype of Resource Model:
 * declaration of the abstract PSocketResource class.
 *
 * @author Vladimir Guzma 2006 (vladimir.guzma@tut.fi)
 * @note rating: red
 */

#ifndef TTA_PSOCKETRESOURCE_HH
#define TTA_PSOCKETRESOURCE_HH

#include <string>
#include <map>

#include "SchedulingResource.hh"
/**
 * An interface for scheduling resources of Resource Model.
 *
 * The derived class PSocketResource represents a combined resource of
 * a FU/RF port and a socket that connects it to a bus.
 */

class PSocketResource : public SchedulingResource {
public:
    virtual ~PSocketResource();
    PSocketResource(const std::string& name);

    virtual bool isInUse(const int cycle) const;
    virtual bool isAvailable(const int cycle) const;
    virtual bool canAssign(const int cycle, const MoveNode& node) const;
    virtual void assign(const int cycle, MoveNode& node)
        throw (Exception);
    virtual void unassign(const int cycle, MoveNode& node)
        throw (Exception);

protected:

private:
    // Recording PSocket usage - cycle = value map
    typedef std::map<int, int> ResourceRecordType;
    // Copying forbidden
    PSocketResource(const PSocketResource&);
    // Assignment forbidden
    PSocketResource& operator=(const PSocketResource&);

    ResourceRecordType resourceRecord_;
};

#endif
