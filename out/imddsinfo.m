function metadata = imddsinfo(varargin)
	metadata = dxtmex('READ_DDS_META', varargin{:});
end