clear all
close all
LF_Image_PVS_SCL = '/home/rmonteiro/PhD/Sequences/EPFL/4DLF_PVS_Scalable/I01_Bikes__Decoded_13x13_YUV444_10bpp.yuv';
LF_Image_PVS_SCL_Buff34 = 'BM_Block8_WindAssym_8_1_LF_Buff_L34.yuv';
LF_Image_PVS_SCL_Buff56 = 'BM_Block8_WindAssym_8_1_LF_Buff_L56.yuv';

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
%% search window favorable and unfavorable axis
winDispComp = [8 1];

miSizeL12 = 3;
miSizeL34 = 7;
miSizeL56 = 13;
miSize = 13; 
numSAIs = miSize * miSize;

MAX = 1023 * 1023;

PVS =           zeros(miSize * miSize, H_PVS_wo_padding, W_PVS_wo_padding, 3);
PVS_REC =       zeros(H_PVS_wo_padding, W_PVS_wo_padding, 3);

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
PVS_REC(:,:,:) = -1;

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
fBuff34 = fopen(LF_Image_PVS_SCL_Buff34,'r');
fBuff56 = fopen(LF_Image_PVS_SCL_Buff56,'r');
fResults = fopen('results.txt','w');

frameNum = 0;
cc_spiral = spiral(miSize);
scl_spiral = zeros(miSize,miSize);
for l = 1:num_Layers
    for j = 1:miSize
        for i = 1:miSize
            [ypos, xpos] = find(cc_spiral == (j-1)*miSize + i);
            if layerMask(ypos,xpos) == l
                scl_spiral(ypos,xpos) = frameNum + 1;
                frameNum = frameNum + 1;
            end
        end
    end
end 

frameNum = 0;
for l = 1:num_Layers
    for j = 1:miSize
        for i = 1:miSize
            [ypos, xpos] = find(cc_spiral == (j-1)*miSize + i);
            if layerMask(ypos,xpos) == l
                Y = fread(f, [W_PVS H_PVS], 'uint16');
                U = fread(f, [W_PVS H_PVS], 'uint16');
                V = fread(f, [W_PVS H_PVS], 'uint16');
                Y = Y'; U = U'; V = V';
                PVS(frameNum + 1, :, :, 1) = uint16(Y(1:H_PVS_wo_padding,1:W_PVS_wo_padding));
                PVS(frameNum + 1, :, :, 2) = uint16(U(1:H_PVS_wo_padding,1:W_PVS_wo_padding));
                PVS(frameNum + 1, :, :, 3) = uint16(V(1:H_PVS_wo_padding,1:W_PVS_wo_padding));
                frameNum = frameNum + 1;
            end
        end
    end
end
                
frameNum = 0;
for l = 1:num_Layers
    for j = 1:miSize
        for i = 1:miSize 
            [ypos, xpos] = find(cc_spiral == (j-1)*miSize + i);
            if layerMask(ypos,xpos) == l
                if l == 1 || l == 2
                    YBuff34 = fread(fBuff34, [miSizeL34 * W_PVS_wo_padding miSizeL34 * H_PVS_wo_padding], 'uint16');
                    UBuff34 = fread(fBuff34, [miSizeL34 * W_PVS_wo_padding miSizeL34 * H_PVS_wo_padding], 'uint16');
                    VBuff34 = fread(fBuff34, [miSizeL34 * W_PVS_wo_padding miSizeL34 * H_PVS_wo_padding], 'uint16');
                    YBuff56 = fread(fBuff56, [miSizeL56 * W_PVS_wo_padding miSizeL56 * H_PVS_wo_padding], 'uint16');
                    UBuff56 = fread(fBuff56, [miSizeL56 * W_PVS_wo_padding miSizeL56 * H_PVS_wo_padding], 'uint16');
                    VBuff56 = fread(fBuff56, [miSizeL56 * W_PVS_wo_padding miSizeL56 * H_PVS_wo_padding], 'uint16');
                end
                if l >= 3
                    if l == 3 || l == 4 % 3x3 LF to 7x7 LF
                        YBuff34 = fread(fBuff34, [miSizeL34 * W_PVS_wo_padding miSizeL34 * H_PVS_wo_padding], 'uint16');
                        UBuff34 = fread(fBuff34, [miSizeL34 * W_PVS_wo_padding miSizeL34 * H_PVS_wo_padding], 'uint16');
                        VBuff34 = fread(fBuff34, [miSizeL34 * W_PVS_wo_padding miSizeL34 * H_PVS_wo_padding], 'uint16');
                        YBuff34 = YBuff34'; UBuff34 = UBuff34'; VBuff34 = VBuff34';
                        YBuff56 = fread(fBuff56, [miSizeL56 * W_PVS_wo_padding miSizeL56 * H_PVS_wo_padding], 'uint16');
                        UBuff56 = fread(fBuff56, [miSizeL56 * W_PVS_wo_padding miSizeL56 * H_PVS_wo_padding], 'uint16');
                        VBuff56 = fread(fBuff56, [miSizeL56 * W_PVS_wo_padding miSizeL56 * H_PVS_wo_padding], 'uint16');
                        for iSAI = 10:45
                            [yposNonCausal, xposNonCausal] = find(scl_spiral == iSAI);
                            for y = 1:H_PVS_wo_padding
                                for x = 1:W_PVS_wo_padding
                                    PVS_REC(y,x,1) = YBuff34((yposNonCausal+1)/2 + (y-1)*miSizeL34, (xposNonCausal+1)/2 + (x-1)*miSizeL34);
                                    PVS_REC(y,x,2) = UBuff34((yposNonCausal+1)/2 + (y-1)*miSizeL34, (xposNonCausal+1)/2 + (x-1)*miSizeL34);
                                    PVS_REC(y,x,3) = VBuff34((yposNonCausal+1)/2 + (y-1)*miSizeL34, (xposNonCausal+1)/2 + (x-1)*miSizeL34);
                                end
                            end
                            ds = (squeeze(PVS(iSAI,:,:,1)) - squeeze(PVS_REC(:,:,1))).^2;
                            MSE_Y = mean(ds(:));
                            PSNR_Y = 10*log10(MAX/MSE_Y);
                            fprintf(fResults,'%f\t%f\t%f\n',frameNum+1,iSAI,PSNR_Y);
                        end
                    end
                    if l == 5 || l == 6 % 7x7 LF to 13x13 LF
                        YBuff56 = fread(fBuff56, [miSizeL56 * W_PVS_wo_padding miSizeL56 * H_PVS_wo_padding], 'uint16');
                        UBuff56 = fread(fBuff56, [miSizeL56 * W_PVS_wo_padding miSizeL56 * H_PVS_wo_padding], 'uint16');
                        VBuff56 = fread(fBuff56, [miSizeL56 * W_PVS_wo_padding miSizeL56 * H_PVS_wo_padding], 'uint16');
                        YBuff56 = YBuff56'; UBuff56 = UBuff56'; VBuff56 = VBuff56';
                        for iSAI = 46:169
                            [yposNonCausal, xposNonCausal] = find(scl_spiral == iSAI);
                            for y = 1:H_PVS_wo_padding
                                for x = 1:W_PVS_wo_padding
                                    PVS_REC(y,x,1) = YBuff56(yposNonCausal + (y-1)*miSizeL56, xposNonCausal + (x-1)*miSizeL56);
                                    PVS_REC(y,x,2) = UBuff56(yposNonCausal + (y-1)*miSizeL56, xposNonCausal + (x-1)*miSizeL56);
                                    PVS_REC(y,x,3) = VBuff56(yposNonCausal + (y-1)*miSizeL56, xposNonCausal + (x-1)*miSizeL56);
                                end
                            end
                            ds = (squeeze(PVS(iSAI,:,:,1)) - squeeze(PVS_REC(:,:,1))).^2;
                            MSE_Y = mean(ds(:));
                            PSNR_Y = 10*log10(MAX/MSE_Y);
                            fprintf(fResults,'%f\t%f\t%f\n',frameNum+1,iSAI,PSNR_Y);
                        end
                    end
                end
                frameNum = frameNum + 1
            end
        end
    end
end

