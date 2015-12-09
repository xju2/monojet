#!/bin/bash
#samples="Wenu ttbar Zee Diboson Wtaunu Znunu Wmunu Zmumu Ztautau Dijets DMV"
samples="Wenu ttbar Zee Diboson Wtaunu Znunu Wmunu Zmumu Ztautau Dijets Gamma"
#base_dir="root://eosatlas//eos/atlas/unpledged/group-wisc/xrootd/user/fwang/MonoJet/output_MC15_v4"
base_dir="${GROUPEOSDIR}/monojet/minitrees/mc15_13TeV_v5"
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

#data_dir=$GROUPEOSDIR/monojet/data/EPS_2015/
#data_dir=$GROUPEOSDIR/monoH4l/minitrees/mc15_13TeV_v1/data/
data_dir="$GROUPEOSDIR/monojet/minitrees/mc15_13TeV_v5/data/"
#data_dir="${base_dir}/data/"
eos ls ${data_dir} | sed 's#^#'"${data_dir}"'#' > data.list

echo "/afs/cern.ch/user/x/xju/work/monoJet/process/do_minitree/mc15_13TeV_v3/signal_minitree/combined_dmA_50_300.root" > signal.list
