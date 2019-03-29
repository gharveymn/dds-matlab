classdef DdsImage
    
	% DirectXTex Image properties
	
	properties (GetAccess = public, SetAccess = protected)
		Width
		Height
		Depth
		RowPitch
		SlicePitch
		Pixels
	end
	
	% This should be given by Dds handler
	properties (Access = protected)
		FormatID
	end
	
	methods
		function obj = DdsImage(images, formatid)
			if(nargin > 0)
				m = size(images,1);
				n = size(images,2);
				obj(m,n) = obj;
				for i = 1:m
					for j = 1:n
						obj(i,j).Width = images(i,j).Width;
						obj(i,j).Height = images(i,j).Height;
						obj(i,j).Depth    = images(i,j).Depth;
						obj(i,j).FormatID = formatid;
						obj(i,j).RowPitch = images(i,j).RowPitch;
						obj(i,j).SlicePitch = images(i,j).SlicePitch;
						obj(i,j).Pixels = images(i,j).Pixels;
					end
				end
			end
		end
		
		function imshow(obj)
			imshow(toimage(obj));
		end
		
		function varargout = toimage(obj)
			nout = max(nargout,1);
			[varargout{1:nout}] = ddsio('IMAGE_MATRIX', struct(obj));
		end
		
		function newdds = convert(obj, varargin)
			newdds = Dds(ddsio('CONVERT', struct(obj), varargin{:}));
		end
		
		function newdds = flip(obj, flags)
			newdds = Dds(ddsio('FLIP_ROTATE', struct(obj), flags));
		end
		
		function newdds = rotate(obj, flags)
			newdds = Dds(ddsio('FLIP_ROTATE', struct(obj), flags));
		end
		
		function newdds = decompress(obj,fmt)
			if(nargin == 2)
				newdds = Dds(ddsio('DECOMPRESS', struct(obj), fmt));
			else
				newdds = Dds(ddsio('DECOMPRESS', struct(obj)));
			end
		end
		
		function s = struct(obj)
			m = size(obj,1);
			n = size(obj,2);
			for i = m:-1:1
				for j = n:-1:1
					s(i,j) = struct('Width',      obj(i,j).Width, ...
								 'Height',     obj(i,j).Height, ...
								 'Depth',      obj(i,j).Depth, ...
								 'FormatID',   obj(i,j).FormatID, ...
								 'RowPitch',   obj(i,j).RowPitch, ...
								 'SlicePitch', obj(i,j).SlicePitch, ...
								 'Pixels',     obj(i,j).Pixels);
				end
			end	
		end
		
	end
end

