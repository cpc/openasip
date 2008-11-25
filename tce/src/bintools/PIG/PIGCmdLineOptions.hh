/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
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
    std::string tpefFile(int index) const
        throw (OutOfRange);
    std::string programImageOutputFormat() const;
    std::string dataImageOutputFormat() const;
    std::string compressorPlugin() const;
    int dataMemoryWidthInMAUs() const;
    bool generateDataImages() const;
    bool generateDecompressor() const;
    int compressorParameterCount() const;
    std::string compressorParameter(int index) const
        throw (OutOfRange);
    bool showCompressors() const;
    std::string progeOutputDirectory() const;

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
};

#endif
