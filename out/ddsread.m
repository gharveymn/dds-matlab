function dds = ddsread(filename, flags)
	%DDSREAD Summary of this function goes here
	%   Detailed explanation goes here
	
	validateattributes(filename,{'char'},{'nonempty'},'ddsread','FILENAME',1);
	
	if(nargin > 1)
		[metadata,image] = ddsio('READ',filename, flags);
	else
		[metadata,image] = ddsio('READ',filename);
	end
	
	dds = Dds(metadata, image);
end
