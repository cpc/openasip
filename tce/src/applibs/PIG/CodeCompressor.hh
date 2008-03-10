/**
 * @file CodeCompressor.hh
 *
 * Macro definitions for defining code compressor plugins for program image
 * generator.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_CODE_COMPRESSOR_HH
#define TTA_CODE_COMPRESSOR_HH

#include "CodeCompressorPlugin.hh"

/**
 * Exports the given class as a code compressor plugin.
 */
#define EXPORT_CODE_COMPRESSOR(PLUGIN_NAME__) \
extern "C" { \
    CodeCompressorPlugin* create_code_compressor() {\
        return new PLUGIN_NAME__(); \
    }\
    void delete_code_compressor(\
        CodeCompressorPlugin* target) {\
        delete target;\
    }\
}


#endif
