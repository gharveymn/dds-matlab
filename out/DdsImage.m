classdef DdsImage
    
	properties (GetAccess = public, SetAccess = protected)
		Width
		Height
		Format
		RowPitch
		SlicePitch
		Pixels
	end
	
	properties (Access = protected)
		FormatID
	end
	
	methods
		function obj = DdsImage(images)
			if(nargin > 0)
				m = size(images,1);
				n = size(images,2);
				obj(m,n) = obj;
				for i = 1:m
					for j = 1:n
						obj(i,j).Width = images(i,j).Width;
						obj(i,j).Height = images(i,j).Height;
						obj(i,j).Format = images(i,j).Format;
						obj(i,j).RowPitch = images(i,j).RowPitch;
						obj(i,j).SlicePitch = images(i,j).SlicePitch;
						obj(i,j).Pixels = images(i,j).Pixels;
						obj(i,j).FormatID = images(i,j).FormatID;
					end
				end
			end
		end
		
		function img = GetImage(obj, varargin)
			if(nargin == 2)
				mipidx = varargin{1};
			else
				mipidx = 1;
			end
			
			if(nargin == 3)
				arrayidx = varargin{2};
			else
				arrayidx = 1;
			end
			
			mip = obj.GetMip(mipidx, arrayidx);
			
			% convert to srgb
			
		end
		
	end
end

