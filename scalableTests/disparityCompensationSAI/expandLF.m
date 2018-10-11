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
           
% 0 - don't generate SAI
% 9 - causal reference SAI
% 8 - causal NON reference SAI (same layer)
% 1 - horizontal     '-'
% 2 - vertical       '|'
% 3 - diagonal left  '\'
% 4 - diagonal right '/'

expandToLF7 = [ 0 0 0 0 0 0 0 ;
                0 9 1 9 1 9 0 ;
                0 2 3 2 4 2 0 ;
                0 9 1 9 1 9 0 ;
                0 2 4 2 3 2 0 ;
                0 9 1 9 1 9 0 ;
                0 0 0 0 0 0 0 ];
            
expandToLF13 = [ 0 0 9 1 9 1 9 1 9 1 9 0 0 ;
                 0 0 2 3 2 3 2 4 2 4 2 0 0 ;
                 9 1 9 1 9 1 9 1 9 1 9 1 9 ;
                 2 3 2 3 2 3 2 4 2 4 2 4 2 ;
                 9 1 9 1 9 1 9 1 9 1 9 1 9 ;
                 2 3 2 3 2 3 2 4 2 4 2 4 2 ;
                 9 1 9 1 9 1 9 1 9 1 9 1 9 ;
                 2 4 2 4 2 4 2 3 2 3 2 3 2 ;
                 9 1 9 1 9 1 9 1 9 1 9 1 9 ;
                 2 4 2 4 2 4 2 3 2 3 2 3 2 ;
                 9 1 9 1 9 1 9 1 9 1 9 1 9 ;
                 0 0 2 4 2 4 2 3 2 3 2 0 0 ;
                 0 0 9 1 9 1 9 1 9 1 9 0 0 ];
           
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
fileID_L34 = fopen('LF_Buff_L34.yuv', 'w' );
fileID_L56 = fopen('LF_Buff_L56.yuv', 'w' );
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
                PVS(frameNum + 1, :, :, 1) = uint16(Y(1:H_PVS,1:W_PVS));
                PVS(frameNum + 1, :, :, 2) = uint16(U(1:H_PVS,1:W_PVS));
                PVS(frameNum + 1, :, :, 3) = uint16(V(1:H_PVS,1:W_PVS));
                % gen LFs for the different layers
                LF(ypos,xpos,:,:,1) = uint16(Y(1:H_PVS_wo_padding,1:W_PVS_wo_padding));
                LF(ypos,xpos,:,:,2) = uint16(U(1:H_PVS_wo_padding,1:W_PVS_wo_padding));
                LF(ypos,xpos,:,:,3) = uint16(V(1:H_PVS_wo_padding,1:W_PVS_wo_padding));
                if l <= 2
                    LF_L12((ypos+1)/4,(xpos+1)/4,:,:,1) = uint16(Y(1:H_PVS_wo_padding,1:W_PVS_wo_padding));
                    LF_L12((ypos+1)/4,(xpos+1)/4,:,:,2) = uint16(U(1:H_PVS_wo_padding,1:W_PVS_wo_padding));
                    LF_L12((ypos+1)/4,(xpos+1)/4,:,:,3) = uint16(V(1:H_PVS_wo_padding,1:W_PVS_wo_padding));
                end
                if l <= 4
                    LF_L34((ypos+1)/2,(xpos+1)/2,:,:,1) = uint16(Y(1:H_PVS_wo_padding,1:W_PVS_wo_padding));
                    LF_L34((ypos+1)/2,(xpos+1)/2,:,:,2) = uint16(U(1:H_PVS_wo_padding,1:W_PVS_wo_padding));
                    LF_L34((ypos+1)/2,(xpos+1)/2,:,:,3) = uint16(V(1:H_PVS_wo_padding,1:W_PVS_wo_padding));
                end
                if l <= 6
                    LF_L56(ypos+1,xpos+1,:,:,1) = uint16(Y(1:H_PVS_wo_padding,1:W_PVS_wo_padding));
                    LF_L56(ypos+1,xpos+1,:,:,2) = uint16(U(1:H_PVS_wo_padding,1:W_PVS_wo_padding));
                    LF_L56(ypos+1,xpos+1,:,:,3) = uint16(V(1:H_PVS_wo_padding,1:W_PVS_wo_padding));
                end
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
                % starting from layer 3 because it has discontinuities 
                if l >= 3
                    if l == 3 || l == 4 % 3x3 LF to 7x7 LF
                        expandToLF7((ypos+1)/2,(xpos+1)/2) = 8;
                        for iSAI = 10:45 
                            [yposNonCausal, xposNonCausal] = find(scl_spiral == iSAI);
                            if expandToLF7((yposNonCausal+1)/2,(xposNonCausal+1)/2) > 0 && expandToLF7((yposNonCausal+1)/2,(xposNonCausal+1)/2) < 8
                                if expandToLF7((yposNonCausal+1)/2,(xposNonCausal+1)/2) == 1 % horizontal
                                    referenceSAI0 = scl_spiral(yposNonCausal, xposNonCausal -2); % L
                                    referenceSAI1 = scl_spiral(yposNonCausal, xposNonCausal +2);  % R
                                    dispCompensationSearchWindow = [-winDispComp(2) -winDispComp(1); winDispComp(2) 0];
                                end
                                if expandToLF7((yposNonCausal+1)/2,(xposNonCausal+1)/2) == 2 % vertical
                                    referenceSAI0 = scl_spiral(yposNonCausal -2, xposNonCausal); % U
                                    referenceSAI1 = scl_spiral(yposNonCausal +2, xposNonCausal);  % D
                                    dispCompensationSearchWindow = [-winDispComp(1) -winDispComp(2); 0 winDispComp(2)];
                                end
                                if expandToLF7((yposNonCausal+1)/2,(xposNonCausal+1)/2) == 3 % diagonal L \
                                    referenceSAI0 = scl_spiral(yposNonCausal -2, xposNonCausal -2); % UL
                                    referenceSAI1 = scl_spiral(yposNonCausal +2, xposNonCausal +2);   % DR
                                    dispCompensationSearchWindow = [-winDispComp(1) -winDispComp(1); 0 0];
                                end
                                if expandToLF7((yposNonCausal+1)/2,(xposNonCausal+1)/2) == 4 % diagonal R /
                                    referenceSAI0 = scl_spiral(yposNonCausal -2, xposNonCausal +2); % UR
                                    referenceSAI1 = scl_spiral(yposNonCausal +2, xposNonCausal -2);   % DL
                                    dispCompensationSearchWindow = [-winDispComp(1) 0; 0 winDispComp(1)];
                                end
                                [ dx dy ] = disparityCompensationSAI_BM( squeeze(PVS(referenceSAI0+1,:,:,1)), squeeze(PVS(referenceSAI1+1,:,:,1)), dispCompensationSearchWindow, 8);
                                compensatedSAIY = compensateDisparitySAI( squeeze(PVS(referenceSAI0+1,:,:,1)), round(dx/2), round(dy/2) );
                                compensatedSAIU = compensateDisparitySAI( squeeze(PVS(referenceSAI0+1,:,:,2)), round(dx/2), round(dy/2) );
                                compensatedSAIV = compensateDisparitySAI( squeeze(PVS(referenceSAI0+1,:,:,3)), round(dx/2), round(dy/2) );
                                for y = 1:H_PVS_wo_padding
                                    for x = 1:W_PVS_wo_padding
                                        LF_Buff_L34((yposNonCausal+1)/2 + (y-1)*miSizeL34, (xposNonCausal+1)/2 + (x-1)*miSizeL34, 1) = uint16(compensatedSAIY(y,x));
                                        LF_Buff_L34((yposNonCausal+1)/2 + (y-1)*miSizeL34, (xposNonCausal+1)/2 + (x-1)*miSizeL34, 2) = uint16(compensatedSAIU(y,x));
                                        LF_Buff_L34((yposNonCausal+1)/2 + (y-1)*miSizeL34, (xposNonCausal+1)/2 + (x-1)*miSizeL34, 3) = uint16(compensatedSAIV(y,x));
                                    end
                                end
                                %imshow(LF_Buff_L34(1000:1500,1000:1500,1),[]);
                                %imshow(compensatedSAI,[])
                            end
                        end
                    end
                     
                    if l == 5 || l == 6 % 7x7 LF to 13x13 LF
                        expandToLF13(ypos,xpos) = 8;
                        for iSAI = 46:169 
                            [yposNonCausal, xposNonCausal] = find(scl_spiral == iSAI);
                            if expandToLF13(yposNonCausal,xposNonCausal) > 0 && expandToLF13(yposNonCausal,xposNonCausal) < 8
                                if expandToLF13(yposNonCausal,xposNonCausal) == 1 % horizontal
                                    referenceSAI0 = scl_spiral(yposNonCausal, xposNonCausal -1); % L
                                    referenceSAI1 = scl_spiral(yposNonCausal, xposNonCausal +1);  % R
                                    dispCompensationSearchWindow = [-winDispComp(2) -winDispComp(1); winDispComp(2) 0];
                                end
                                if expandToLF13(yposNonCausal,xposNonCausal) == 2 % vertical
                                    referenceSAI0 = scl_spiral(yposNonCausal -1, xposNonCausal); % U
                                    referenceSAI1 = scl_spiral(yposNonCausal +1, xposNonCausal);  % D
                                    dispCompensationSearchWindow = [-winDispComp(1) -winDispComp(2); 0 winDispComp(2)];
                                end
                                if expandToLF13(yposNonCausal,xposNonCausal) == 3 % diagonal L \
                                    referenceSAI0 = scl_spiral(yposNonCausal -1, xposNonCausal -1); % UL
                                    referenceSAI1 = scl_spiral(yposNonCausal +1, xposNonCausal +1);   % DR
                                    dispCompensationSearchWindow = [-winDispComp(1) -winDispComp(1); 0 0];
                                end
                                if expandToLF13(yposNonCausal,xposNonCausal) == 4 % diagonal R /
                                    referenceSAI0 = scl_spiral(yposNonCausal -1, xposNonCausal +1); % UR
                                    referenceSAI1 = scl_spiral(yposNonCausal +1, xposNonCausal -1);   % DL
                                    dispCompensationSearchWindow = [-winDispComp(1) 0; 0 winDispComp(1)];
                                end
                                [ dx dy ] = disparityCompensationSAI_BM( squeeze(PVS(referenceSAI0+1,:,:,1)), squeeze(PVS(referenceSAI1+1,:,:,1)), dispCompensationSearchWindow, 8);
                                compensatedSAIY = compensateDisparitySAI( squeeze(PVS(referenceSAI0+1,:,:,1)), round(dx/2), round(dy/2) );
                                compensatedSAIU = compensateDisparitySAI( squeeze(PVS(referenceSAI0+1,:,:,2)), round(dx/2), round(dy/2) );
                                compensatedSAIV = compensateDisparitySAI( squeeze(PVS(referenceSAI0+1,:,:,3)), round(dx/2), round(dy/2) );
                                for y = 1:H_PVS_wo_padding
                                    for x = 1:W_PVS_wo_padding
                                        LF_Buff_L56(yposNonCausal + (y-1)*miSizeL56, xposNonCausal + (x-1)*miSizeL56, 1) = uint16(compensatedSAIY(y,x));
                                        LF_Buff_L56(yposNonCausal + (y-1)*miSizeL56, xposNonCausal + (x-1)*miSizeL56, 2) = uint16(compensatedSAIU(y,x));
                                        LF_Buff_L56(yposNonCausal + (y-1)*miSizeL56, xposNonCausal + (x-1)*miSizeL56, 3) = uint16(compensatedSAIV(y,x));
                                    end
                                end
                                %imshow(LF_Buff_L34(1000:1500,1000:1500,1),[]);
                                %imshow(compensatedSAI,[])
                            end
                        end
                    end
                end
                fwrite(fileID_L34, permute(LF_Buff_L34, [2 1 3]), 'uint16');
                fwrite(fileID_L56, permute(LF_Buff_L56, [2 1 3]), 'uint16');
                SAIUsage(ypos,xpos) = frameNum;
                frameNum = frameNum + 1
                %close all
            end
        end
    end
    
end

fclose(fileID_L34);
fclose(fileID_L56);







