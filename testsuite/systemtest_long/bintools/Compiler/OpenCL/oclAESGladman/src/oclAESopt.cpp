#define shrBOOL bool
#define shrCheckErrorEX(x, y, z) shrCheckError(x, y)

// *********************************************************************
// Demo application function to compute AES Encryption or Decryption over
// a text file
// *********************************************************************

// standard utilities and systems includes
#include <oclUtils.h>

#include <stdlib.h>
#include <stdio.h>

#include "aes_key.h"
#include "aes_crypt.h"
#include "tables.h"

// Name of the file with the source code for the computation kernel
// *********************************************************************
const char* clSourcefile = "AES.cl";


// Configurable
#define NUM_GROUPS 4
#define FUNCTION "Encrypt"
#define FILE_SIZE 4 // KB
// Not configurable
#define NUM_THREADS FILE_SIZE*1024/16
//#define LOCAL_WORK_SIZE NUM_THREADS/NUM_GROUPS
#define LOCAL_WORK_SIZE 2


unsigned char encryption_key_c[] = "1234567890abcdef";
unsigned char expandedkey[200];
unsigned int expandedkeyInt[200];

//input and output buffers
unsigned char *h_idata_c, *h_odata_c;
unsigned int  *h_idata_int, *h_odata_int;
unsigned int  *golden_results;

// OpenCL Vars
cl_context cxGPUContext;        // OpenCL context
cl_command_queue cqCommandQue;  // OpenCL command que
cl_device_id* cdDevices;        // OpenCL device list    
cl_program cpProgram;           // OpenCL program
cl_kernel ckKernel;             // OpenCL kernel
cl_mem cmMemObjs[7];            // OpenCL memory buffer objects
size_t szGlobalWorkSize[1];     // 1D var for Total # of work items
size_t szLocalWorkSize[1];		// 1D var for # of work items in the work group	
size_t szParmDataBytes;			// Byte size of context information
size_t szKernelLength;			// Byte size of kernel code
cl_int ciErr1, ciErr2;			// Error code var
char* cPathAndName = NULL;      // var for full paths to data, src, etc.
char* cSourceCL = NULL;         // Buffer to hold source for compilation 

shrBOOL bQuickTest = shrFALSE;  

// Forward Declarations
// *********************************************************************
void Cleanup (int iExitCode);
void cblock2iblock(unsigned char *cblock, unsigned int *iblock);


// Main function 
// *********************************************************************
int main(int argc, char **argv)
{
            
    unsigned int mem_size_c = FILE_SIZE*1024*sizeof(char);
    unsigned int mem_size_int = FILE_SIZE*(1024/4)*sizeof(int);	
    
    // allocate host memory
    h_idata_c = (unsigned char*)malloc(mem_size_c);
    h_odata_c = (unsigned char*)malloc(mem_size_c);
    h_idata_int = (unsigned int*)malloc(mem_size_int);
    h_odata_int = (unsigned int*)malloc(mem_size_int);
    golden_results = (unsigned int*)malloc(mem_size_int);    
    
    //Initialice 1 K of data (chars)
    for(int i = 0 ; i < FILE_SIZE*1024; i++)
    	h_idata_c[i] = (unsigned char) (i % 255);
    //Pack chars into words          
    for(int i = 0; i < FILE_SIZE*1024/16; i++)
    	cblock2iblock(&h_idata_c[i*16],&h_idata_int[i*4]);
    //expand key      
     KeyExpand2(encryption_key_c, expandedkey);
     Key2Int(expandedkey, expandedkeyInt);
     
     //Calculate golden results
     for(int i = 0; i < FILE_SIZE*1024/4; i++)
     	golden_results[i] = h_idata_int[i];
     for(int i = 0; i < FILE_SIZE*1024/16; i++)	
	encrypt_aes2Int(&golden_results[i*4],expandedkeyInt);
         
    // Create the OpenCL context on a GPU device
    cxGPUContext = clCreateContextFromType(0, CL_DEVICE_TYPE_GPU, NULL, NULL, &ciErr1);
    
    // Get the list of GPU devices associated with context
    ciErr1 |= clGetContextInfo(cxGPUContext, CL_CONTEXT_DEVICES, 0, NULL, &szParmDataBytes);
    cdDevices = (cl_device_id*)malloc(szParmDataBytes);
    ciErr1 |= clGetContextInfo(cxGPUContext, CL_CONTEXT_DEVICES, szParmDataBytes, cdDevices, NULL);
    
    // Create a command-queue
    cqCommandQue = clCreateCommandQueue(cxGPUContext, cdDevices[0], 0, &ciErr2);
    ciErr1 |= ciErr2; 
     if(ciErr1 != CL_SUCCESS)
    {
    	puts("Error while creating command queue.\n");
    	Cleanup(1);
    }

    // set Global and Local work size dimensions
    szGlobalWorkSize[0] = NUM_THREADS;
    szLocalWorkSize[0]= LOCAL_WORK_SIZE;

    // create input and output data buffers in GPU
    cmMemObjs[0] = clCreateBuffer(cxGPUContext, CL_MEM_COPY_HOST_PTR  | CL_MEM_READ_ONLY, mem_size_int, (void *)h_idata_int, &ciErr2);
    ciErr1 |= ciErr2;    
    cmMemObjs[1] = clCreateBuffer(cxGPUContext, CL_MEM_WRITE_ONLY, mem_size_int, NULL, &ciErr2);
    ciErr1 |= ciErr2;
    
    //create buffer in GPU for the encryption key
    cmMemObjs[2] = clCreateBuffer(cxGPUContext, CL_MEM_COPY_HOST_PTR  | CL_MEM_READ_ONLY, 200*sizeof(unsigned int), (void *)expandedkeyInt, &ciErr2);    
    ciErr1 |= ciErr2;
    
    // create constant arrays for AES Sboxes in GPU
    
    cmMemObjs[3] = clCreateBuffer(cxGPUContext, CL_MEM_COPY_HOST_PTR  | CL_MEM_READ_ONLY, 256*sizeof(unsigned int), (void *)_Alogtable, &ciErr2);    
    ciErr1 |= ciErr2;
    cmMemObjs[4] = clCreateBuffer(cxGPUContext, CL_MEM_COPY_HOST_PTR  | CL_MEM_READ_ONLY, 256*sizeof(unsigned int), (void *)_Logtable, &ciErr2);
    ciErr1 |= ciErr2;
    cmMemObjs[5] = clCreateBuffer(cxGPUContext, CL_MEM_COPY_HOST_PTR  | CL_MEM_READ_ONLY, 256*sizeof(unsigned int), (void *)_Sen, &ciErr2);
    ciErr1 |= ciErr2;
    cmMemObjs[6] = clCreateBuffer(cxGPUContext, CL_MEM_COPY_HOST_PTR  | CL_MEM_READ_ONLY, 256*sizeof(unsigned int), (void *)_Sde, &ciErr2);
    ciErr1 |= ciErr2;
    
    
    if(ciErr1 != CL_SUCCESS)
    {
    	puts("Error while creating data buffers in the device\n");
    	Cleanup(1);
    }
    
    // Read the kernel in from file
    cPathAndName = shrFindFilePath(clSourcefile, argv[0]);
//    puts("Path and name = %s\n",cPathAndName);
    shrCheckErrorEX(cPathAndName != NULL, shrTRUE, &Cleanup);
    cSourceCL = oclLoadProgSource(cPathAndName, "// My comment\n", &szKernelLength);
    shrCheckErrorEX(cSourceCL != NULL, shrTRUE, &Cleanup);
    

    // Create and build the program
    cpProgram = clCreateProgramWithSource(cxGPUContext, 1, (const char **)&cSourceCL, &szKernelLength, &ciErr1);
    ciErr1 |= clBuildProgram(cpProgram, 0, NULL, NULL, NULL, NULL);
#if 0
    if (ciErr1 != CL_SUCCESS)
    {
        // write out standard error
        // shrLog(LOGBOTH | ERRORMSG, (double)ciErr1, STDERROR);

        // write out the build log and ptx, then exit
        char cBuildLog[10240];
        char* cPtx;
        size_t szPtxLength;
        clGetProgramBuildInfo(cpProgram, oclGetFirstDev(cxGPUContext), CL_PROGRAM_BUILD_LOG, 
                              sizeof(cBuildLog), cBuildLog, NULL );
        oclGetProgBinary(cpProgram, oclGetFirstDev(cxGPUContext), &cPtx, &szPtxLength);
        shrLog(LOGBOTH, 0.0, "\n\nLog:\n%s\n\n\n\n\nPtx:\n%s\n\n\n", cBuildLog, cPtx);
        Cleanup(EXIT_FAILURE); 
    }
    if (ciErr1 != CL_SUCCESS)
    {
        puts("Error while building the program.\n");
        Cleanup(1);
    }
#endif

    // Create the kernel
    ckKernel = clCreateKernel(cpProgram, FUNCTION, &ciErr1);
    if (ciErr1 != CL_SUCCESS)
    {
        puts("Error while creating the kernel.\n");
        Cleanup(1);
    }

    // Set the Argument values:
    // data_in, data_out, key, Alogtable, Logtable, Sen, Sde, memsize
    ciErr1 |= clSetKernelArg(ckKernel, 0, sizeof(cl_mem), (void*)&cmMemObjs[0]);
    ciErr1 |= clSetKernelArg(ckKernel, 1, sizeof(cl_mem), (void*)&cmMemObjs[1]);
    ciErr1 |= clSetKernelArg(ckKernel, 2, sizeof(cl_mem), (void*)&cmMemObjs[2]);
    
    ciErr1 |= clSetKernelArg(ckKernel, 3, sizeof(cl_mem), (void*)&cmMemObjs[3]);
    ciErr1 |= clSetKernelArg(ckKernel, 4, sizeof(cl_mem), (void*)&cmMemObjs[4]);
    ciErr1 |= clSetKernelArg(ckKernel, 5, sizeof(cl_mem), (void*)&cmMemObjs[5]);
    ciErr1 |= clSetKernelArg(ckKernel, 6, sizeof(cl_mem), (void*)&cmMemObjs[6]);
    
    //ciErr1 |= clSetKernelArg(ckKernel, 7, sizeof(unsigned int), (void*)&mem_size_c);
#ifdef VERIFICATION    
    // Launch kernel
    iprintf(
        "Launching the kernel.\nNUM_THREADS = %d\nNUM_GROUPS = %d\nLOCAL_WORK_SIZE = %d\nFILE SIZE = %d KB\n", 
        NUM_THREADS, NUM_GROUPS, LOCAL_WORK_SIZE, FILE_SIZE); 
#endif
    ciErr1 |= clEnqueueNDRangeKernel(cqCommandQue, ckKernel, 1, NULL, szGlobalWorkSize, szLocalWorkSize, 0, NULL, NULL);
    if (ciErr1 != CL_SUCCESS)
    {
        puts("Error while launching the kernel.\n");
        Cleanup(1);
    }

    // Read back results and check accumulated errors
    ciErr1 |= clEnqueueReadBuffer(cqCommandQue, cmMemObjs[1], CL_TRUE, 0, mem_size_int, (void *)h_odata_int, 0, NULL, NULL);
    if (ciErr1 != CL_SUCCESS)
    {
        puts("Error while reading back results.\n");
        Cleanup(1);
    }
#ifdef VERIFICATION    
    puts("Results read back.\n");

    //Compare reults
    int error = 0;
    for(int i = 0; i < FILE_SIZE*1024/4; i++)
    {
    	if(golden_results[i] != h_odata_int[i])
    		error += 1;
    }
    if(error != 0)
    	puts("Results do not match.\n");
    else
    	puts("Results OK!!!\n");

    //print results
    /*for(int i = 0; i < 1024/4; i++)
    	{
    		
    		printf("0x%X\n", h_odata_int[i]);
    	}
    */
    
    // Cleanup and leave
    Cleanup (EXIT_SUCCESS);
#endif    
}
        
void Cleanup (int iExitCode)
{
    // Cleanup allocated objects
    shrLog(LOGBOTH, 0.0, "\nStarting Cleanup...\n\n");
    if(cdDevices)free(cdDevices);
    //    if(cPathAndName)free(cPathAndName);
    if(cSourceCL)free(cSourceCL);
	if(ckKernel)clReleaseKernel(ckKernel);  
    if(cpProgram)clReleaseProgram(cpProgram);
    if(cqCommandQue)clReleaseCommandQueue(cqCommandQue);
    if(cxGPUContext)clReleaseContext(cxGPUContext);
    clReleaseMemObject(cmMemObjs[0]);
    clReleaseMemObject(cmMemObjs[1]);
    clReleaseMemObject(cmMemObjs[2]);
    clReleaseMemObject(cmMemObjs[3]);
    clReleaseMemObject(cmMemObjs[4]);
    clReleaseMemObject(cmMemObjs[5]);
    clReleaseMemObject(cmMemObjs[6]);

    // Free host memory
    free(h_idata_c);    
    free(h_odata_c);
    free(h_idata_int);    
    free(h_odata_int);
    free(golden_results);
    
    exit (iExitCode);
}

void cblock2iblock(unsigned char *cblock, unsigned int *iblock)
{
	byte0_2_word(cblock[0], iblock[0]);
	byte1_2_word(cblock[1], iblock[0]);
	byte2_2_word(cblock[2], iblock[0]);
	byte3_2_word(cblock[3], iblock[0]);
	
	byte0_2_word(cblock[4], iblock[1]);
	byte1_2_word(cblock[5], iblock[1]);
	byte2_2_word(cblock[6], iblock[1]);
	byte3_2_word(cblock[7], iblock[1]);
	
	byte0_2_word(cblock[8], iblock[2]);
	byte1_2_word(cblock[9], iblock[2]);
	byte2_2_word(cblock[10], iblock[2]);
	byte3_2_word(cblock[11], iblock[2]);
	
	byte0_2_word(cblock[12], iblock[3]);
	byte1_2_word(cblock[13], iblock[3]);
	byte2_2_word(cblock[14], iblock[3]);
	byte3_2_word(cblock[15], iblock[3]);
}
