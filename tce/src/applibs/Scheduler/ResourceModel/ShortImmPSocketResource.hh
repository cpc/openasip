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
 * @file ShortImmPSocketResource.hh
 *
 * declaration of the ShortImmPSocketResource class.
 *
 * @author Ari Metsähalme 2006 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#ifndef TTA_SHORTIMMPSOCKETRESOURCE_HH
#define TTA_SHORTIMMPSOCKETRESOURCE_HH

#include "OutputPSocketResource.hh"

/**
 * An interface for scheduling resources of Resource Model
 * The derived class ShortImmPSocketResource
 */

class ShortImmPSocketResource : public OutputPSocketResource {
public:
    virtual ~ShortImmPSocketResource();
    ShortImmPSocketResource(
        const std::string& name,
        int immediateWidth,
        bool signExtends);

    virtual bool isShortImmPSocketResource() const;

    int immediateWidth() const;
    bool signExtends() const;
    bool zeroExtends() const;

protected:
    virtual bool validateRelatedGroups();

private:
    /// Copying forbidden.
    ShortImmPSocketResource(const ShortImmPSocketResource&);
    /// Assignment forbidden.
    ShortImmPSocketResource& operator=(const ShortImmPSocketResource&);

    /// Width of immediates carried by the related bus.
    int immediateWidth_;
    /// Extension mode of related bus.
    bool signExtends_;
};

#endif
