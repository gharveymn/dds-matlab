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
		
		function imshow(obj)
			if(obj.Metadata.IsCubeMap)
				h = obj.Images(1).Height;
				w = obj.Images(1).Width;
				cubearr(:,:,:,2) = toimage(obj.Images(3));
				cubearr(:,:,:,1) = zeros(h, w, 3);
				cubearr(:,:,:,3) = zeros(h, w, 3);
				cubearr(:,:,:,4) = zeros(h, w, 3);
				cubearr(:,:,:,5) = toimage(obj.Images(2));
				cubearr(:,:,:,6) = toimage(obj.Images(5));
				cubearr(:,:,:,7) = toimage(obj.Images(1));
				cubearr(:,:,:,8) = toimage(obj.Images(6));
				cubearr(:,:,:,9) = zeros(h, w, 3);
				cubearr(:,:,:,10) = toimage(obj.Images(4));
				cubearr(:,:,:,11) = zeros(h, w, 3);
				cubearr(:,:,:,12) = zeros(h, w, 3);
				montage(cubearr, 'Size', [3,4]);
			else
				imshow(obj.Images(1));
			end
		end
		
		function newdds = convert(obj, varargin)
			newdds = Dds(ddsio('CONVERT_DDS', struct(obj), varargin{:}));
		end
		
		function newdds = flip(obj, flags)
			newdds = Dds(ddsio('FLIP_ROTATE_DDS',struct(obj), flags));
		end
		
		function newdds = rotate(obj, flags)
			newdds = Dds(ddsio('FLIP_ROTATE_DDS',struct(obj), flags));
		end
		
		function newdds = decompress(obj,fmt)
			if(nargin == 2)
				newdds = Dds(ddsio('DECOMPRESS_DDS', struct(obj), fmt));
			else
				newdds = Dds(ddsio('DECOMPRESS_DDS', struct(obj)));
			end
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

