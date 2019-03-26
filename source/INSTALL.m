function INSTALL
%% INSTALL  Run this file to compile <strong>ddsread</strong>.
%    The output is written to '../out/private'.
%
%    Thanks for downloading!
	
	opts = OPTIONS;

	thisfolder = fileparts(which(mfilename));
	
	output_path  = fullfile(thisfolder,'..', 'out', 'private');
	header_path  = fullfile(thisfolder, 'src', 'headers');
	library_path = fullfile(thisfolder, 'lib', 'x64');

	mexflags = {'-O', '-v', '-outdir', output_path, ...
		['-I' header_path], ['-L' library_path], '-lDirectXTex'};

	if(opts.debug)
		mexflags = [mexflags {'-g'}];
	end
	
	sources = {
		'dds.cpp',...
		'mlerrorutils.cpp'
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

	fprintf('-Compiling dds...')
	% mexflags = [mexflags {'COMPFLAGS="$COMPFLAGS /O2"'}];
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