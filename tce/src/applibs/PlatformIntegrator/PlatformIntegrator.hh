/*
    Copyright (c) 2002-2010 Tampere University of Technology.

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
 * @file PlatformIntegerator.hh
 *
 * Declaration of PlatformIntegrator class.
 *
 * @author Otto Esko 2010 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_PLATFORM_INTEGRATOR_HH
#define TTA_PLATFORM_INTEGRATOR_HH

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "MemoryGenerator.hh"

class PlatformIntegrator {
public:
    
    // structure for memory information
    struct MemInfo {
        MemType type;
        int mauWidth;
        int widthInMaus;
        int addrw;
        std::string asName;
    };

    PlatformIntegrator();

    PlatformIntegrator(
        std::string progeOutputDir,
        std::string entityName,
        std::string outputDir,
        std::string programName,
        int targetClockFreq,
        std::ostream& warningStream,
        std::ostream& errorStream);

    virtual ~PlatformIntegrator();

    virtual void integrateProcessor(MemInfo& imem, MemInfo& dmem) = 0;

    /**
     * Returns the FPGA device family
     */
    virtual std::string deviceFamily() const = 0;


    /**
     * Returns the FPGA device name
     */
    virtual std::string deviceName() const = 0;


    /**
     * Returns the device package name
     */
    virtual std::string devicePackage() const = 0;

    /**
     * Returns the device speed class
     */
    virtual std::string deviceSpeedClass() const = 0;


    /**
     * Returns the target clock frequency in MHz
     */
    virtual int targetClockFrequency() const;

    /**
     * Prints information about the platform
     */
    virtual void printInfo(std::ostream& stream) const = 0;


protected:

    virtual void createNewToplevelTemplate();

    virtual std::string writeNewToplevel();
    
    /**
     * Return new toplevel entity name
     */
    std::string entityName() const;

    /**
     * Returns the scheduled program name without .tpef ending
     */
    std::string programName() const;

    /**
     * Returns string to the proge-output path of the fileName
     *
     * @param filename Name of file
     * @param absolute Return absolute path
     * @return Path to file
     */
    std::string
    progeFilePath(std::string fileName, bool absolute = false) const;
    
    /**
     * Returns string to the platform integrator output path of the fileName
     *
     * @param filename Name of file
     * @param absolute Return absolute path
     * @return Path to file
     */
    std::string outputFilePath(std::string fileName, bool absolute = false);

    /**
     * Returns the platform integrator output path string
     *
     * @return Platform Integrator output path
     */
    std::string outputPath();

    /**
     * Utility function for processing vhdl signals.
     *
     * Signal name format is fu_name_<TAG>_name : <dir> <type> {);}
     * This function return "fu_name_<TAG>_name" part of the name
     *
     * @param original Original signal name
     * @return Signal name which has been stripped from direction and type
     */
    std::string chopToSignalName(const std::string& original) const;

    /**
     * Utility function for processing vhdl signals.
     *
     * Signal name format is fu_name_<TAG>_name : <dir> <type> {);}
     * This function chops the signal name in such way that the name starts
     * from <TAG> i.e. returns <TAG>_name : <dir> <type> {);}
     *
     * @param original Original signal name
     * @param tag Starting tag
     * @return Chopped string
     */
    std::string chopSignalToTag(
        const std::string& original, const std::string& tag) const;

    /**
     * Utility function for processing vhdl signals.
     *
     * Signal name format is fu_name_<TAG>_name : <dir> <type> {);}
     * This function strips the semicolon and possible duplicate ')' braces
     * from the signal i.e. returns fu_name_<TAG>_name : <dir> <type>
     * 
     * @param original Original signal name
     * @return Stripped signal name 
     */
    std::string stripSignalEnd(const std::string& original) const;

    /**
     * Appends all the vhdl files from ProGe output directory's vhdl and
     * gcu_ic subdirectories to the given vector.
     *
     * @param files Vector where the filenames are appended to.
     */
    void progeOutputHdlFiles(std::vector<std::string>& files) const;

    /**
     * Reads TTA toplevel from file and writes it to componentStream
     */
    void readTTAToplevel();

    /**
     * Return TTA toplevel as string vector.
     * readTTAToplevel() must be called before this
     */
    const std::vector<std::string>& ttaToplevel() const;

    std::ostream& warningStream();

    std::ostream& errorStream();

    std::ostream& newToplevelStream();
    std::ostream& newEntityStream();
    std::ostream& componentStream();
    std::ostream& signalStream();
    std::ostream& ttaToplevelInstStream();
    std::ostream& imemInstStream();
    std::ostream& dmemInstStream();
    std::ostream& signalConnectionStream();
    

private:

    void createOutputDir();

    

    std::string progeOutputDir_;
    std::string entityName_;
    std::string outputDir_;
    bool outputDirCreated_;
    std::string programName_;
    int targetFrequency_;
    
    std::ostream& warningStream_;
    std::ostream& errorStream_;

    std::vector<std::string> ttaToplevel_;
    
    std::ostringstream newToplevelStream_;
    std::ostringstream newEntityStream_;
    std::ostringstream componentStream_;
    std::ostringstream signalStream_;
    std::ostringstream toplevelInstantiationStream_;
    std::ostringstream imemInstantiationStream_;
    std::ostringstream dmemInstantiationStream_;
    std::ostringstream signalConnectionStream_;
};
#endif
