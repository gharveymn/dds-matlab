classdef Dds
	%DDS Summary of this class goes here
	%   Detailed explanation goes here
    
    properties (GetAccess = public, SetAccess = protected)
	  Width
	  Height
	  Depth
	  ArraySize
	  MipLevels
	  MiscFlags
	  MiscFlags2
	  Format
	  Dimension
    end
    
    properties (GetAccess = public, SetAccess = protected)
	    Images
    end
    
    	properties (Access = protected)
		FormatID
	end
	
	methods
		function obj = Dds(metadata, images)
			
			if(nargin > 0)
				obj.Width      = metadata.Width;
				obj.Height     = metadata.Height;
				obj.Depth      = metadata.Depth;
				obj.ArraySize  = metadata.ArraySize;
				obj.MipLevels  = metadata.MipLevels;
				obj.MiscFlags  = metadata.MiscFlags;
				obj.MiscFlags2 = metadata.MiscFlags2;
				obj.Format     = metadata.Format;
				obj.Dimension  = metadata.Dimension;
				obj.FormatID   = metadata.FormatID;
				if(nargin > 1)
					obj.Images = DdsImage(images);
				end
			end
		end
		
		function mip = GetMip(obj, varargin)
			
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
			
			if(arrayidx > size(obj.Data,1))
				error('arrayidx exceeds number of available items.');
			end
			
			if(mipidx > size(obj.Data, 2))
				error('mipidx exceeds number of available mipmaps per item.');
			end
			
			mip = obj.Data{arrayidx, mipidx};
			
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

