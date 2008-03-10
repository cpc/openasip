/**
 * @file TestApplication.hh
 *
 * Declaration of TestApplication class.
 *
 * @author Jari Mäntyneva 2007 (jari.mantyneva@tut.fi)
 * @note rating: red
 */

#ifndef TTA_TEST_APPLICATION_HH
#define TTA_TEST_APPLICATION_HH

#include <string>
#include <vector>
#include <sstream>
#include "Exception.hh"


/**
 * Class for handling files in test application directory. 
 */
class TestApplication {
public:
    typedef double Runtime;
    typedef int ClockCycles;


    TestApplication(const std::string& testApplicationPath)
        throw (IOException);
    virtual ~TestApplication();

    std::vector<std::string> description() const;

    bool hasApplication() const;
    bool hasSetupSimulation() const;
    bool hasSimulateTTASim() const;
    bool hasCorrectOutput() const;
    bool hasVerifySimulation() const;
    bool hasCleanupSimulation() const;

    const std::string applicationPath() const;
    void setupSimulation() const;
    std::istream* simulateTTASim() const;
    const std::string correctOutput() const;
    bool verifySimulation() const;
    void cleanupSimulation() const;
    ClockCycles cycleCount() const;
    Runtime maxRuntime() const;

private:
    /// Path of the test application directory.
    const std::string testApplicationPath_;
    /// Maximum runtime of the test appication in nano seconds
    Runtime maxRuntime_;

    /// Name of the file that contains a description.
    static const std::string DESCRIPTION_FILE_NAME_;
    /// Name of the sequential program file.
    static const std::string APPLICATION_FILE_NAME_;
    /// Name of the file that contains setup script.
    static const std::string SETUP_FILE_NAME_;
    /// Name of the file that runs the simulation.
    static const std::string SIMULATE_TTASIM_FILE_NAME_;
    /// Name of the correct simulation output file.
    static const std::string CORRECT_OUTPUT_FILE_NAME_;
    /// Name of the verify script file.
    static const std::string VERIFY_FILE_NAME_;
    /// Name of the clean up file.
    static const std::string CLEANUP_FILE_NAME_;
    /// Name of the file that contains maximum runtime.
    static const std::string MAX_RUNTIME_;
    /// Maximum line length in a file.
    static const int MAX_LINE_LENGTH_;
};




#endif
