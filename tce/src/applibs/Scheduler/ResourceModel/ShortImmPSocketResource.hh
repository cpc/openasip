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
 * @file ShortImmPSocketResource.hh
 *
 * declaration of the ShortImmPSocketResource class.
 *
 * @author Ari Metsähalme 2006 (ari.metsahalme-no.spam-tut.fi)
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
        bool signExtends,
        unsigned int initiationInterval = 0);

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
