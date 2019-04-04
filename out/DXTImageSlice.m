classdef DXTImageSlice
    
	% DirectXTex Image properties
	
	properties (GetAccess = public, SetAccess = protected)
		Width
		Height
		RowPitch
		SlicePitch
		Pixels
	end
	
	% These should be given by DXTImage handler
	properties (Access = protected)
		FormatID
		Flags
	end
	
	methods
		function obj = DXTImageSlice(images)
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
			[varargout{1:nout}] = dxtmex('TO_IMAGE', struct(obj));
		end
		
		function ddswrite(obj, varargin)
			dxtmex('WRITE_DDS', struct(obj), varargin{:});
		end
		
		function hdrwrite(obj, varargin)
			dxtmex('WRITE_HDR', struct(obj), varargin{:});
		end
		
		function tgawrite(obj, varargin)
			dxtmex('WRITE_TGA', struct(obj), varargin{:});
		end
		
		function obj = flip(obj, varargin)
			obj = DXTImage(dxtmex('FLIP_ROTATE', struct(obj), varargin{:}));
		end
		
		function obj = rotate(obj, varargin)
			obj = DXTImage(dxtmex('FLIP_ROTATE', struct(obj), varargin{:}));
		end
		
		function obj = resize(obj, varargin)
			obj = DXTImage(dxtmex('RESIZE', struct(obj), varargin{:}));
		end
		
		function obj = convert(obj, varargin)
			obj = DXTImage(dxtmex('CONVERT', struct(obj), varargin{:}));
		end
		
		function obj = convertToSinglePlane(obj, varargin)
			obj = DXTImage(dxtmex('CONVERT_TO_SINGLE_PLANE', struct(obj), varargin{:}));
		end
		
		function obj = generateMipMaps(obj, varargin)
			obj = DXTImage(dxtmex('GENERATE_MIPMAPS', struct(obj), varargin{:}));
		end
		function obj = generateMipMaps3D(obj, varargin)
			obj = DXTImage(dxtmex('GENERATE_MIPMAPS_3D', struct(obj), varargin{:}));
		end
		function obj = scaleMipMapsAlphaForCoverage(obj, varargin)
			obj = DXTImage(dxtmex('SCALE_MIPMAPS_ALPHA_FOR_COVERAGE', struct(obj), varargin{:}));
		end
		function obj = premultiplyAlpha(obj, varargin)
			obj = DXTImage(dxtmex('PREMULTIPLY_ALPHA', struct(obj), varargin{:}));
		end
		
		function obj = compress(obj, varargin)
			obj = DXTImage(dxtmex('COMPRESS', struct(obj), varargin{:}));
		end
		
		function obj = decompress(obj, varargin)
			obj = DXTImage(dxtmex('DECOMPRESS', struct(obj), varargin{:}));
		end
		
		function obj = computeNormalMap(obj, varargin)
			obj = DXTImage(dxtmex('COMPUTE_NORMAL_MAP', struct(obj), varargin{:}));
		end
		
		function obj = copyRectangle(obj, src, varargin)
			obj = DXTImage(dxtmex('COPY_RECTANGLE', struct(obj), struct(src), varargin{:}));
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

