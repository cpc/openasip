/*
    Copyright (c) 2002-2021 Tampere University.

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
 * @file ShrinkRegisterCounts.cc
 *
 * Explorer plugin that iteratively reduces the number of registers in the
 * Register File until the performance drops below the threshold.
 *
 * @author Alex Hirvonen
 * @note rating: red
 */

#include <vector>

#include "DSDBManager.hh"
#include "DesignSpaceExplorerPlugin.hh"
#include "ExplorationTools.hh"
#include "Machine.hh"

using namespace TTAMachine;

class ShrinkRegisterCounts : public DesignSpaceExplorerPlugin {
    PLUGIN_DESCRIPTION(
        "Iteratively reduces the number of registers in the "
        "given Register File until the cycle count drops below the "
        "threshold.");

    ShrinkRegisterCounts()
        : DesignSpaceExplorerPlugin(), ccThreshold_(0), rfToShrink_("") {
        addParameter(
            ccThresholdPN_, INT, false, "0",
            "Clockcycle count threshold to stop shrinking Register File "
            "size.");
        addParameter(
            rfToShrinkPN_, STRING, false, "",
            "The name of the Register File which size will be shrinked.");
    }

    virtual bool
    requiresStartingPointArchitecture() const {
        return true;
    }
    virtual bool
    producesArchitecture() const {
        return true;
    }
    virtual bool
    requiresHDB() const {
        return false;
    }
    virtual bool
    requiresSimulationData() const {
        return false;
    }
    virtual bool
    requiresApplication() const {
        return true;
    }

    virtual std::vector<RowID>
    explore(const RowID& configurationID, const unsigned int&) {
        readParameters();
        std::vector<RowID> result;
        DSDBManager::MachineConfiguration conf;
        conf.hasImplementation = false;
        Machine* mach = NULL;

        // load the adf from file or from dsdb
        try {
            conf = db().configuration(configurationID);
            mach = db().architecture(conf.architectureID);
        } catch (const Exception& e) {
            std::ostringstream msg(std::ostringstream::out);
            Application::errorStream()
                << "Error loading the adf." << std::endl;
            return result;
        }
        assert(mach != NULL);

        std::vector<TCEString> sortedRFids{};

        // specific RF name to resize is given
        if (rfToShrink_.length() > 0) {
            RegisterFile* rf =
                mach->registerFileNavigator().item(rfToShrink_);
            sortedRFids.push_back(rf->name());
            // Otherwise start resizing RFs by decreasing width order
        } else {
            sortedRFids = ExplorationTools::sortRFsByWidth(mach);
        }

        auto rfiter = sortedRFids.begin();

        while (true) {
            // Nothing to resize
            if (sortedRFids.empty()) {
                break;
            }

            // Reached end of RF names list, start over
            if (rfiter == sortedRFids.end()) {
                rfiter = sortedRFids.begin();
            }

            RegisterFile* rf = mach->registerFileNavigator().item(*rfiter);

            // Resizing for this RF is finished
            if (rf->numberOfRegisters() <= 16) {
                sortedRFids.erase(rfiter);
                continue;
            }
            if (Application::verboseLevel() > 0) {
                Application::errorStream()
                    << "Shrinking " << rf->name() << ", [" << rf->width()
                    << "bit] size ..."
                    << "\n\t" << rf->numberOfRegisters() << " -> "
                    << rf->numberOfRegisters() / 2 << std::endl;
            }

            rf->setNumberOfRegisters(rf->numberOfRegisters() / 2);

            RowID confBackupID = db().configurationId(conf);
            // add machine to configuration, save new conf to db, evaluate
            conf.architectureID = db().addArchitecture(*mach);
            RowID confId = db().addConfiguration(conf);
            if (Application::verboseLevel() > 0) {
                Application::errorStream() << "\tEvaluating...";
            }
            evaluate(conf);

            std::vector<ClockCycleCount> cycles = db().cycleCounts(conf);
            ClockCycleCount maxCC =
                *max_element(cycles.begin(), cycles.end());
            if (Application::verboseLevel() > 0) {
                Application::errorStream()
                    << " cycle count: " << maxCC << std::endl;
            }

            // Cycle count threshold was given
            if (ccThreshold_ != 0) {
                if (maxCC > ccThreshold_) {
                    // revert machine to previous state before RF resizing
                    if (Application::verboseLevel() > 0) {
                        Application::errorStream()
                            << "\tThreshold " << ccThreshold_ << " reached."
                            << std::endl;
                    }
                    conf = db().configuration(confBackupID);
                    mach = db().architecture(confBackupID);
                    sortedRFids.erase(rfiter);
                    continue;
                }
            }
            rfiter++;
            result.push_back(confId);
        }
        return result;
    }

protected:
    // Clockcycle count threshold
    ClockCycleCount ccThreshold_;
    static const TCEString ccThresholdPN_;

    // The name of the Register File which size will be shrinked.
    static const TCEString rfToShrinkPN_;
    TCEString rfToShrink_;

    /**
     * Reads the parameters given to the plugin.
     */
    void
    readParameters() {
        readOptionalParameter(ccThresholdPN_, ccThreshold_);
        readOptionalParameter(rfToShrinkPN_, rfToShrink_);
    }
};

const TCEString ShrinkRegisterCounts::ccThresholdPN_("cc_threshold");
const TCEString ShrinkRegisterCounts::rfToShrinkPN_("rf_to_shrink");

EXPORT_DESIGN_SPACE_EXPLORER_PLUGIN(ShrinkRegisterCounts)
