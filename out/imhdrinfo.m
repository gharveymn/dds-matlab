function metadata = imhdrinfo(varargin)
	metadata = dxtmex('READ_HDR_META', varargin{:});
end