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

using namespace std;

void mexFunction(int nlhs,mxArray *plhs[],int nrhs,const mxArray *prhs[])
{
    //make sure there is exactly one input
    if (nrhs != 1) {
        mexErrMsgIdAndTxt("MATLAB:mexevalstring:nInput", "Oneinput arguments required.");
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

    
    //Set up MEX output (same steps as above...)
    mxArray *resolution;
    double *output;
    resolution = plhs[0] = mxCreateDoubleMatrix(1,1,mxREAL);
    output = mxGetPr(resolution);
    //**************************************************//    
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
     m_Raw->SetCurrentController(0, 200);
    
  /*  long firstScanNumber = 0, lastScanNumber = 0;

    // Verifies if can get the first scan
    hr = m_Raw->GetFirstSpectrumNumber(&firstScanNumber);
    if (FAILED(hr)) {
        mexErrMsgTxt("ERROR: Unable to get first scan.");
    }

    // Ask for the last scan number and check to make sure scan is within bounds
    m_Raw->GetLastSpectrumNumber(&lastScanNumber);
 
    */
    double dHalfMassRes;
    hr = m_Raw->GetMassResolution(&dHalfMassRes);
    if(FAILED(hr)){
        mexErrMsgTxt("ERROR: Unable to get mass resolution.");
        
    }
    
  //output retention time
    *output = dHalfMassRes; 
    
  
  //close raw file
    hr=m_Raw->Close();
      if (FAILED(hr)) {
        mexErrMsgTxt("ERROR: Unable to close RAW file.");
        }

}