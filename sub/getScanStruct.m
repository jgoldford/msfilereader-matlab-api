function [scans] = getScanStruct( path )
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
scans = struct;
%call mex function getTotalScans to get Total number of scans
numScans = getTotalScans(path);
%for each scan, obtain total number of data points (reads), mz,intensity
%and noise arrays
for i=1:numScans
    [scans(i).reads,scans(i).mz,scans(i).intensity,scans(i).noise]=getLabelData(path,i);

end
end

