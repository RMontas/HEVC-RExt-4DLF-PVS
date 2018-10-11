function [ dx, dy ] = disparityCompensationSAI_BM( im0, im1, window, blockSize )
%% block matching between two KNOWN frames to dertermine disparity

[H,W]=size(im0);
bh = H / blockSize;
bw = W / blockSize;

dx = zeros(H,W);
dy = zeros(H,W);

for j = 1:bh
    for i = 1:bw
        dispVectorX = 0;
        dispVectorY = 0;
        currentDistortion = Inf;
        bestDistortion = Inf;
        for windH = window(1,1):window(2,1)
            %for windW = -window(2):0 %% searching the left view on the right view vectors should be negative
            for windW = window(1,2):window(2,2)
                if (j-1)*blockSize + 1 + windH > 0 && (j-1)*blockSize + blockSize + windH <= H && ...
                   (i-1)*blockSize + 1 + windW > 0 && (i-1)*blockSize + blockSize + windW <= W
                    currentDistortion = immse(im0((j-1)*blockSize + 1:(j-1)*blockSize + blockSize, ...
                                                  (i-1)*blockSize + 1:(i-1)*blockSize + blockSize), ...
                                              im1((j-1)*blockSize + 1 + windH:(j-1)*blockSize + blockSize + windH, ...
                                                  (i-1)*blockSize + 1 + windW:(i-1)*blockSize + blockSize + windW));
                    if currentDistortion < bestDistortion
                        bestDistortion = currentDistortion;
                        dispVectorX = windW;
                        dispVectorY = windH;
                    end
                end
            end
        end
        dx((j-1)*blockSize + 1:(j-1)*blockSize + blockSize, ...
           (i-1)*blockSize + 1:(i-1)*blockSize + blockSize) = dispVectorX;
        dy((j-1)*blockSize + 1:(j-1)*blockSize + blockSize, ...
           (i-1)*blockSize + 1:(i-1)*blockSize + blockSize) = dispVectorY;
    end
end
    
end

