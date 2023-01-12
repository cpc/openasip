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
 * @file PIGCmdLineOptions.hh
 *
 * Declaration of PIGCmdLineOptions class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_PIG_CMD_LINE_OPTIONS_HH
#define TTA_PIG_CMD_LINE_OPTIONS_HH

#include "CmdLineOptions.hh"

/**
 * Command line options for the command line interface of PIG (generatebits).
 */
class PIGCmdLineOptions : public CmdLineOptions {
public:
    PIGCmdLineOptions();
    virtual ~PIGCmdLineOptions();

    std::string bemFile() const;    
    int tpefFileCount() const;
    std::string tpefFile(int index) const;
    std::string programImageOutputFormat() const;
    std::string dataImageOutputFormat() const;
    std::string compressorPlugin() const;
    int dataMemoryWidthInMAUs() const;
    bool generateDataImages() const;
    bool generateDecompressor() const;
    int compressorParameterCount() const;
    std::string compressorParameter(int index) const;
    bool showCompressors() const;
    std::string progeOutputDirectory() const;
    std::string entityName() const;
    CmdLineOptionParser* dataStart() const;
    virtual void printVersion() const;
    virtual void printHelp() const;
    void printUsage() const;
    
private:
    /// Long name of the BEM file parameter.
    static const std::string BEM_PARAM_NAME;
    /// Long name of the TPEF file parameter.
    static const std::string TPEF_PARAM_NAME;
    /// Long name of the program image output format parameter.
    static const std::string PI_FORMAT_PARAM_NAME;
    /// Long name of the data image output format parameter.
    static const std::string DI_FORMAT_PARAM_NAME;
    /// Long name of the plugin file parameter.
    static const std::string COMPRESSOR_PARAM_NAME;
    /// Long name of the parameter that defines whether to create data 
    /// images.
    static const std::string DATA_IMG_PARAM_NAME;
    /// Long name of the parameter that tells whether to generate 
    /// decompressor or not.
    static const std::string GEN_DECOMP_PARAM_NAME;
    /// Long name of the parameter that tells the width of data mem in MAUs.
    static const std::string DMEM_WIDTH_IN_MAUS_PARAM_NAME;
    /// Long name of the paramter that tells the width of inst mem in MAUs.
    static const std::string IMEM_WIDTH_IN_MAUS_PARAM_NAME;
    /// Long name of parameter passed to code compressor plugin.
    static const std::string COMPRESSOR_PARAMS_PARAM_NAME;
    /// Long name of the parameter that tells whether to show compressors.
    static const std::string SHOW_COMPRESSORS_PARAM_NAME;
    /// Long name of the parameter which tells the proge-output dir
    static const std::string HDL_OUTPUT_DIR;

    static const std::string DATA_START;
};

#endif
