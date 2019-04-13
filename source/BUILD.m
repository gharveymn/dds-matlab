function BUILD
%% BUILD  Run this file to compile <strong>dxtmex</strong>.
%    The output is written to '../out/private'.
%
%    Thanks for downloading!
	
	opts = OPTIONS;

	thisfolder = fileparts(which(mfilename));
	
	output_path  = fullfile(thisfolder,'..', 'out', 'private');
	header_path  = fullfile(thisfolder, 'lib', 'DirectXTex', 'DirectXTex');
	library_path = fullfile(thisfolder, 'lib', 'DirectXTex', 'DirectXTex', 'Bin', 'Desktop_2017', 'x64', 'Release');

	mexflags = {'-O', '-v', '-outdir', output_path, ...
		['-I' header_path], ['-L' library_path], '-lDirectXTex'};

	if(opts.debug)
		mexflags = [mexflags {'-g'}];
	end
	
	sources = {
		'dxtmex.cpp',...
		'dxtmex_mexerror.cpp',...
		'dxtmex_mexutils.cpp',...
		'dxtmex_maps.cpp',...
		'dxtmex_dxtimagearray.cpp',...
		'dxtmex_dxtimage.cpp',...
		'dxtmex_pixel.cpp'
		};

	for i = 1:numel(sources)
		sources{i} = fullfile(fileparts(which(mfilename)),'src',sources{i});
	end

% 	if(maxsz > 2^31-1)
% 		% R2018a
% 		if(verLessThan('matlab','9.4'))
% 			mexflags = [mexflags {'-largeArrayDims'}];
% 		else
% 			warning(['Compiling in compatibility mode; the R2018a' ...
% 				'MEX API may not support certain functions '...
% 				'which are integral to this function'])
% 			mexflags = [mexflags, {'-R2017b'}];
% 		end
% 	else
% 		mexflags = [mexflags {'-compatibleArrayDims'}];
% 	end

	fprintf('-Compiling dxtmex...')
	mexflags = [mexflags {'COMPFLAGS="$COMPFLAGS /W4"'}];
	% mexflags = [mexflags {'CFLAGS="$CFLAGS -Wall -Werror -Wno-unused-function"'}];
	mex(mexflags{:} , sources{:})
	fprintf(' successful.\n')
	if(opts.debug)
		fprintf('-Compiled for DEBUGGING.\n');
	else
		fprintf('-Compiled for RELEASE.\n');
	end
	fprintf('%s\n', ['-The function is located in ' output_path '.']);
end