#!/bin/bash
samples="Wenu ttbar Zee Diboson Wtaunu Znunu Wmunu Zmumu Ztautau Dijets"
#samples="Dijets"
script="${MonoJetCodeDir}/StackProcess/bsubs/submit_make_hists.py"

weight_dir=$1

if [ "x$weight_dir" == "x" ];then
    echo "provide weight directory"
    exit
fi
weight_dir=`pwd`/${weight_dir}
echo ${weight_dir}


for sample in ${samples}
do
    echo $sample
    python ${script} ${sample}.list ${sample} --use_weight ${weight_dir}/${sample}_weight.txt --filesPerJob 1 --noDphiCut
    #python ${script} ${sample}.list ${sample} --use_weight ${weight_dir}/all_weights.txt --filesPerJob 2 --noDphiCut
done

#submit for data
#python ${script} data.list data --isData --noDphiCut

