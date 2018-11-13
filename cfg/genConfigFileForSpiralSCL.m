function [] = genConfigFileForSpiralSCL(max_ref_pics, SCL_FLAG)

%max_ref_pics = 4;
num_MIs = 13;
if SCL_FLAG == 1
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
else
num_Layers = 6;
layerMask =  [  2 2 2 2 2 2 2 2 2 2 2 2 2 ;
		2 2 2 2 2 2 2 2 2 2 2 2 2 ;
		2 2 2 2 2 2 2 2 2 2 2 2 2 ;
		2 2 2 2 2 2 2 2 2 2 2 2 2 ;
		2 2 2 2 2 2 2 2 2 2 2 2 2 ;
		2 2 2 2 2 2 2 2 2 2 2 2 2 ;
		2 2 2 2 2 2 1 2 2 2 2 2 2 ;
		2 2 2 2 2 2 2 2 2 2 2 2 2 ;
		2 2 2 2 2 2 2 2 2 2 2 2 2 ;
		2 2 2 2 2 2 2 2 2 2 2 2 2 ;
		2 2 2 2 2 2 2 2 2 2 2 2 2 ;
		2 2 2 2 2 2 2 2 2 2 2 2 2 ;
		2 2 2 2 2 2 2 2 2 2 2 2 2 ];
end
SAIUsage = zeros(num_MIs, num_MIs);
SAIUsage(floor(num_MIs/2)+1,floor(num_MIs/2)+1) = -1;      
cc_spiral = spiral(num_MIs); 
cc_spiral(:,:) = cc_spiral(:,:) -1;
QPOffset = [3 2 3 1];
QPFactor = [0.4624 0.4624 0.4624 0.5780];
refPicsActive = 1;
qp = 1;
POC = 1;
for l = 2:num_Layers % starts with 2 because Frame 1 is on layer 2
    for j = 1:(num_MIs * num_MIs)-1
        [ypos, xpos] = find(cc_spiral == j);
        if layerMask(ypos,xpos) == l
            fprintf('Frame%d:\tB\t%d\t%d\t0\t0\t%f\t0\t0\t0\t%d\t%d\t',POC,POC,QPOffset(qp),QPFactor(qp),refPicsActive,POC);
            refSAIs = getRefSAIs(POC, [ypos xpos], SAIUsage, max_ref_pics, cc_spiral, layerMask);
            for i = 1:size(refSAIs); fprintf('%d ',refSAIs(i)); end
            fprintf('\t0\n');
            SAIUsage(ypos,xpos) = POC;
            qp = qp + 1;
            if qp > 4; qp = 1; end
            POC = POC + 1;
            if refPicsActive < max_ref_pics; refPicsActive = refPicsActive + 1; end
        end
    end
end
end

function [refSAIs] = getRefSAIs(currentSAI, currentSAIPos, SAIUsage, max_ref_pics, cc_spiral, layerMask)
% criteria 1 : lowest euclidean distance
% criteria 2 : lowest POC

[num_MIs] = size(SAIUsage);
num_MIs = num_MIs(1);
SAIREFUsage = zeros(num_MIs, num_MIs);
refSAIs = zeros(currentSAI,1);
distanceL2toCurrentSAI = zeros(num_MIs,num_MIs);
distanceL2toCurrentSAI(:,:) = Inf;
num_Layers = max(max(layerMask));
% calculate euclidean distance
currentYPos = currentSAIPos(1);
currentXPos = currentSAIPos(2);
for l = 1:num_Layers
    for j = 0:(num_MIs * num_MIs)-1
        [ypos, xpos] = find(cc_spiral == j);
        if layerMask(ypos,xpos) == l
            if SAIUsage(ypos,xpos) ~= 0
                distanceL2toCurrentSAI(ypos,xpos) = sqrt((ypos-currentYPos).^2 + (xpos-currentXPos).^2);
            end
        end
    end
end
distanceL2toCurrentSAI(distanceL2toCurrentSAI(:,:) == 0) = Inf; % overwrite currentSAI distance

for i = 1:currentSAI
    lowestDistance = Inf;
    bestYPos = 0; bestXPos = 0;
    for l = 1:num_Layers
        for j = 0:(num_MIs * num_MIs)-1
            [ypos, xpos] = find(cc_spiral == j);
            if layerMask(ypos,xpos) == l
                if SAIUsage(ypos,xpos) ~= 0 && distanceL2toCurrentSAI(ypos,xpos) < lowestDistance && SAIREFUsage(ypos,xpos) == 0 
                    lowestDistance = distanceL2toCurrentSAI(ypos,xpos);
                    bestYPos = ypos; bestXPos = xpos;
                end
            end
        end
    end
    % find ref POC in scalable spiral
    refPOC = 0;
    currentPOC = 0;
    for L = 1:num_Layers
        for J = 0:(num_MIs * num_MIs)-1
            [ypos, xpos] = find(cc_spiral == J);
            if layerMask(ypos,xpos) == L
                if ypos == bestYPos && xpos == bestXPos
                    refPOC = currentPOC;
                end
                currentPOC = currentPOC + 1;
            end
        end
    end
    refSAIs(i) = refPOC - currentSAI;
    SAIREFUsage(bestYPos, bestXPos) = 1;
end

end











