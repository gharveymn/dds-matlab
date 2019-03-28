classdef Dds
	%DDS Summary of this class goes here
	%   Detailed explanation goes here
	
	properties (GetAccess = public, SetAccess = protected)
		Metadata
	end
	
	properties (GetAccess = public, SetAccess = protected)
		Images
	end
	
	properties (Access = protected)
		FormatID
	end
	
	methods
		function obj = Dds(ddsstruct)
			if(nargin > 0)
				m = size(ddsstruct,1);
				n = size(ddsstruct,2);
				for i = m:-1:1
					for j = n:-1:1
						obj(i,j).Metadata = ddsstruct(i,j).Metadata;
						obj(i,j).Images = DdsImage(ddsstruct(i,j).Images, obj(i,j).Metadata.Format.ID);
					end
				end
			end
		end
		
		function newdds = convert(obj, varargin)
			newdds = Dds(ddsio('CONVERT_DDS', struct(obj), varargin{:}));
		end
		
		function newdds = flip(obj, flags)
			newdds = Dds(ddsio('FLIPROTATE_DDS',struct(obj), flags));
		end
		
		function newdds = rotate(obj, flags)
			newdds = Dds(ddsio('FLIPROTATE_DDS',struct(obj), flags));
		end
		
		function s = struct(obj)
			m = size(obj,1);
			n = size(obj,2);
			for i = m:-1:1
				for j = n:-1:1
					s(i,j) = struct('Metadata', obj(i,j).Metadata, 'Images', struct(obj(i,j).Images));
				end
			end			
		end
		
	end
	
	methods (Static)
		function obj = read(varargin)
			obj = Dds(ddsio('READ',varargin{:}));
		end
		
		function metadata = finfo(varargin)
			metadata = ddsio('READ_METADATA', varargin{:});
		end
	end
	
end

