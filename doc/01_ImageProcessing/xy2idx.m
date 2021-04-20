function [ idx ] = xy2idx( Px, Py, img_height)
    idx=(Px-1)*img_height+Py;
end

