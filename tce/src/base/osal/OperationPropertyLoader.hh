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
 * @file OperationPropertyLoader.hh
 *
 * Declaration of OperationPropertyLoader.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @note rating: yellow
 * @note reviewed 7 September 2004 by pj, jn, jm, ao
 */

#ifndef TTA_OPERATION_PROPERTY_LOADER_HH
#define TTA_OPERATION_PROPERTY_LOADER_HH

#include <map>
#include <vector>
#include <string>

#include "OperationSerializer.hh"
#include "Exception.hh"

class Operation;
class ObjectState;
class OperationModule;

/**
 * Imports the static information contents of an operation from a data file.
 */
class OperationPropertyLoader {
public:
    OperationPropertyLoader();
    virtual ~OperationPropertyLoader();

    void loadOperationProperties(
        Operation& operation, const OperationModule& module);

private:
    /// Container containing already read ObjectState trees.
    typedef std::map<std::string, std::vector<ObjectState*> > ObjectStateCache;
    
    /// Iterator for map containing already read ObjectState trees.
    typedef ObjectStateCache::iterator MapIter;
    /// value_type for map containing already read ObjectState trees.
    typedef ObjectStateCache::value_type ValueType;

    /// Copying not allowed.
    OperationPropertyLoader(const OperationPropertyLoader&);
    /// Assignment not allowed.
    OperationPropertyLoader& operator=(const OperationPropertyLoader&);

    void loadModule(const OperationModule& module);

    /// Serializer instance.
    OperationSerializer serializer_;
    /// Cache for already read ObjectState trees.
    ObjectStateCache operations_;
};

#endif
