/**
 * @file SchedulingPlan.cc
 *
 * Implementation of SchedulingPlan class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#include <string>
#include <vector>

#include "SchedulingPlan.hh"
#include "SchedulerPluginLoader.hh"
#include "StartableSchedulerModule.hh"
#include "HelperSchedulerModule.hh"
#include "tce_config.h"

#ifdef PYTHON_ENABLED
#include "PythonSchedulerModule.hh"
#endif

#include "ObjectState.hh"
#include "Application.hh"
#include "SchedulerConfigurationSerializer.hh"

using std::string;
using std::vector;
/**
 * Constructor.
 */
SchedulingPlan::SchedulingPlan():
    pluginLoader_(&SchedulerPluginLoader::instance()) {
}

/**
 * Destructor.
 */
SchedulingPlan::~SchedulingPlan() {
}

/**
 * Constructs the scheduling chain from the given configuration.
 *
 * @todo Add support for local options for modules.
 *
 * @todo Support for multi-level recursive registration of helper modules.
 * Currently, helper modules can be only registered to the main module.
 *
 * @param conf The configuration.
 * @exception ObjectStateLoadingException If the configuration is invalid.
 * @exception DynamicLibraryException If there's an error loading a plugin
 *            module.
 */
void
SchedulingPlan::build(const ObjectState& conf)
    throw (ObjectStateLoadingException, DynamicLibraryException) {

    // load pass modules

    for (int i = 0; i < conf.childCount(); i++) {

        ObjectState* pass = conf.child(i);
        BaseSchedulerModule* mainModule = NULL;
        HelperList helpers;

        for (int j = 0; j < pass->childCount(); j++) {

            ObjectState* module = pass->child(j);
            string pluginName = "";
            string fileName = "";
            std::vector<ObjectState*> pluginOptions;

            for (int k = 0; k < module->childCount(); k++) {

                ObjectState* child = module->child(k);

                if (child->name() == "name") {
                    pluginName = child->stringValue();
                    continue;

                } else if (child->name() == "file") {
                    fileName = child->stringValue();
                    continue;

                } else if (child->name() == "option") {
                    pluginOptions.push_back(child);
                    continue;
                }
            }

            BaseSchedulerModule* base;

            // See if the file name ends with ".py"
            if (fileName.length() >= 3 && fileName.rfind(".py") == fileName.length() - 3) {
#ifdef PYTHON_ENABLED
              // This is a Python pass
              base = new PythonSchedulerModule(fileName);
              if (mainModule != NULL) {
                string method = "SchedulingPlan::build";
                string msg = "Found multiple main modules in pass "
                  "configuration.";
                throw ObjectStateLoadingException(
                                                  __FILE__, __LINE__, method, msg);
              }
              mainModule = base;
#else
              Application::logStream() 
                  << "Tried to load a .py module without the Python support."
                  << std::endl;
#endif

            } else {
              // This is a loadable plugin
              base =
                  &pluginLoader_->loadModule(pluginName, fileName, pluginOptions);

              if (base->isStartable()) {
                if (mainModule != NULL) {
                  string method = "SchedulingPlan::build";
                  string msg = "Found multiple main modules in pass "
                    "configuration.";
                  throw ObjectStateLoadingException(
                                                    __FILE__, __LINE__, method, msg);
                }
                mainModule = base;

              } else {
                HelperSchedulerModule* helper =
                  dynamic_cast<HelperSchedulerModule*>(base);
                if (helper == NULL) {
                  string method = "SchedulingPlan::build";
                  string msg = "Found non-startable module that is not a "
                    "helper module.";
                  throw ObjectStateLoadingException(
                                                    __FILE__, __LINE__, method, msg);
                } else {
                  helpers.push_back(helper);
                }
              }
            }
        }

        // register helper modules to main module
        // add here: multi-level recursive registration

        if (mainModule == NULL) {
            string method = "SchedulingPlan::build";
            string msg = "No main module found in pass configuration.";
            throw ObjectStateLoadingException(
                __FILE__, __LINE__, method, msg);
        }

        for (unsigned int i = 0; i < helpers.size(); i++) {
            try {
                mainModule->registerHelperModule(*helpers.at(i));
            } catch (const IllegalRegistration& e) {
                string method = "SchedulingPlan::build";
                string msg = "Error registering a helper module: ";
                throw ObjectStateLoadingException(
                    __FILE__, __LINE__, method, msg + e.errorMessage());
            }
        }
        passes_.push_back(
            dynamic_cast<StartableSchedulerModule*>(mainModule));
        helpers_.push_back(helpers);
    }
}

/**
 * Return the number of passes in the scheduling chain.
 *
 * @return The number of passes in the scheduling chain.
 */
int
SchedulingPlan::passCount() const {
    return passes_.size();
}

/**
 * Return the pass in the given index.
 *
 * @param index The index.
 * @return The pass in the given index.
 * @exception OutOfRange If the index is negative or exceeds the number
 *            of passes - 1.
 */
StartableSchedulerModule&
SchedulingPlan::pass(int index) const throw (OutOfRange) {
    if (index < 0 || static_cast<unsigned int>(index) >= passes_.size()) {
        string method = "SchedulingPlan::pass()";
        string msg = "Index out of bounds.";
        throw OutOfRange(__FILE__, __LINE__, method, msg);
    } else {
        return *passes_.at(index);
    }
}

/**
 * Return the number of helpers registered in the given main module.
 *
 * @return The number of helpers registered in the given main module.
 */
int
SchedulingPlan::helperCount(int index) const throw (OutOfRange) {
    if (index < 0 || static_cast<unsigned int>(index) >= passes_.size()) {
        string method = "SchedulingPlan::helperCount()";
        string msg = "Index out of bounds.";
        throw OutOfRange(__FILE__, __LINE__, method, msg);
    } else {
        return helpers_.at(index).size();
    }
}

/**
 * Return the helper module of the given pass in the given index.
 *
 * @param passIndex Pass index.
 * @param helperIndex Helper index.
 * @return The helper module of the given pass in the given index.
 * @exception OutOfRange If one index is negative or exceeds the number
 *            of passes/helpers - 1.
 */
HelperSchedulerModule&
SchedulingPlan::helper(int passIndex, int helperIndex) const
    throw (OutOfRange) {

    if (passIndex < 0 || helperIndex < 0 ||
        static_cast<unsigned int>(passIndex) >= passes_.size() ||
        static_cast<unsigned int>(helperIndex) >=
        helpers_.at(passIndex).size()) {

        string method = "SchedulingPlan::helperCount()";
        string msg = "Index out of bounds.";
        throw OutOfRange(__FILE__, __LINE__, method, msg);

    } else {
        return *helpers_.at(passIndex).at(helperIndex);
    }
}

/**
 * Loads a SchedulingPlan from the given configuration file.
 *
 * @param configurationFileName The name of the file to load the configuration
 * from.
 * @return A Scheduling plan instance.
 * @exception Exception In case some error occured. All exceptions are
 * generated by SchedulerConfigurationSerializer.
 */
SchedulingPlan*
SchedulingPlan::loadFromFile(const std::string& configurationFile)
    throw (Exception) {

    SchedulerConfigurationSerializer serializer;
    serializer.setSourceFile(configurationFile);
    return serializer.readConfiguration();
}
