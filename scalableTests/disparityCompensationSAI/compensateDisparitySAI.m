function [ compensatedIm0 ] = compensateDisparitySAI( referenceImg, dx, dy )

[H W] = size(referenceImg);
compensatedIm0 = referenceImg;

for j = 1:H
    for i = 1:W
        if j - dy(j,i) > 0 && j - dy(j,i) <= H && i - dx(j,i) > 0 && i - dx(j,i) <= W
            compensatedIm0(j,i) = referenceImg(j - dy(j,i) ,i - dx(j,i));
        end
    end
end


end

