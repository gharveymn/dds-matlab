function dxtimage = readdds(varargin)
	dxtimage = DXTImage(dxtmex('READ_DDS',varargin{:}));
end