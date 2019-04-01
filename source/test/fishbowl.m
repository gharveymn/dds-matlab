function h = fishbowl(imgs)
% reference: https://en.wikipedia.org/wiki/Cube_mapping
	x = linspace(1,-1,size(imgs{1},2));
	z = linspace(1,-1,size(imgs{1},1));
	[X,Z] = meshgrid(x,z);
	Y = ones(size(imgs{1},1), size(imgs{1},2));

	Zs = Z./sqrt(X.^2 + Y.^2 + Z.^2);
	A  = sqrt(1 - Zs.^2);
	B  = sqrt(X.^2 + 1);
	Xs =  A./B;
	Ys = -X.*Xs;

	h = figure;
	hold on
	warp( Xs, Ys, Zs,imgs{1});
	warp(-Xs,-Ys, Zs,imgs{2});
	warp( Ys, Zs, Xs,imgs{3});
	warp( Ys,-Zs,-Xs,imgs{4});
	warp( Ys,-Xs, Zs,imgs{5});
	warp(-Ys, Xs, Zs,imgs{6});
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
