function dxtimage = readhdr(varargin)
	dxtimage = DXTImage(dxtmex('READ_HDR',varargin{:}));
end