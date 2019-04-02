classdef DdsSlice
    
	% DirectXTex Image properties
	
	properties (GetAccess = public, SetAccess = protected)
		Width
		Height
		RowPitch
		SlicePitch
		Pixels
	end
	
	% These should be given by Dds handler
	properties (Access = protected)
		FormatID
		Flags
	end
	
	methods
		function obj = DdsSlice(images)
			if(nargin > 0)
				m = size(images,1);
				n = size(images,2);
				obj(m,n) = obj;
				for i = 1:m
					for j = 1:n
						obj(i,j).Width = images(i,j).Width;
						obj(i,j).Height = images(i,j).Height;
						obj(i,j).RowPitch = images(i,j).RowPitch;
						obj(i,j).SlicePitch = images(i,j).SlicePitch;
						obj(i,j).Pixels = images(i,j).Pixels;
						obj(i,j).FormatID = images(i,j).FormatID;
						obj(i,j).Flags = images(i,j).Flags;
					end
				end
			end
		end
		
		function h = imshow(obj)
			h = imshow(toimage(obj));
		end
		
		function varargout = toimage(obj)
			nout = max(nargout,1);
			[varargout{1:nout}] = ddsmex('TO_IMAGE', struct(obj));
		end
		
		function newdds = convert(obj, varargin)
			newdds = Dds(ddsmex('CONVERT', struct(obj), varargin{:}));
		end
		
		function newdds = flip(obj, flags)
			newdds = Dds(ddsmex('FLIP_ROTATE', struct(obj), flags));
		end
		
		function newdds = rotate(obj, flags)
			newdds = Dds(ddsmex('FLIP_ROTATE', struct(obj), flags));
		end
		
		function newdds = decompress(obj,fmt)
			if(nargin == 2)
				newdds = Dds(ddsmex('DECOMPRESS', struct(obj), fmt));
			else
				newdds = Dds(ddsmex('DECOMPRESS', struct(obj)));
			end
		end
		
		function s = struct(obj)
			m = size(obj,1);
			n = size(obj,2);
			for i = m:-1:1
				for j = n:-1:1
					s(i,j) = struct('Width',      obj(i,j).Width, ...
								 'Height',     obj(i,j).Height, ...
								 'FormatID',   obj(i,j).FormatID, ...
								 'RowPitch',   obj(i,j).RowPitch, ...
								 'SlicePitch', obj(i,j).SlicePitch, ...
								 'Pixels',     obj(i,j).Pixels,...
								 'Flags',      obj(i,j).Flags);
				end
			end	
		end
		
	end
end

