function dxtimage = readtga(varargin)
	dxtimage = DXTImage(dxtmex('READ_TGA',varargin{:}));
end