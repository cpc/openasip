/*
 * Copyright 1993-2009 NVIDIA Corporation.  All rights reserved.
 *
 * NOTICE TO USER:
 *
 * This source code is subject to NVIDIA ownership rights under U.S. and
 * international Copyright laws.  Users and possessors of this source code
 * are hereby granted a nonexclusive, royalty-free license to use this code
 * in individual and commercial software.
 *
 * NVIDIA MAKES NO REPRESENTATION ABOUT THE SUITABILITY OF THIS SOURCE
 * CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR
 * IMPLIED WARRANTY OF ANY KIND.  NVIDIA DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOURCE CODE, INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE.
 * IN NO EVENT SHALL NVIDIA BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS,  WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION,  ARISING OUT OF OR IN CONNECTION WITH THE USE
 * OR PERFORMANCE OF THIS SOURCE CODE.
 *
 * U.S. Government End Users.   This source code is a "commercial item" as
 * that term is defined at  48 C.F.R. 2.101 (OCT 1995), consisting  of
 * "commercial computer  software"  and "commercial computer software
 * documentation" as such terms are  used in 48 C.F.R. 12.212 (SEPT 1995)
 * and is provided to the U.S. Government only as a commercial end item.
 * Consistent with 48 C.F.R.12.212 and 48 C.F.R. 227.7202-1 through
 * 227.7202-4 (JUNE 1995), all U.S. Government End Users acquire the
 * source code with only those rights set forth herein.
 *
 * Any use of this source code in individual and commercial software must
 * include, in the user documentation and internal comments to the code,
 * the above Disclaimer and U.S. Government End Users Notice.
 */
 
// copied here to allow umodified compilation of the NVIDIA
// examples
#ifndef OCL_UTILS_H
#define OCL_UTILS_H

//#ifndef __TCE__
//#error This is a TCE compatibility header for NVIDIA OpenCL.
//#endif

// OS-dependent common headers and Cross API utililties
// #include <shrUtils.h>

// OpenCL header
#include <CL/cl.h>

#include <stdlib.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

// SDK Version
#define oclSDKVERSION "1.00.00.04"  // 20090517

#ifdef __cplusplus
#define EXTERN extern "C"
#else
#define EXTERN 
#endif

// Temorary #define until profiling issues with WinVista32 and WinVista64 are fixed
// *********************************************************************
// (When fix is complete, delete this and all code line references to it)
// Uncomment #define to enable OpenCL profiling  
// #define GPU_PROFILING

// Helper function for De-allocating cl objects
// *********************************************************************
EXTERN void oclDeleteMemObjs(cl_mem* cmMemObjs, int iNumObjs);

//////////////////////////////////////////////////////////////////////////////
//! Print info about the device
//!
//! @param iLogMode       enum LOGBOTH, LOGCONSOLE, LOGFILE
//! @param device         OpenCL id of the device
//////////////////////////////////////////////////////////////////////////////
EXTERN void oclPrintDevInfo(int iLogMode, cl_device_id device);

//////////////////////////////////////////////////////////////////////////////
//! Print the device name
//!
//! @param iLogMode       enum LOGBOTH, LOGCONSOLE, LOGFILE
//! @param device         OpenCL id of the device
//////////////////////////////////////////////////////////////////////////////
EXTERN void oclPrintDevName(int iLogMode, cl_device_id device);

//////////////////////////////////////////////////////////////////////////////
//! Gets the id of the first device from the context
//!
//! @return the id 
//! @param cxMainContext         OpenCL context
//////////////////////////////////////////////////////////////////////////////
EXTERN cl_device_id oclGetFirstDev(cl_context cxMainContext);

//////////////////////////////////////////////////////////////////////////////
//! Gets the id of the nth device from the context
//!
//! @return the id or -1 when out of range
//! @param cxMainContext         OpenCL context
//! @param device_idx            index of the device of interest
//////////////////////////////////////////////////////////////////////////////
EXTERN cl_device_id oclGetDev(cl_context cxMainContext, unsigned int device_idx);

//////////////////////////////////////////////////////////////////////////////
//! Gets the id of device with maximal FLOPS from the context
//!
//! @return the id 
//! @param cxMainContext         OpenCL context
//////////////////////////////////////////////////////////////////////////////
EXTERN cl_device_id oclGetMaxFlopsDev(cl_context cxMainContext);

//////////////////////////////////////////////////////////////////////////////
//! Loads a Program file and prepends the cPreamble to the code.
//!
//! @return the source string if succeeded, 0 otherwise
//! @param cFilename        program filename
//! @param cPreamble        code that is prepended to the loaded file, typically a set of #defines or a header
//! @param szFinalLength    returned length of the code string
//////////////////////////////////////////////////////////////////////////////
EXTERN char* oclLoadProgSource(const char* cFilename, const char* cPreamble, size_t* szFinalLength);

//////////////////////////////////////////////////////////////////////////////
//! Get the binary (PTX) of the program associated with the device
//!
//! @param cpProgram    OpenCL program
//! @param cdDevice     device of interest
//! @param binary       returned code
//! @param length       length of returned code
//////////////////////////////////////////////////////////////////////////////
EXTERN void oclGetProgBinary( cl_program cpProgram, cl_device_id cdDevice, char** binary, size_t* length);

// no log files in TTAOpenCL yet ;)
#define shrSetLogFileName(__X)
#define LOGBOTH 0

#define shrLog(__IGNORE, __IGNORE2, __MSG, ...) printf(__MSG, ## __VA_ARGS__)

//inline 


EXTERN inline void __shrCheckErrorEX(int iSample, int iReference, void (*pCleanup)(int), const char* cFile, const int iLine)
{
    if (iReference != iSample)
    {
        shrLog(LOGBOTH | ERRORMSG, (double)iSample, "line %i , in file %s !!!\n\n" , iLine, cFile);
        if (pCleanup != NULL)
        {
            pCleanup(EXIT_FAILURE);
        }
        else
        {
            shrLog(LOGBOTH | CLOSELOG, 0.0, "Exiting...\n");
            exit(EXIT_FAILURE);
        }
    }
}
//#define shrCheckErrorEX(a, b, c) __shrCheckErrorEX(a, b, c, __FILE__ , __LINE__)
//#define shrCheckError(a, b) __shrCheckErrorEX(a, b, 0, __FILE__, __LINE__)

#define shrCheckError(a, b) 

#define shrFindFilePath(__IGNORE, __IGNORE_TOO) (char*)"__dummy_ignore_me"

#define shrTRUE 1
#define shrFALSE 0

#define shrEXIT(__IGN, __IGN2) return EXIT_SUCCESS

#endif
