classdef DdsImage
    
	% DirectXTex Image properties
	
	properties (GetAccess = public, SetAccess = protected)
		Width
		Height
		RowPitch
		SlicePitch
		Pixels
	end
	
	properties (Access = protected)
		FormatID
	end
	
	% Custom interfacing properties
	
	properties (GetAccess = public, SetAccess = protected)
		Format
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
		
		function newobj = convert(obj, newfmt, filter, threshold)
			
			imgpack = {obj.Width, ...
					 obj.Height, ...
					 obj.FormatID, ...
					 obj.RowPitch, ...
					 obj.SlicePitch, ...
					 obj.Pixels};
			
			if(nargin > 2)
				if(nargin > 3)
					newimg = ddsio('CONVERT', imgpack, newfmt, filter, threshold);
				else
					newimg = ddsio('CONVERT', imgpack, newfmt, filter);
				end
			else
				newimg = ddsio('CONVERT', imgpack, newfmt);
			end
			newobj = DdsImage(newimg);
		end
		
	end
end

