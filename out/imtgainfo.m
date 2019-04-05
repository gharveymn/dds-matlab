function metadata = imtgainfo(varargin)
	metadata = dxtmex('READ_TGA_META', varargin{:});
end