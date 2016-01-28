#!/bin/bash
#samples="Wenu ttbar Zee Diboson Wtaunu Znunu Wmunu Zmumu Ztautau Dijets DMV"
samples="Wenu ttbar Zee Diboson Wtaunu Znunu Wmunu Zmumu Ztautau Dijets"
tag_name="mc15b_v1"
base_dir="${GROUPEOSDIR}/monojet/minitrees/${tag_name}"
for sample in ${samples}
do
    outputName=${sample}
    #if [ ${sample} == "Dijets.susy1.p2357" ]; then
    #    outputName="Dijets"
    #fi
    echo $outputName 
    fullDir=${base_dir}/${sample}/
    #echo ${fullDir}
    eos ls ${fullDir} | sed 's#^#'"${fullDir}"'#' > ${outputName}.list
done

data_dir="$GROUPEOSDIR/monojet/minitrees/${tag_name}/data/"
#data_dir="${base_dir}/data/"
eos ls ${data_dir} | sed 's#^#'"${data_dir}"'#' > data.list

#echo "/afs/cern.ch/user/x/xju/work/monoJet/process/do_minitree/mc15_13TeV_v3/signal_minitree/combined_dmA_50_300.root" > signal.list
