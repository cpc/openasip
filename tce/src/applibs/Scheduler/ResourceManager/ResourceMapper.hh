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
