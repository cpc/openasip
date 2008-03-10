/**
 * @file OperationSimulator.cc
 *
 * Declaration of OperationSimulator class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_OPERATION_SIMULATOR_HH
#define TTA_OPERATION_SIMULATOR_HH

#include <string>
#include <vector>

#include "Operation.hh"
#include "OperationContext.hh"
#include "DataObject.hh"
#include "SimValue.hh"

/**
 * Contains functionality to simulate operation behavior.
 */
class OperationSimulator {
public:
    static OperationSimulator& instance();

    bool simulateTrigger(
        Operation& op,
        std::vector<DataObject> inputs,
        std::vector<SimValue*>& outputs,
        OperationContext& context,
        unsigned int bitWidth,
        std::string& result);

    bool lateResult(
        Operation& op,
        std::vector<DataObject> inputs,
        std::vector<SimValue*>& outputs,
        OperationContext& context,
        unsigned int bitWidth,
        std::string& result);

    void advanceClock(OperationContext& context);

    bool initializeSimValue(
        std::string value,
        SimValue* sim,
        std::string& result);

private:
    OperationSimulator();
    virtual ~OperationSimulator();

    bool initializeOutputs(
        Operation& op,
        std::vector<DataObject> inputs,
        std::vector<SimValue*>& outpus,
        unsigned int bitWidth,
        std::string& result);

    /// Unique instance.
    static OperationSimulator* instance_;
};

#endif
