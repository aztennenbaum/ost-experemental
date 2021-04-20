function [ Px, Py ] = idx2xy(idx, img_height)
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here

    Px=floor((idx-1)/img_height)+1;
    Py=mod(idx-1,img_height)+1;

end

