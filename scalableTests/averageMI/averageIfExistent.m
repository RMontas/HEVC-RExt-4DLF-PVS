function [ p ] = averageIfExistent( C )
% C is a 3x3 matrix 
existentPixels = zeros(3,3);
numExistentPixels = 0;
for j = 1:3
    for i = 1:3
        if not(j == 2 && i == 2) 
            if C(j,i) >= 0
                existentPixels(j,i) = 1;
                numExistentPixels = numExistentPixels + 1;
            end
        end
    end
end

p=0;
if numExistentPixels > 0
    p = round(sum(sum(C.*existentPixels))/numExistentPixels);
end


end

