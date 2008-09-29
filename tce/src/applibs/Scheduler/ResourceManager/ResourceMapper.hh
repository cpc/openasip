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
 * @file ResourceMapper.hh
 *
 * Declaration of ResourceMapper class.
 *
 * @author Ari Metsähalme 2006 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_RESOURCE_MAPPER_HH
#define TTA_RESOURCE_MAPPER_HH

#include <vector>

#include "Exception.hh"

namespace TTAMachine {
    class MachinePart;
}

class ResourceBroker;
class SchedulingResource;

/**
 * Groups together the brokers managed by the resource build director and
 * maps each resource object to corresponding machine parts.
 */
class ResourceMapper {
public:
    ResourceMapper();
    virtual ~ResourceMapper();

    void addResourceMap(const ResourceBroker& broker)
        throw (ObjectAlreadyExists);
    int resourceCount(const TTAMachine::MachinePart& mp) const;
    SchedulingResource& resourceOf(
        const TTAMachine::MachinePart& mp,
        int index = 0) const throw (KeyNotFound);

private:
    /// Resource brokers.
    std::vector<const ResourceBroker*> brokers_;
};

#endif
