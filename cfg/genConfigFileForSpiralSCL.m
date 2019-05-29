function [] = genConfigFileForSpiralSCL(max_ref_pics, SCL_FLAG, RA_FLAG, MAX_REF_LAYER)

num_MIs = 13;
%% SCALABILITY
num_Layers = 6;
if SCL_FLAG == 1 %% SCL
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
else %% NON SCL
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
%% RANDOM ACCESS
randomAccessProfiles

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
            refPicsActive = 0;
            [refSAIs, refPicsActive] = getRefSAIs(POC, [ypos xpos], SAIUsage, max_ref_pics, cc_spiral, layerMask, getCurrentRALayer([ypos xpos], raLayerMask), raLayerMask, MAX_REF_LAYER);
            if refPicsActive > 0
                fprintf('Frame%d:\tB\t%d\t%d\t0\t0\t%f\t0\t0\t0\t%d\t%d\t',POC,POC,QPOffset(qp),QPFactor(qp),refPicsActive,POC);
                for i = 1:size(refSAIs); fprintf('%d ',refSAIs(i)); end
                fprintf('\t0\n');
            else
                fprintf('Frame%d:\tI\t%d\t%d\t0\t0\t%f\t0\t0\t0\t%d\t%d\t',POC,POC,0,QPFactor(qp),refPicsActive,POC);
                for i = 1:size(refSAIs); fprintf('%d ',refSAIs(i)); end
                fprintf('\t0\n');
            end
            SAIUsage(ypos,xpos) = POC;
            qp = qp + 1;
            if qp > 4; qp = 1; end
            POC = POC + 1;
            %if refPicsActive < max_ref_pics; refPicsActive = refPicsActive + 1; end
        end
    end
end
end

function [refSAIs, num_ref_pics] = getRefSAIs(currentSAI, currentSAIPos, SAIUsage, max_ref_pics, cc_spiral, layerMask, currentRALayers, raLayerMask, MAX_REF_LAYER)
% criteria 1 : lowest euclidean distance
% criteria 2 : lowest POC

[num_MIs] = size(SAIUsage);
num_MIs = num_MIs(1);
SAIREFUsage = zeros(num_MIs, num_MIs);
refSAIs = zeros(currentSAI,1);
distanceL2toCurrentSAI = zeros(num_MIs,num_MIs);
distanceL2toCurrentSAI(:,:) = Inf;
num_Layers = max(max(layerMask));
currentYPos = currentSAIPos(1);
currentXPos = currentSAIPos(2);

num_ref_pics = 0;

[~, num_ra_layers_being_used] = size(currentRALayers);

% calculate euclidean distance
for l = 1:num_Layers
    for j = 0:(num_MIs * num_MIs)-1
        [ypos, xpos] = find(cc_spiral == j);
        if layerMask(ypos,xpos) == l
            if SAIUsage(ypos,xpos) ~= 0
                validRefernece = 0;
                for ra = 1:num_ra_layers_being_used
                    if raLayerMask(currentRALayers(ra), ypos, xpos)
                        validRefernece = 1;
                    end
                end
                if validRefernece == 1
                    distanceL2toCurrentSAI(ypos,xpos) = sqrt((ypos-currentYPos).^2 + (xpos-currentXPos).^2);
                else
                    distanceL2toCurrentSAI(ypos,xpos) = Inf;
                end
            end
        end
    end
end
distanceL2toCurrentSAI(distanceL2toCurrentSAI(:,:) == 0) = Inf; % overwrite currentSAI distance
%%
% discard references that are on layers with higher number than MAX_REF_LAYER
% e.g. MAX_REF_LAYER = 4, we can only use ref frames from L1, L2, L3 and L4
for l = 1:num_Layers
    for j = 0:(num_MIs * num_MIs)-1
        [ypos, xpos] = find(cc_spiral == j);
        if layerMask(ypos,xpos) == l
            if SAIUsage(ypos,xpos) ~= 0
                if l > MAX_REF_LAYER
                    distanceL2toCurrentSAI(ypos,xpos) = Inf; 
                end
            end
        end
    end
end
%%

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
    if lowestDistance < Inf
        SAIREFUsage(bestYPos, bestXPos) = 1;
        if num_ref_pics < max_ref_pics
            num_ref_pics = num_ref_pics + 1;
        end
    end
    refSAIs(i) = refPOC - currentSAI;
end

% fill repeated refSAIs with the other SAIs so they are not discarded from
% the memory
listedRefSAIs = num_ref_pics;
for i = num_ref_pics+1:currentSAI
    for cand = 1:currentSAI
        difThan = 0;
        for j = 1:i-1
            if refSAIs(j) ~= -cand
                difThan = difThan + 1;
            end
        end
        if difThan == listedRefSAIs
            refSAIs(i) = -cand;
            listedRefSAIs = listedRefSAIs + 1;
            break;
        end
    end
end

end

function [raLayers] = getCurrentRALayer(currentSAIPos, raLayerMask)

currentYPos = currentSAIPos(1);
currentXPos = currentSAIPos(2);
[num_ra_layers,~,~] = size(raLayerMask);
numRALayersBeingUsed = 0;
for l = 1:num_ra_layers
    if raLayerMask(l, currentYPos, currentXPos) == 1
        numRALayersBeingUsed = numRALayersBeingUsed + 1;
        raLayers(numRALayersBeingUsed) = l; 
    end
end

end










