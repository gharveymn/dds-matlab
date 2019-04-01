function h = cubeproj3(d)
	% reference: https://en.wikipedia.org/wiki/Cube_mapping
	x = linspace(1,-1,d.Metadata.Width);
	z = linspace(1,-1,d.Metadata.Height);
	[X,Z] = meshgrid(x,z);
	Y = ones(d.Metadata.Height, d.Metadata.Width);

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
	ax = gca;
	hold on
	xlim([-1,1])
	ylim([-1,1])
	zlim([-1,1])
	xlabel x
	ylabel y
	zlabel z
 	axis vis3d
	ax.Projection = 'perspective';
	warp( Xs,-Ys, Zs,d.Images(1).toimage);
	warp(-Xs, Ys, Zs,d.Images(2).toimage);
	warp( Ys,-Zs, Xs,d.Images(3).toimage);
	warp( Ys, Zs,-Xs,d.Images(4).toimage);
	warp( Ys, Xs, Zs,d.Images(5).toimage);
	warp(-Ys,-Xs, Zs,d.Images(6).toimage);
	view([52.5, 30])
	hold off
end
