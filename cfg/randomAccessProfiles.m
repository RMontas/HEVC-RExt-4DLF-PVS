


if RA_FLAG == 0 %% NO RA
    ra_num_Layers = 1;
    raLayerMask = zeros(ra_num_Layers, num_MIs, num_MIs);
    raLayerMask(1,:,:) =[   1 1 1 1 1 1 1 1 1 1 1 1 1 ;
                            1 1 1 1 1 1 1 1 1 1 1 1 1 ;
                            1 1 1 1 1 1 1 1 1 1 1 1 1 ;
                            1 1 1 1 1 1 1 1 1 1 1 1 1 ;
                            1 1 1 1 1 1 1 1 1 1 1 1 1 ;
                            1 1 1 1 1 1 1 1 1 1 1 1 1 ;
                            1 1 1 1 1 1 1 1 1 1 1 1 1 ;
                            1 1 1 1 1 1 1 1 1 1 1 1 1 ;
                            1 1 1 1 1 1 1 1 1 1 1 1 1 ;
                            1 1 1 1 1 1 1 1 1 1 1 1 1 ;
                            1 1 1 1 1 1 1 1 1 1 1 1 1 ;
                            1 1 1 1 1 1 1 1 1 1 1 1 1 ;
                            1 1 1 1 1 1 1 1 1 1 1 1 1 ];
                    %%
elseif RA_FLAG == 1 %% HEMISPHERES 2-way
    ra_num_Layers = 2;
    raLayerMask = zeros(ra_num_Layers, num_MIs, num_MIs);
    
    raLayerMask(1,:,:) =[   1 1 1 1 1 1 1 1 1 1 1 1 1 ; %% N
                            1 1 1 1 1 1 1 1 1 1 1 1 1 ;
                            1 1 1 1 1 1 1 1 1 1 1 1 1 ;
                            1 1 1 1 1 1 1 1 1 1 1 1 1 ;
                            1 1 1 1 1 1 1 1 1 1 1 1 1 ;
                            1 1 1 1 1 1 1 1 1 1 1 1 1 ;
                            1 1 1 1 1 1 1 1 1 1 1 1 1 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ];
                        
    raLayerMask(2,:,:) =[   0 0 0 0 0 0 0 0 0 0 0 0 0 ; %% S
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            1 1 1 1 1 1 1 1 1 1 1 1 1 ;
                            1 1 1 1 1 1 1 1 1 1 1 1 1 ;
                            1 1 1 1 1 1 1 1 1 1 1 1 1 ;
                            1 1 1 1 1 1 1 1 1 1 1 1 1 ;
                            1 1 1 1 1 1 1 1 1 1 1 1 1 ;
                            1 1 1 1 1 1 1 1 1 1 1 1 1 ;
                            1 1 1 1 1 1 1 1 1 1 1 1 1 ];
                        
                    %%
elseif RA_FLAG == 2 %% QUADRANTS 4-way
    ra_num_Layers = 4;
    raLayerMask = zeros(ra_num_Layers, num_MIs, num_MIs);
    
    raLayerMask(1,:,:) =[   1 1 1 1 1 1 1 0 0 0 0 0 0 ; %% NW
                            1 1 1 1 1 1 1 0 0 0 0 0 0 ;
                            1 1 1 1 1 1 1 0 0 0 0 0 0 ; 
                            1 1 1 1 1 1 1 0 0 0 0 0 0 ;
                            1 1 1 1 1 1 1 0 0 0 0 0 0 ;
                            1 1 1 1 1 1 1 0 0 0 0 0 0 ;
                            1 1 1 1 1 1 1 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ];
    
    raLayerMask(2,:,:) =[   0 0 0 0 0 0 1 1 1 1 1 1 1 ; %% NE
                            0 0 0 0 0 0 1 1 1 1 1 1 1 ;
                            0 0 0 0 0 0 1 1 1 1 1 1 1 ;
                            0 0 0 0 0 0 1 1 1 1 1 1 1 ;
                            0 0 0 0 0 0 1 1 1 1 1 1 1 ;
                            0 0 0 0 0 0 1 1 1 1 1 1 1 ;
                            0 0 0 0 0 0 1 1 1 1 1 1 1 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ];
                                              
    raLayerMask(3,:,:) =[   0 0 0 0 0 0 0 0 0 0 0 0 0 ; %% SW
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            1 1 1 1 1 1 1 0 0 0 0 0 0 ;
                            1 1 1 1 1 1 1 0 0 0 0 0 0 ;
                            1 1 1 1 1 1 1 0 0 0 0 0 0 ;
                            1 1 1 1 1 1 1 0 0 0 0 0 0 ;
                            1 1 1 1 1 1 1 0 0 0 0 0 0 ;
                            1 1 1 1 1 1 1 0 0 0 0 0 0 ;
                            1 1 1 1 1 1 1 0 0 0 0 0 0 ];
                        
    raLayerMask(4,:,:) =[   0 0 0 0 0 0 0 0 0 0 0 0 0 ; %% SE
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 1 1 1 1 1 1 1 ;
                            0 0 0 0 0 0 1 1 1 1 1 1 1 ;
                            0 0 0 0 0 0 1 1 1 1 1 1 1 ;
                            0 0 0 0 0 0 1 1 1 1 1 1 1 ;
                            0 0 0 0 0 0 1 1 1 1 1 1 1 ;
                            0 0 0 0 0 0 1 1 1 1 1 1 1 ;
                            0 0 0 0 0 0 1 1 1 1 1 1 1 ];
                    %%    
elseif RA_FLAG == 3 %% 9-way 
    ra_num_Layers = 9;
    raLayerMask = zeros(ra_num_Layers, num_MIs, num_MIs);
    
    raLayerMask(1,:,:) =[   1 1 1 1 1 0 0 0 0 0 0 0 0 ; %% NW
                            1 1 1 1 1 0 0 0 0 0 0 0 0 ;
                            1 1 1 1 1 0 0 0 0 0 0 0 0 ; 
                            1 1 1 1 1 0 0 0 0 0 0 0 0 ;
                            1 1 1 1 1 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ];
                        
    raLayerMask(2,:,:) =[   0 0 0 0 1 1 1 1 1 0 0 0 0 ; %% N
                            0 0 0 0 1 1 1 1 1 0 0 0 0 ;
                            0 0 0 0 1 1 1 1 1 0 0 0 0 ;
                            0 0 0 0 1 1 1 1 1 0 0 0 0 ;
                            0 0 0 0 1 1 1 1 1 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ];    
                        
	raLayerMask(3,:,:) =[   0 0 0 0 0 0 0 0 1 1 1 1 1 ; %% NE
                            0 0 0 0 0 0 0 0 1 1 1 1 1 ;
                            0 0 0 0 0 0 0 0 1 1 1 1 1 ; 
                            0 0 0 0 0 0 0 0 1 1 1 1 1 ;
                            0 0 0 0 0 0 0 0 1 1 1 1 1 ; 
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ];
                        
    raLayerMask(4,:,:) =[   0 0 0 0 0 0 0 0 0 0 0 0 0 ; %% W
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ; 
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            1 1 1 1 1 0 0 0 0 0 0 0 0 ;
                            1 1 1 1 1 0 0 0 0 0 0 0 0 ;
                            1 1 1 1 1 0 0 0 0 0 0 0 0 ;
                            1 1 1 1 1 0 0 0 0 0 0 0 0 ;
                            1 1 1 1 1 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ];
                        
	raLayerMask(5,:,:) =[   0 0 0 0 0 0 0 0 0 0 0 0 0 ; %% Center
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ; 
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 1 1 1 1 1 0 0 0 0 ;
                            0 0 0 0 1 1 1 1 1 0 0 0 0 ;
                            0 0 0 0 1 1 1 1 1 0 0 0 0 ;
                            0 0 0 0 1 1 1 1 1 0 0 0 0 ;
                            0 0 0 0 1 1 1 1 1 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ];
                        
	raLayerMask(6,:,:) =[   0 0 0 0 0 0 0 0 0 0 0 0 0 ; %% E
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ; 
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 1 1 1 1 1 ;
                            0 0 0 0 0 0 0 0 1 1 1 1 1 ;
                            0 0 0 0 0 0 0 0 1 1 1 1 1 ;
                            0 0 0 0 0 0 0 0 1 1 1 1 1 ;
                            0 0 0 0 0 0 0 0 1 1 1 1 1 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ];
                        
    raLayerMask(7,:,:) =[   0 0 0 0 0 0 0 0 0 0 0 0 0 ; %% SW
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            1 1 1 1 1 0 0 0 0 0 0 0 0 ;
                            1 1 1 1 1 0 0 0 0 0 0 0 0 ;
                            1 1 1 1 1 0 0 0 0 0 0 0 0 ;
                            1 1 1 1 1 0 0 0 0 0 0 0 0 ; 
                            1 1 1 1 1 0 0 0 0 0 0 0 0 ];
                        
    raLayerMask(8,:,:) =[   0 0 0 0 0 0 0 0 0 0 0 0 0 ; %% S
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 1 1 1 1 1 0 0 0 0 ;
                            0 0 0 0 1 1 1 1 1 0 0 0 0 ; 
                            0 0 0 0 1 1 1 1 1 0 0 0 0 ;
                            0 0 0 0 1 1 1 1 1 0 0 0 0 ;
                            0 0 0 0 1 1 1 1 1 0 0 0 0 ];
                        
    raLayerMask(9,:,:) =[   0 0 0 0 0 0 0 0 0 0 0 0 0 ; %% SE
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 0 0 0 0 0 ;
                            0 0 0 0 0 0 0 0 1 1 1 1 1 ;
                            0 0 0 0 0 0 0 0 1 1 1 1 1 ;
                            0 0 0 0 0 0 0 0 1 1 1 1 1 ;
                            0 0 0 0 0 0 0 0 1 1 1 1 1 ;
                            0 0 0 0 0 0 0 0 1 1 1 1 1 ];
end











