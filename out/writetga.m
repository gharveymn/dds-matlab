function writetga(data, map, filename, varargin)
	dxtmex('WRITE_MATRIX_TGA', data, map, filename, varargin{:});
end