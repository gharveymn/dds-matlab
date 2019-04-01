function h = cubeproj(d)
	% reference: https://en.wikipedia.org/wiki/Cube_mapping
	x = linspace(1,-1,d.Metadata.Width);
	z = linspace(1,-1,d.Metadata.Height);
	[X,Z] = meshgrid(x,z);
	Y = ones(d.Metadata.Height, d.Metadata.Width);
	
	Xsq = X.^2;
	Ysq = Y.^2;
	Zsq = Z.^2;
	
	Xs = X.*sqrt(1 - Ysq./2 - Zsq./2 + Ysq .* Zsq ./ 3);
	Ys = Y.*sqrt(1 - Zsq./2 - Xsq./2 + Zsq .* Xsq ./ 3);
	Zs = Z.*sqrt(1 - Xsq./2 - Ysq./2 + Xsq .* Ysq ./ 3);
	
	h = figure;
	hold on
	surf( Xs, Ys, Zs,d.Images(1).toimage, 'EdgeColor', 'none');
	surf(-Xs,-Ys, Zs,d.Images(2).toimage, 'EdgeColor', 'none');
	surf(-Zs,-Xs, Ys,d.Images(3).toimage, 'EdgeColor', 'none');
	surf( Zs,-Xs,-Ys,d.Images(4).toimage, 'EdgeColor', 'none');
	surf( Ys,-Xs, Zs,d.Images(5).toimage, 'EdgeColor', 'none');
	surf(-Ys, Xs, Zs,d.Images(6).toimage, 'EdgeColor', 'none');
	xlim([-1,1])
	ylim([-1,1])
	zlim([-1,1])
	xlabel x
	ylabel y
	zlabel z
	axis vis3d
end
