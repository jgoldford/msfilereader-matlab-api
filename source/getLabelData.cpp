#include <comutil.h>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include "mex.h"
#include <matrix.h> 
#include <string.h>
#import "C:\Program Files\Thermo\MSFileReader\Xrawfile2_x64.dll"

//#define NUMBER_OF_STRUCTS (sizeof(friends)/sizeof(struct scan))
#define NUMBER_OF_FIELDS (sizeof(field_names)/sizeof(*field_names))

using namespace std;

struct myScan
{
  int reads;
  double *mass;
  float *noise;
  double *intensity;
};


void mexFunction(int nlhs,mxArray *plhs[],int nrhs,const mxArray *prhs[])
{
    //make sure there is exactly one 
    if (nrhs != 2) {
        mexErrMsgIdAndTxt("MATLAB:mexevalstring:nInput", "Two input arguments required.");
    } 
//**********************************************************************************//
//Set up MEX input and output
    //Declarations for File name Input
    mxArray *yData;
    int yLength;
    char *fileName;
    
    //copy input pointer y
    yData = (mxArray*)prhs[0];
    
    //make "fileName" point to char array
    yLength = (int)(mxGetN(yData)+1);
    fileName = (char *)(mxCalloc(yLength,sizeof(char)));
    mxGetString(yData,fileName,yLength);

    
    mxArray *zData;
    int scanNum;
    //copy input pointer y
    zData = (mxArray*)prhs[1];
    //make "scanNum" point to int scalar
    scanNum = (int)(mxGetScalar(zData));
    
    
    
//*********************************************************************************//    
// Code that opens RAW files    
    
    //Declare for reading raw files
    //HRESULT lRet;
    TCHAR pth[MAX_PATH];
    MSFileReaderLib::IXRawfile4Ptr m_Raw;
    
    // Initialize COM interface
    HRESULT hr = CoInitialize(NULL);    
    if (FAILED(hr)) {
        mexErrMsgTxt("Cannot initialize COM interface.");
    }
 
    //CoInitialize( NULL );
    hr = m_Raw.CreateInstance("MSFileReader.XRawfile.1");
    if (FAILED(hr)) {
        mexErrMsgTxt("Cannot access XRawfile.dll.");
    }
//***********************************************************************************//
    MultiByteToWideChar(CP_ACP,0,fileName,-1,(LPWSTR)pth,MAX_PATH);
    long lr = m_Raw->Open((LPWSTR)pth);
    // Look for data that belong to the first mass spectra device in the file
     m_Raw->SetCurrentController(0, 1);
    
    long firstScanNumber = 0, lastScanNumber = 0;

    // Verifies if can get the first scan
    hr = m_Raw->GetFirstSpectrumNumber(&firstScanNumber);
    if (FAILED(hr)) {
        mexErrMsgTxt("ERROR: Unable to get first scan.");
    }

    // Ask for the last scan number to prepare memory space, for cycle 
    // and final verification
    m_Raw->GetLastSpectrumNumber(&lastScanNumber);
    long totalNumScans = (lastScanNumber - firstScanNumber) + 1;
    
   if ( (scanNum < firstScanNumber || scanNum > lastScanNumber)){
        mexErrMsgTxt("ERROR: Not a valid scan number for this raw file.");
        
    }
    

    //mexPrintf("NUMBER OF SCANS: %d\n", totalNumScans);

myScan scan;


    //populate structure array
        long i = (long)scanNum; 
        long nRet;
        VARIANT varLabels;
        VARIANT varFlags;
        VariantInit(&varLabels);
        VariantInit(&varFlags);
        nRet = m_Raw->GetLabelData(&varLabels, &varFlags, &i);
        if(nRet !=0){
            mexErrMsgTxt("ERROR: Unable to get label data.");
        }
        int read_i;
        int sz = varLabels.parray->rgsabound[0].cElements;
        scan.reads = sz;
         
         
        //allocate memory for arrays within structure
        scan.mass = (double*)malloc((scan.reads)*sizeof(double));
        scan.intensity =(double*)malloc((scan.reads)*sizeof(double));
        scan.noise = (float*)malloc((scan.reads)*sizeof(float));
        //mexPrintf("%d",sz);
        
        double* pdval;
        pdval=(double*)varLabels.parray->pvData;
        
        
	// pdval[read_i*6] == m/z
    // pdval[read_i*6+1] == intensity 
    // pdval[read_i*6+4] == noise
    for(read_i=0; (read_i) < sz ;(read_i)++){
        scan.mass[(read_i)] = pdval[read_i*6];
        scan.intensity[(read_i)] = pdval[(read_i)*6+1];
        scan.noise[(read_i)]= (float)pdval[(read_i)*6+4];
        //mexPrintf("%f, %f, %f\n",pdval[(read_i)*6],pdval[(read_i)*6+1],pdval[(read_i)*6+4]);
    }   
        //mexPrintf("%f, %f, %f\n",pdval[(4)*6],pdval[(4)*6+1],pdval[(4)*6+4]);

    
          
    VariantClear(&varLabels);
    VariantClear(&varFlags);
  
/*// transfer scan structure to mexStructure array
    const char *field_names[] = {"reads","mass","intensity","noise"};
    mwSize dims[2] = {1, 1 };
    //int name_field, phone_field;
    mwIndex idx=(int)scanNum;
    
    if(nlhs > 1){
        mexErrMsgTxt("Too many output arguments.");
    }
    
    // Create a 1-by-n array of structs. 
    plhs[0] = mxCreateStructArray(2, dims, NUMBER_OF_FIELDS, field_names);
   
    
    //POPULATE THE STRUCTURE ARRAY
     
            mxArray *readsVal, *massVal, *intenVal, *noiseVal;
            mwSize n=scan.reads;
            
             
            //create data types
            readsVal = mxCreateDoubleMatrix(1,1,mxREAL);
            massVal = mxCreateDoubleMatrix(1,n,mxREAL);
            intenVal = mxCreateDoubleMatrix(1,n,mxREAL);
            noiseVal = mxCreateDoubleMatrix(1,n,mxREAL);
            
            //scans[idx].reads
            //associate pointers
            *mxGetPr(readsVal) = (double)scan.reads;
            *mxGetPr(massVal) =  scan.mass[1];
            *mxGetPr(intenVal) = scan.intensity[1];
            *mxGetPr(noiseVal) = scan.noise[1];
            
            //set fields
             mxSetFieldByNumber(plhs[0],1,0,readsVal);
             mxSetFieldByNumber(plhs[0],1,1,massVal);
             mxSetFieldByNumber(plhs[0],1,2,intenVal);
             mxSetFieldByNumber(plhs[0],1,3,noiseVal);
             
             mxFree(readsVal);
             mxFree(massVal);
             mxFree(intenVal);
             mxFree(noiseVal);
     */   
   
   mxArray *reads_out,*intensity_out,*mass_out,*noise_out;
   double *reads,*intensity,*mass,*noise;
   
   
   reads_out = plhs[0] =  mxCreateDoubleMatrix(1,1,mxREAL);
   mass_out = plhs[1]= mxCreateDoubleMatrix(sz,1,mxREAL);
   intensity_out = plhs[2]= mxCreateDoubleMatrix(sz,1,mxREAL);
   noise_out = plhs[3]= mxCreateDoubleMatrix(sz,1,mxREAL);
   
   reads = mxGetPr(reads_out);
   mass = mxGetPr(mass_out);
   intensity = mxGetPr(intensity_out);
   noise = mxGetPr(noise_out);
    
   reads[0] = scan.reads;
   for(read_i=0; (read_i) < sz ;(read_i)++){
        mass[read_i] = scan.mass[read_i];
        intensity[read_i] = scan.intensity[read_i];
        noise[read_i]= (double)scan.noise[read_i];
   }
   
    
   free(scan.intensity);
   free(scan.mass);
   free(scan.noise);
   hr=m_Raw->Close();
   
  
   
   
    return;
}