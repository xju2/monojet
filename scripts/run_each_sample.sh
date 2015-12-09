#!/bin/bash
weight_dir="/afs/cern.ch/user/x/xju/work/monoJet/process/do_minitree/mc15_13TeV_v3"
sample_name="DMV"
out_name="DMV"
mc_id_lists=`cat ../weights/${sample_name}_weight.txt  | awk -F: '{printf("%d ", $1)}'`
echo ${sample_name}
for mc_id in $mc_id_lists
do
    #echo ${mc_id}
    CountSingleProcess ${sample_name}.list -mc_id=${mc_id} -use_weight=${weight_dir}/weights/${sample_name}_weight.txt -nodphi -signal
    if [ ! -d ${sample_name} ]; then
        mkdir ${sample_name}
    fi
    mv output.root ${sample_name}/${sample_name}_${mc_id}.root
done
