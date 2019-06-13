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
 * @file OperationModule.hh
 *
 * Declaration of OperationModule class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @note rating: yellow
 * @note reviewed 7 September 2004 by pj, jn, jm, ao
 */

#ifndef TTA_OPERATION_MODULE_HH
#define TTA_OPERATION_MODULE_HH

#include <string>

#include "Exception.hh"
#include "FileSystem.hh"

/**
 * Class that represents the name and the path of external files where
 * Operation definitions are stored.
 */
class OperationModule {
public:

    OperationModule(const std::string& name, const std::string& path);
    OperationModule(const OperationModule& om);
    virtual ~OperationModule();

    OperationModule& operator=(const OperationModule& om);

    virtual std::string name() const;
    virtual bool definesBehavior() const;
    virtual std::string behaviorModule() const;
    virtual std::string propertiesModule() const;
    virtual bool hasBehaviorSource() const;
    virtual std::string behaviorSourceModule() const;

private:
    std::string propertyFileName() const;
    std::string behaviorFileName() const;
    std::string behaviorSourceFileName() const;

    /// File extension of operation property file.
    static const std::string PROPERTY_FILE_EXTENSION;
    /// File extension of operation behavior file.
    static const std::string BEHAVIOR_FILE_EXTENSION;
    /// File extension of operation behavior source file.
    static const std::string BEHAVIOR_SOURCE_FILE_EXTENSION;
    
    /// The path of the module.
    std::string path_;
    /// The name of the module.
    std::string name_;
};

//////////////////////////////////////////////////////////////////////////////
// NullOperationModule
//////////////////////////////////////////////////////////////////////////////

/**
 * Singleton class that is used to represent a null value for operation state.
 *
 */
class NullOperationModule : public OperationModule {
public:

    static NullOperationModule& instance();

    virtual ~NullOperationModule();

    virtual std::string name() const;
    virtual bool definesBehavior() const;
    virtual std::string behaviorModule() const;
    virtual std::string propertiesModule() const;
    virtual bool hasBehaviorSource() const;
    virtual std::string behaviorSourceModule() const;

private:
    NullOperationModule();
    NullOperationModule(const NullOperationModule& om);
    NullOperationModule& operator=(const NullOperationModule& om);

    /// Unique instance of NullOperationModule.
    static NullOperationModule instance_;
};

#include "OperationModule.icc"

#endif
