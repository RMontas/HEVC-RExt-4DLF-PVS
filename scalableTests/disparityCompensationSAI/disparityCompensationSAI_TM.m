function [ dx, dy ] = disparityCompensationSAI_TM( im0, im1, window, blockSize, templateSize )
%% template matching between two frames to dertermine disparity

[H,W]=size(im0);
bh = H / blockSize;
bw = W / blockSize;

dx = zeros(H,W);
dy = zeros(H,W);
pixelsTemplate = (blockSize + templateSize) * (blockSize + templateSize) - (blockSize * blockSize);

for j = 1:bh
    for i = 1:bw
        dispVectorX = 0;
        dispVectorY = 0;
        currentDistortion = Inf;
        bestDistortion = Inf;
        %% test NULL vector
        windH = 0;
        windW = 0;
        if (j-1)*blockSize + 1 - templateSize + windH > 0 && (j-1)*blockSize + blockSize + windH <= H && ...
           (i-1)*blockSize + 1 - templateSize + windW > 0 && (i-1)*blockSize + blockSize + windW <= W && ...
           (j-1)*blockSize + 1 - templateSize > 0 && (j-1)*blockSize + blockSize <= H && ...
           (i-1)*blockSize + 1 - templateSize > 0 && (i-1)*blockSize + blockSize <= W
           for py = 1-templateSize:blockSize
               for px = 1-templateSize:blockSize
                   if py <= 0 || px <= 0
                       currentDistortion = currentDistortion + (im0((j-1)*blockSize + py,         (i-1)*blockSize + px) - ...
                           im1((j-1)*blockSize + py + windH, (i-1)*blockSize + px + windW)).^2;
                   end
               end
           end
           currentDistortion = currentDistortion / ((blockSize + templateSize) * pixelsTemplate);
           bestDistortion = currentDistortion;
           dispVectorX = windW;
           dispVectorY = windH;
        end
        %% test search window
        for windH = -window(1):window(1)
            %for windW = -window(2):0 %% searching the left view on the right view vectors should be negative
            for windW = -window(2):window(2)
                currentDistortion = 0;
                if (j-1)*blockSize + 1 - templateSize + windH > 0 && (j-1)*blockSize + blockSize + windH <= H && ...
                   (i-1)*blockSize + 1 - templateSize + windW > 0 && (i-1)*blockSize + blockSize + windW <= W && ...
                   (j-1)*blockSize + 1 - templateSize > 0 && (j-1)*blockSize + blockSize <= H && ...
                   (i-1)*blockSize + 1 - templateSize > 0 && (i-1)*blockSize + blockSize <= W
                    for py = 1-templateSize:blockSize
                        for px = 1-templateSize:blockSize
                            if py <= 0 || px <= 0
                                currentDistortion = currentDistortion + (im0((j-1)*blockSize + py,         (i-1)*blockSize + px) - ...
                                                                         im1((j-1)*blockSize + py + windH, (i-1)*blockSize + px + windW)).^2;
                            end
                        end
                    end
                    currentDistortion = currentDistortion / ((blockSize + templateSize) * pixelsTemplate);
                    if currentDistortion < bestDistortion
                        bestDistortion = currentDistortion;
                        dispVectorX = windW;
                        dispVectorY = windH;
                    end
                end
            end
        end
        %% write best output
        dx((j-1)*blockSize + 1:(j-1)*blockSize + blockSize, ...
           (i-1)*blockSize + 1:(i-1)*blockSize + blockSize) = dispVectorX;
        dy((j-1)*blockSize + 1:(j-1)*blockSize + blockSize, ...
           (i-1)*blockSize + 1:(i-1)*blockSize + blockSize) = dispVectorY;
    end
end
    
end

