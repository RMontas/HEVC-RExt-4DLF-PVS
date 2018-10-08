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

miSize = 13;
numSAIs = miSize * miSize;

LF = zeros(miSize, miSize, H_PVS_wo_padding, W_PVS_wo_padding, 4);
LF_Buff = zeros(miSize * H_PVS_wo_padding, miSize * W_PVS_wo_padding, 3);
LF_L12 = zeros(3, 3, H_PVS_wo_padding, W_PVS_wo_padding, 4);
LF_Buff_L12 = zeros(3 * H_PVS_wo_padding, 3 * W_PVS_wo_padding, 3);
LF_L34 = zeros(7, 7, H_PVS_wo_padding, W_PVS_wo_padding, 4);
LF_Buff_L34 = zeros(7 * H_PVS_wo_padding, 7 * W_PVS_wo_padding, 3);
LF_L56 = zeros(miSize, miSize, H_PVS_wo_padding, W_PVS_wo_padding, 4);
LF_Buff_L56 = zeros(miSize * H_PVS_wo_padding, miSize * W_PVS_wo_padding, 3);

LF_L34(:,:,:,:,:) = -1;
LF_Buff_L34(:,:,:,:,:) = -1;
LF_L56(:,:,:,:,:) = -1;
LF_Buff_L56(:,:,:,:,:) = -1;

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
                %LF(ypos,xpos,:,:,1) = uint16(Y(1:H_PVS_wo_padding,1:W_PVS_wo_padding));
                %LF(ypos,xpos,:,:,2) = uint16(U(1:H_PVS_wo_padding,1:W_PVS_wo_padding));
                %LF(ypos,xpos,:,:,3) = uint16(V(1:H_PVS_wo_padding,1:W_PVS_wo_padding));
                %LF_Buff = reconstruct_lenslet_img10_anyMISize(LF, miSize);
                
                if l == 3 || l == 4
                    tmp_LF_Buff_L34 = LF_Buff_L34;
                    for my = 1:H_PVS_wo_padding
                        for mx = 1:W_PVS_wo_padding
                            for py = 1:7
                                for px = 1:7
                                    if LF_Buff_L34(py + (my - 1) * 7, px + (mx - 1) * 7, 1) == -1
                                        if py + (my - 1) * 7 -1 >= 1 && px + (mx - 1) * 7 -1 >= 1 && py + (my - 1) * 7 + 1 <= H_PVS_wo_padding * 7 && px + (mx - 1) * 7 + 1 <= W_PVS_wo_padding * 7
                                            tmp_LF_Buff_L34(py + (my - 1) * 7, px + (mx - 1) * 7) = averageIfExistent( LF_Buff_L34( ...
                                                           (py + (my - 1) * 7)-1:(py + (my - 1) * 7)+1, ... % 3x3 template
                                                           (px + (mx - 1) * 7)-1:(px + (mx - 1) * 7)+1,1));
                                        end
                                    end
                                end
                            end
                           % imshow(tmp_LF_Buff_L34(2000/2:3000/2,4000/2:5000/2,1), [])
                        end
                    end
                    save(strcat('LF_Buff_L34_',num2str(frameNum),'.mat'),'tmp_LF_Buff_L34')
                    %figure, imshow(tmp_LF_Buff_L34(2000/2:3000/2,4000/2:5000/2,1), [])
                end
                
                if l == 5 || l == 6
                    tmp_LF_Buff_L56 = LF_Buff_L56;
                    for my = 1:H_PVS_wo_padding
                        for mx = 1:W_PVS_wo_padding
                            for py = 1:miSize
                                for px = 1:miSize
                                    if LF_Buff_L56(py + (my - 1) * miSize, px + (mx - 1) * miSize, 1) == -1
                                        if py + (my - 1) * miSize -1 >= 1 && px + (mx - 1) * miSize -1 >= 1 && py + (my - 1) * miSize + 1 <= H_PVS_wo_padding * miSize && px + (mx - 1) * miSize + 1 <= W_PVS_wo_padding * miSize
                                            tmp_LF_Buff_L56(py + (my - 1) * miSize, px + (mx - 1) * miSize) = averageIfExistent( LF_Buff_L56( ...
                                                           (py + (my - 1) * miSize)-1:(py + (my - 1) * miSize)+1, ... % 3x3 template
                                                           (px + (mx - 1) * miSize)-1:(px + (mx - 1) * miSize)+1,1));
                                        end
                                    end
                                end
                            end
                           % imshow(tmp_LF_Buff_L34(2000/2:3000/2,4000/2:5000/2,1), [])
                        end
                    end
                    save(strcat('LF_Buff_L56_',num2str(frameNum),'.mat'),'tmp_LF_Buff_L56')
                    %figure, imshow(tmp_LF_Buff_L34(2000/2:3000/2,4000/2:5000/2,1), [])
                end
                
                if l <= 2
                    for y = 1:H_PVS_wo_padding
                        for x = 1:W_PVS_wo_padding
                            LF_Buff_L12((ypos+1)/4 + (y-1)*3, (xpos+1)/4 + (x-1)*3, 1) = uint16(Y(y,x));
                            LF_Buff_L12((ypos+1)/4 + (y-1)*3, (xpos+1)/4 + (x-1)*3, 2) = uint16(U(y,x));
                            LF_Buff_L12((ypos+1)/4 + (y-1)*3, (xpos+1)/4 + (x-1)*3, 3) = uint16(V(y,x));
                            LF_Buff_L34((ypos+1)/2 + (y-1)*7, (xpos+1)/2 + (x-1)*7, 1) = uint16(Y(y,x));
                            LF_Buff_L34((ypos+1)/2 + (y-1)*7, (xpos+1)/2 + (x-1)*7, 2) = uint16(U(y,x));
                            LF_Buff_L34((ypos+1)/2 + (y-1)*7, (xpos+1)/2 + (x-1)*7, 3) = uint16(V(y,x));
                            LF_Buff_L56(ypos + (y-1)*miSize, xpos + (x-1)*miSize, 1) = uint16(Y(y,x));
                            LF_Buff_L56(ypos + (y-1)*miSize, xpos + (x-1)*miSize, 2) = uint16(U(y,x));
                            LF_Buff_L56(ypos + (y-1)*miSize, xpos + (x-1)*miSize, 3) = uint16(V(y,x));
                        end
                    end
                end
                if l > 2 && l <= 4
                    for y = 1:H_PVS_wo_padding
                        for x = 1:W_PVS_wo_padding
                            LF_Buff_L34((ypos+1)/2 + (y-1)*7, (xpos+1)/2 + (x-1)*7, 1) = uint16(Y(y,x));
                            LF_Buff_L34((ypos+1)/2 + (y-1)*7, (xpos+1)/2 + (x-1)*7, 2) = uint16(U(y,x));
                            LF_Buff_L34((ypos+1)/2 + (y-1)*7, (xpos+1)/2 + (x-1)*7, 3) = uint16(V(y,x));
                            LF_Buff_L56(ypos + (y-1)*miSize, xpos + (x-1)*miSize, 1) = uint16(Y(y,x));
                            LF_Buff_L56(ypos + (y-1)*miSize, xpos + (x-1)*miSize, 2) = uint16(U(y,x));
                            LF_Buff_L56(ypos + (y-1)*miSize, xpos + (x-1)*miSize, 3) = uint16(V(y,x));
                        end
                    end
                end
                if l >= 5
                    for y = 1:H_PVS_wo_padding
                        for x = 1:W_PVS_wo_padding
                            LF_Buff_L56(ypos + (y-1)*miSize, xpos + (x-1)*miSize, 1) = uint16(Y(y,x));
                            LF_Buff_L56(ypos + (y-1)*miSize, xpos + (x-1)*miSize, 2) = uint16(U(y,x));
                            LF_Buff_L56(ypos + (y-1)*miSize, xpos + (x-1)*miSize, 3) = uint16(V(y,x));
                        end
                    end
                end
                
                %imshow(LF_Buff_L12(2500/4:2699/4,4000/4:4199/4,1),[])            
               % imshow(LF_Buff_L34(2000/2:3000/2,4000/2:5000/2,1),[])
                %imshow(LF_Buff_L56(2500:2699,4000:4199,1),[])
                
                
                frameNum = frameNum + 1
            end
        end
    end
end
           
           
           
           