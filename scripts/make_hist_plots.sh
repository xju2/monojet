#!/bin/bash
regions="SR WENU WMUNU ZMM ZEE"
met_cuts="1"
variables="met"
for variable in ${variables}
do
    for met_cut in ${met_cuts}
    do
        for region in ${regions}
        do 
            hist_name="${variable}_${regions}_${met_cut}"
            echo ${hist_name}
            root -l -b -q plot_from_hist.cxx\(\"${hist_name}\"\)
        done
    done
done
