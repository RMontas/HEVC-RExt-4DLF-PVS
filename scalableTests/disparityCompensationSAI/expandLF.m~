clear all
LF_Image_PVS_SCL = '/home/rmonteiro/PhD/Sequences/EPFL/4DLF_PVS_Scalable/I01_Bikes__Decoded_13x13_YUV444_10bpp.yuv';

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

PVS(:,:,:,:) = -1;

LF(:,:,:,:,:) = -1;
LF_L12(:,:,:,:,:) = -1;
LF_L34(:,:,:,:,:) = -1;
LF_L56(:,:,:,:,:) = -1;

LF_Buff(:,:,:) = -1;
LF_Buff_L12(:,:,:) = -1;
LF_Buff_L34(:,:,:) = -1;
LF_Buff_L56(:,:,:) = -1;

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
                LF(ypos,xpos,:,:,1) = uint16(Y(1:H_PVS_wo_padding,1:W_PVS_wo_padding));
                LF(ypos,xpos,:,:,2) = uint16(U(1:H_PVS_wo_padding,1:W_PVS_wo_padding));
                LF(ypos,xpos,:,:,3) = uint16(V(1:H_PVS_wo_padding,1:W_PVS_wo_padding));
                LF_Buff = reconstruct_lenslet_img10_anyMISize(LF, miSize);
                frameNum = frameNum + 1
            end
          
            if l == 3
                if ypos == 8 && xpos == 8 % frame 9 % use frame 0 and 1
                    [ dx8, dy8 ] = disparityCompensationSAI_BM( squeeze(PVS(1,:,:,1)), squeeze(PVS(2,:,:,1)), [1 8], 8 );
                    figure, imshow(dx8,[])
                    figure, imshow(dy8,[])
                    figure, histogram(dx8)
                    figure, histogram(dy8)
                end
            end
            
            
        end
    end
end







