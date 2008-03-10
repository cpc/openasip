/**
 * @file OperationBehaviorLoader.hh
 *
 * Declaration of OperationBehaviorLoader class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: yellow
 * @note reviewed 19 August 2004 by pj, jn, ao, ac
 */

#ifndef TTA_OPERATION_BEHAVIOR_LOADER_HH
#define TTA_OPERATION_BEHAVIOR_LOADER_HH

#include <string>
#include <map>

#include "PluginTools.hh"

class OperationIndex;
class OperationBehavior;
class Operation;

/**
 * The purpose of this class is to import operation behavior models
 * from dynamic modules.
 *
 * PluginTools is used to import the operation behavior modules.
 * OperationIndex is used to obtain modules in which certain operation
 * belongs to.
 */
class OperationBehaviorLoader {
public:
    explicit OperationBehaviorLoader(OperationIndex& index);
    virtual ~OperationBehaviorLoader();

    OperationBehavior& importBehavior(const Operation& parent)
        throw (DynamicLibraryException, InstanceNotFound);

private:
    /// Contains operation behavior models indexed by operation names.
    typedef std::map<std::string, OperationBehavior*> BehaviorMap;
    /// Contains all deletion function of operation behavior models.
    typedef std::map<OperationBehavior*, void (*)(OperationBehavior*)>
        DestructionMap;
    
    /// Copying not allowed.
    OperationBehaviorLoader(const OperationBehaviorLoader&);
    /// Assignment not allowed.
    OperationBehaviorLoader& operator=(const OperationBehaviorLoader&);

    void freeBehavior();

    /// The name of the creation function in dynamic module.
    static const std::string CREATE_FUNC;
    /// The name of the deletion function in dynamic module.
    static const std::string DELETE_FUNC;

    /// Indexed table of all modules and operations accessible for this loader.
    OperationIndex& index_;
    /// PluginTools for loading dynamic modules.
    PluginTools tools_;
    /// Container of all loaded operation behavior models.
    BehaviorMap behaviors_;
    /// Container of all destruction functions of behavioral models.
    DestructionMap destructors_;
};

#endif
