close all;
figure;
keyboard;
for num=1:20
	I=min(abs(trnd(1,700))*2^-num,10);
	imshow(I);
	FastSExtractor(I);
end
