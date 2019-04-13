function varargout = readdds(varargin)
	if(nargout > 0)
		varargout = cell(nargout, 1);
	end
	dxtimage = DXTImage(dxtmex('READ_DDS',varargin{:}));
	switch(nargout)
		case 1
			% manual case
			varargout{1} = dxtimage;
		case 2
			% imread case
			varargout{1} = dxtimage(1).Images(1).toimage;
			varargout{2} = [];
		case 3
			% imread case
			% return alpha channel
			[varargout{1},varargout{3}] = dxtimage(1).Images(1).toimage;
			varargout{2} = [];
	end
end