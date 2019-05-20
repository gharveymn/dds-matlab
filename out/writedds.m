function writedds(data, map, filename, varargin)
	dxtmex('WRITE_MATRIX_DDS', data, map, filename, varargin{:});
end
