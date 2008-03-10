/**
 * @file OperationModule.hh
 *
 * Declaration of OperationModule class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
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
    virtual std::string behaviorModule() const
        throw (FileNotFound);
    virtual std::string propertiesModule() const;
    virtual bool hasBehaviorSource() const;
    virtual std::string behaviorSourceModule() const
        throw (FileNotFound);

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
    virtual std::string behaviorModule() const
        throw (FileNotFound);
    virtual std::string propertiesModule() const;
    virtual bool hasBehaviorSource() const;
    virtual std::string behaviorSourceModule() const
	throw (FileNotFound);

private:

    NullOperationModule();
    NullOperationModule(const NullOperationModule& om);
    NullOperationModule& operator=(const NullOperationModule& om);

    /// Unique instance of NullOperationModule.
    static NullOperationModule instance_;
};

#include "OperationModule.icc"

#endif
