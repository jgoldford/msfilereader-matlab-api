function [experiment] = getRawData( path , varargin)
%getScanStruct: Converts ORBITRAP scan data from .raw file to matlab structure
%array
%   Inputs: path - string that represents full path where the raw file is
%   located
%   Outputs: scans - structure containing the following fields:
%                   .reads = total number of data points
%                   .mz = mass to charge ratio array;
%                   .intensity = raw intensity measurement array;
%                   .noise = raw noise measurement array;

%create structure for output
experiment = struct;
scans = struct;

allLabel = varargin{1};

%call mex function getTotalScans to get Total number of scans
numScans = getTotalScans(path);
%for each scan, obtain total number of data points (reads), mz,intensity
%and noise arrays
for i=1:numScans
    [experiment.RT(i)] = RTfromScanNum(path,i);
    if allLabel
        [scans(i).reads,scans(i).mz,scans(i).intensity,scans(i).noise,scans(i).resolution,scans(i).baseline,scans(i).charge]=GetAllLabelData(path,i);  
    else
    [scans(i).reads,scans(i).mz,scans(i).intensity,scans(i).noise]=getLabelData(path,i); 
end
experiment.scans = scans;

end
