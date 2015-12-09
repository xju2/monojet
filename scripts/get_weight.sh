#!/bin/bash
#samples="Wenu ttbar Zee singleT Diboson Wtaunu Znunu Wmunu Zmumu Ztautau Dijets"
samples="Wenu ttbar Zee Diboson Wtaunu Znunu Wmunu Zmumu Ztautau Dijets signal"

weight_dir="weights"
if [ ! -d ${weight_dir} ];
then
    mkdir ${weight_dir}
fi

for sample in $samples
do
    echo $sample
    get_lumi_weight ${sample}.list ${weight_dir}/${sample}_weight.txt
done
