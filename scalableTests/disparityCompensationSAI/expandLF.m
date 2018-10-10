clear all
close all
LF_Image_PVS_SCL = '/home/rmonteiro/PhD/Sequences/EPFL/4DLF_PVS_Scalable/I01_Bikes__Decoded_13x13_YUV444_10bpp.yuv';
%LF_Image_PVS_SCL = '/home/rmonteiro/PhD/Scripts/HEVC_tests/HEVC_EPFL_4DLF_PVS_SCL_B_YUV444_10/4DLF_13x13_PVS_SCL_I01_YUV444_10/32/rec.yuv';

num_Layers = 6;
layerMask =  [ 6 6 4 6 3 6 4 6 3 6 4 6 6 ;
               6 5 6 5 6 5 6 5 6 5 6 5 6 ;
               4 6 2 6 4 6 2 6 4 6 2 6 4 ;
               6 5 6 5 6 5 6 5 6 5 6 5 6 ;
               3 6 4 6 3 6 4 6 3 6 4 6 3 ;
               6 5 6 5 6 5 6 5 6 5 6 5 6 ;
               4 6 2 6 4 6 1 6 4 6 2 6 4 ;
               6 5 6 5 6 5 6 5 6 5 6 5 6 ;
               3 6 4 6 3 6 4 6 3 6 4 6 3 ;
               6 5 6 5 6 5 6 5 6 5 6 5 6 ;
               4 6 2 6 4 6 2 6 4 6 2 6 4 ;
               6 5 6 5 6 5 6 5 6 5 6 5 6 ;
               6 6 4 6 3 6 4 6 3 6 4 6 6 ];
           
expandToLF7X =  [ 0 0 0 0 0 0 0 ;
                  0 0 1 0 1 0 0 ;
                  0 2 3 2 4 2 0 ;
                  0 0 1 0 1 0 0 ;
                  0 2 4 2 3 2 0 ;
                  0 0 1 0 1 0 0 ;
                  0 0 0 0 0 0 0 ];
           
W_BUF = 8125; % buffer res
H_BUF = 5642; % 13 * 434

W_PVS = 632; % SAI res
H_PVS = 440;

W_PVS_wo_padding = 625;
H_PVS_wo_padding = 434;

miSizeL12 = 3;
miSizeL34 = 7;
miSizeL56 = 13;
miSize = 13; 
numSAIs = miSize * miSize;

PVS =           zeros(miSize * miSize, H_PVS, W_PVS, 3);

LF =            zeros(miSize,       miSize,     H_PVS_wo_padding, W_PVS_wo_padding, 4);
LF_L12 =        zeros(miSizeL12,    miSizeL12,  H_PVS_wo_padding, W_PVS_wo_padding, 4);
LF_L34 =        zeros(miSizeL34,    miSizeL34,  H_PVS_wo_padding, W_PVS_wo_padding, 4);
LF_L56 =        zeros(miSizeL56,    miSizeL56,  H_PVS_wo_padding, W_PVS_wo_padding, 4);

LF_Buff =       zeros(miSize * H_PVS_wo_padding,    miSize * W_PVS_wo_padding, 3);
LF_Buff_L12 =   zeros(miSizeL12 * H_PVS_wo_padding, miSizeL12 * W_PVS_wo_padding, 3);
LF_Buff_L34 =   zeros(miSizeL34 * H_PVS_wo_padding, miSizeL34 * W_PVS_wo_padding, 3);
LF_Buff_L56 =   zeros(miSizeL56 * H_PVS_wo_padding, miSizeL56 * W_PVS_wo_padding, 3);

SAIUsage =      zeros(miSize, miSize);
SAIDistance =   zeros(miSize, miSize);

PVS(:,:,:,:) = -1;

LF(:,:,:,:,:) = -1;
LF_L12(:,:,:,:,:) = -1;
LF_L34(:,:,:,:,:) = -1;
LF_L56(:,:,:,:,:) = -1;

LF_Buff(:,:,:) = -1;
LF_Buff_L12(:,:,:) = -1;
LF_Buff_L34(:,:,:) = -1;
LF_Buff_L56(:,:,:) = -1;

SAIUsage(:,:) = -1;
SAIDistance(:,:) = -1;

f = fopen(LF_Image_PVS_SCL,'r');
frameNum = 0;
cc_spiral = spiral(miSize);
for l = 1:num_Layers
    for j = 1:miSize
        for i = 1:miSize 
            [ypos, xpos] = find(cc_spiral == (j-1)*miSize + i);
            if layerMask(ypos,xpos) == l
                Y = fread(f, [W_PVS H_PVS], 'uint16');
                U = fread(f, [W_PVS H_PVS], 'uint16');
                V = fread(f, [W_PVS H_PVS], 'uint16');
                Y = Y'; U = U'; V = V';
                PVS(frameNum + 1, :, :, 1) = uint16(Y(1:H_PVS,1:W_PVS));
                PVS(frameNum + 1, :, :, 2) = uint16(U(1:H_PVS,1:W_PVS));
                PVS(frameNum + 1, :, :, 3) = uint16(V(1:H_PVS,1:W_PVS));
                % gen LFs for the different layers
                LF(ypos,xpos,:,:,1) = uint16(Y(1:H_PVS_wo_padding,1:W_PVS_wo_padding));
                LF(ypos,xpos,:,:,2) = uint16(U(1:H_PVS_wo_padding,1:W_PVS_wo_padding));
                LF(ypos,xpos,:,:,3) = uint16(V(1:H_PVS_wo_padding,1:W_PVS_wo_padding));
                LF_L12((ypos+1)/4,(xpos+1)/4,:,:,1) = uint16(Y(1:H_PVS_wo_padding,1:W_PVS_wo_padding));
                LF_L12((ypos+1)/4,(xpos+1)/4,:,:,2) = uint16(U(1:H_PVS_wo_padding,1:W_PVS_wo_padding));
                LF_L12((ypos+1)/4,(xpos+1)/4,:,:,3) = uint16(V(1:H_PVS_wo_padding,1:W_PVS_wo_padding));
                LF_L34((ypos+1)/2,(xpos+1)/2,:,:,1) = uint16(Y(1:H_PVS_wo_padding,1:W_PVS_wo_padding));
                LF_L34((ypos+1)/2,(xpos+1)/2,:,:,2) = uint16(U(1:H_PVS_wo_padding,1:W_PVS_wo_padding));
                LF_L34((ypos+1)/2,(xpos+1)/2,:,:,3) = uint16(V(1:H_PVS_wo_padding,1:W_PVS_wo_padding));
                LF_L56(ypos+1,xpos+1,:,:,1) = uint16(Y(1:H_PVS_wo_padding,1:W_PVS_wo_padding));
                LF_L56(ypos+1,xpos+1,:,:,2) = uint16(U(1:H_PVS_wo_padding,1:W_PVS_wo_padding));
                LF_L56(ypos+1,xpos+1,:,:,3) = uint16(V(1:H_PVS_wo_padding,1:W_PVS_wo_padding));
                % generate LF buffers for the different layers 
                for y = 1:H_PVS_wo_padding
                    for x = 1:W_PVS_wo_padding
                        if l <= 2
                            LF_Buff_L12((ypos+1)/4 + (y-1)*miSizeL12, (xpos+1)/4 + (x-1)*miSizeL12, 1) = uint16(Y(y,x));
                            LF_Buff_L12((ypos+1)/4 + (y-1)*miSizeL12, (xpos+1)/4 + (x-1)*miSizeL12, 2) = uint16(U(y,x));
                            LF_Buff_L12((ypos+1)/4 + (y-1)*miSizeL12, (xpos+1)/4 + (x-1)*miSizeL12, 3) = uint16(V(y,x));
                        end
                        if l <= 4
                            LF_Buff_L34((ypos+1)/2 + (y-1)*miSizeL34, (xpos+1)/2 + (x-1)*miSizeL34, 1) = uint16(Y(y,x));
                            LF_Buff_L34((ypos+1)/2 + (y-1)*miSizeL34, (xpos+1)/2 + (x-1)*miSizeL34, 2) = uint16(U(y,x));
                            LF_Buff_L34((ypos+1)/2 + (y-1)*miSizeL34, (xpos+1)/2 + (x-1)*miSizeL34, 3) = uint16(V(y,x));
                        end
                        if l <= 6
                            LF_Buff_L56(ypos + (y-1)*miSizeL56, xpos + (x-1)*miSizeL56, 1) = uint16(Y(y,x));
                            LF_Buff_L56(ypos + (y-1)*miSizeL56, xpos + (x-1)*miSizeL56, 2) = uint16(U(y,x));
                            LF_Buff_L56(ypos + (y-1)*miSizeL56, xpos + (x-1)*miSizeL56, 3) = uint16(V(y,x));
                        end
                    end
                end
                
                % apply disparity compensation to the current 
                % calculate euclidean distance to current SAI
                lowestDistanceSAIX = 7;
                lowestDistanceSAIY = 7;
                bestSAIDistance = Inf;
                SAIDistance = Inf;
                referenceSAI = 0;
                for y = 1:miSize
                    for x = 1:miSize
                        if SAIUsage(y,x) == -1
                            SAIDistance(y,x) = Inf;
                        else
                            SAIDistance(y,x) = sqrt((y-ypos)^2+(x-xpos)^2);
                        end
                    end
                end
                % select reference -> lowest distance and lowest frame
                %                     number i.e. upper layers
                for frame = 0:miSize*miSize-1
                    for y = 1:miSize
                        for x = 1:miSize
                            if SAIUsage(y,x) == frame
                                if  SAIDistance(y,x) < bestSAIDistance
                                    referenceSAI = frame;
                                    bestSAIDistance = SAIDistance(y,x);
                                end
                            end
                        end
                    end
                end %                                    currSAI - fixed template      | refSAI - test several disparity vectors
                [ dx dy ] = disparityCompensationSAI_TM( squeeze(PVS(frameNum+1,:,:,1)), squeeze(PVS(referenceSAI+1,:,:,1)), [8 8], 8, 4);
                %referenceSAI
                %frameNum
                %figure, imshow(dx,[]);
                %figure, imshow(dy,[]);
                
                
                    
                   
                
                SAIUsage(ypos,xpos) = frameNum;
                frameNum = frameNum + 1;
                %close all
            end
        end
    end
end







