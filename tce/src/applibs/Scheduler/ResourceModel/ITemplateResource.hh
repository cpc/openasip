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
 * @file ITemplateResource.hh
 *
 * Declaration of prototype of Resource Model:
 * declaration of the ITemplateResource class.
 *
 * @author Vladimir Guzma 2006 (vladimir.guzma-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_ITEMPLATERESOURCE_HH
#define TTA_ITEMPLATERESOURCE_HH

#include <string>
#include <map>

#include "SchedulingResource.hh"

/**
 * An interface for scheduling resources of Resource Model
 * The derived class ITemplateResource
 */

class ITemplateResource : public SchedulingResource {
public:
    virtual ~ITemplateResource();
    ITemplateResource(const std::string& name, unsigned int initiationInterval = 0);

    virtual bool isInUse(const int cycle) const;
    virtual bool isAvailable(const int cycle) const;
    virtual bool canAssign(const int cycle, const MoveNode& node) const;
    virtual bool canAssign(const int cycle) const;
    virtual void assign(const int cycle, MoveNode& node)
        throw (Exception);
    virtual void assign(const int cycle)
        throw (Exception);
    virtual void unassign(const int cycle, MoveNode& node)
        throw (Exception);
    virtual void unassign(const int cycle)
        throw (Exception);
    virtual bool isITemplateResource() const;
    void clear();

protected:
    virtual bool validateDependentGroups();
    virtual bool validateRelatedGroups();

private:
    //map contains <Cycle, testCounter>
    typedef std::map<int, int> ResourceRecordType;
    // Copying forbidden
    ITemplateResource(const ITemplateResource&);
    // Assignment forbidden
    ITemplateResource& operator=(const ITemplateResource&);

    ResourceRecordType resourceRecord_;
};

#endif
