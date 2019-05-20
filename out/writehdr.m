function writehdr(data, map, filename, varargin)
	dxtmex('WRITE_MATRIX_HDR', data, map, filename, varargin{:});
end