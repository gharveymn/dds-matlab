function h = cubeproj2(d)
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
	props = {'EdgeColor', 'none'};
	surf( Xs, Ys, Zs,d.Images(1).toimage, props{:});
	surf(-Xs,-Ys, Zs,d.Images(2).toimage, props{:});
	surf( Ys, Zs, Xs,d.Images(3).toimage, props{:});
	surf( Ys,-Zs,-Xs,d.Images(4).toimage, props{:});
	surf( Ys,-Xs, Zs,d.Images(5).toimage, props{:});
	surf(-Ys, Xs, Zs,d.Images(6).toimage, props{:});
	view([52.5, 30])
	hold off
end
