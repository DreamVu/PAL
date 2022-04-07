#!/bin/bash

./run

File1=lut/Res1.png 

validated=false 

if test -f "$File1"; then  
    cp -r lut/ ../bin_files/data/
    cp -r er/ ../bin_files/data/
    cp -r inv_er/ ../bin_files/data/ 
    
    cp -r lut/ /usr/local/bin/data/
    cp -r er/ /usr/local/bin/data/
    cp -r inv_er/ /usr/local/bin/data/
        
    rm -rf lut/ er/ inv_er/
    validated=true
fi


if $validated; then     
    rm -rf data
    rm data.zip	
    rm ./include/Res2_ERLeft_inverted.png ./include/Res2_ERLeft_upright.png   
    echo "[PAL:INFO] Camera data files are installed successfully." 
fi
 

