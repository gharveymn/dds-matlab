classdef Dds
	%DDS Summary of this class goes here
	%   Detailed explanation goes here
	
	properties (GetAccess = public, SetAccess = protected)
		Metadata
	end
	
	properties (GetAccess = public, SetAccess = protected)
		Images
	end
	
	properties (Access = protected)
		FormatID
	end
	
	methods
		function obj = Dds(s)
			if(nargin > 0)
				m = size(s,1);
				n = size(s,2);
				for i = m:-1:1
					for j = n:-1:1
						obj(i,j).Metadata = s(i,j).Metadata;
						obj(i,j).Images = DdsSlice(s(i,j).Images, obj(i,j).Metadata.Format.ID);
					end
				end
			end
		end
		
		function h = imshow(obj, option)
			if(obj.Metadata.IsCubeMap)
				if(nargin == 2)
					switch(option)
						case 'sphere'
							h = dispsphere(obj);
						case 'fishbowl'
							h = dispfishbowl(obj);
						case 'cube'
							h = dispcube(obj);
						case 'flat'
							h = dispflat(obj);
						otherwise
							error('Dds:InvalidOptionError',['Invalid option ''' option '''.']);
					end
				else
					h = dispflat(obj);
				end
			else
				h = imshow(obj.Images(1));
			end
		end
		
		function newdds = getMipLevel(obj, lvl)
			newdds = Dds;
			newdds.Metadata = obj.Metadata;
			newdds.Metadata.MipLevels = 1;
			newdds.Images = obj.Images(:, lvl);
		end
		
		function newdds = convert(obj, varargin)
			newdds = Dds(ddsmex('CONVERT', struct(obj), varargin{:}));
		end
		
		function newdds = flip(obj, flags)
			newdds = Dds(ddsmex('FLIPROTATE',struct(obj), flags));
		end
		
		function newdds = rotate(obj, flags)
			newdds = Dds(ddsmex('FLIPROTATE',struct(obj), flags));
		end
		
		function newdds = decompress(obj,fmt)
			if(nargin == 2)
				newdds = Dds(ddsmex('DECOMPRESS', struct(obj), fmt));
			else
				newdds = Dds(ddsmex('DECOMPRESS', struct(obj)));
			end
		end
		
		function s = struct(obj)
			m = size(obj,1);
			n = size(obj,2);
			for i = m:-1:1
				for j = n:-1:1
					s(i,j) = struct('Metadata', obj(i,j).Metadata, 'Images', struct(obj(i,j).Images));
				end
			end			
		end
	end
	
	methods (Access = protected)
		
		function h = dispflat(obj)
			cubearr = cell(12,1);
			% indexing reference: https://en.wikipedia.org/wiki/Cube_mapping#/media/File:Cube_map.svg
			cubearr{2} =  toimage(obj.Images(3));
			cubearr{5} =  toimage(obj.Images(2));
			cubearr{6} =  toimage(obj.Images(5));
			cubearr{7} =  toimage(obj.Images(1));
			cubearr{8} =  toimage(obj.Images(6));
			cubearr{10} = toimage(obj.Images(4));
			h = figure;
			montage(cubearr, 'Size', [3,4]);
		end
		
		function h = dispcube(obj)
			x = linspace(1,-1,obj.Metadata.Width);
			z = linspace(1,-1,obj.Metadata.Height);
			[X,Z] = meshgrid(x,z);
			Y = ones(obj.Metadata.Height, obj.Metadata.Width);
			
			h = figure;
			hold on
			xlim([-1,1])
			ylim([-1,1])
			zlim([-1,1])
			xlabel x
			ylabel y
			zlabel z
			axis vis3d
			camproj('perspective');
			warp( X,-Y, Z,obj.Images(1).toimage);
			warp(-X, Y, Z,obj.Images(2).toimage);
			warp(-Z, X, Y,obj.Images(3).toimage);
			warp( Z, X,-Y,obj.Images(4).toimage);
			warp( Y, X, Z,obj.Images(5).toimage);
			warp(-Y,-X, Z,obj.Images(6).toimage);
			view([52.5, 30])
			cameratoolbar('SetMode', 'orbit')
			hold off
		end
		
		function h = dispsphere(obj)
			% reference: https://en.wikipedia.org/wiki/Cube_mapping
			x = linspace(1,-1,obj.Metadata.Width);
			z = linspace(1,-1,obj.Metadata.Height);
			[X,Z] = meshgrid(x,z);
			Y = ones(obj.Metadata.Height, obj.Metadata.Width);
	
		%	Naive:	
		% 	R = sqrt(X.^2 + Y.^2 + Z.^2);
		% 	THETA = acos(Z./R);
		% 	PHI = -atan(X);
		% 	
		% 	Xs = sin(THETA).*cos(PHI);
		% 	Ys = sin(THETA).*sin(PHI);
		% 	Zs = cos(THETA);

		%	Simplified:
			Zs = Z./sqrt(X.^2 + Y.^2 + Z.^2);
			A  = sqrt(1 - Zs.^2);
			B  = sqrt(X.^2 + 1);
			Xs =  A./B;
			Ys = -X.*Xs;

			h = figure;
			hold on
			xlim([-1,1])
			ylim([-1,1])
			zlim([-1,1])
			xlabel x
			ylabel y
			zlabel z
			axis vis3d
			camproj('perspective');
			warp( Xs,-Ys, Zs,obj.Images(1).toimage);
			warp(-Xs, Ys, Zs,obj.Images(2).toimage);
			warp( Ys,-Zs, Xs,obj.Images(3).toimage);
			warp( Ys, Zs,-Xs,obj.Images(4).toimage);
			warp( Ys, Xs, Zs,obj.Images(5).toimage);
			warp(-Ys,-Xs, Zs,obj.Images(6).toimage);
			view([52.5, 30])
			cameratoolbar('SetMode', 'orbit');
			hold off
		end
		
		function h = dispfishbowl(obj)
			% reference: https://en.wikipedia.org/wiki/Cube_mapping
			x = linspace(1,-1,obj.Metadata.Width);
			z = linspace(1,-1,obj.Metadata.Height);
			[X,Z] = meshgrid(x,z);
			Y = ones(obj.Metadata.Height, obj.Metadata.Width);

			Zs = Z./sqrt(X.^2 + Y.^2 + Z.^2);
			A  = sqrt(1 - Zs.^2);
			B  = sqrt(X.^2 + 1);
			Xs =  A./B;
			Ys = -X.*Xs;

			% this is just mirrored from the sphere projection
			h = figure;
			hold on			
			warp( Xs, Ys, Zs,obj.Images(1).toimage);
			warp(-Xs,-Ys, Zs,obj.Images(2).toimage);
			warp( Ys, Zs, Xs,obj.Images(3).toimage);
			warp( Ys,-Zs,-Xs,obj.Images(4).toimage);
			warp( Ys,-Xs, Zs,obj.Images(5).toimage);
			warp(-Ys, Xs, Zs,obj.Images(6).toimage);
			camtarget([1,0,0])
			camproj('perspective')
			campos([0,0,0])
			camva(60);
			camup([0,0,1])
			campan(0,0)
			camroll(0)
			cameratoolbar('SetMode','pan');
			hold off
		end
		
	end
	
	
	methods (Static)
		function obj = read(varargin)
			obj = Dds(ddsmex('READ',varargin{:}));
		end
		
		function metadata = finfo(varargin)
			metadata = ddsmex('READMETADATA', varargin{:});
		end
	end
	
end

