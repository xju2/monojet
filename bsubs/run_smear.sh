#!/bin/bash
basedir=$1
input_file=$2
run_number=$3
out_file_name=$4
neventsPerThread=1000
if [ "$#" -gt "4" ]; then
    neventsPerThread=$5
fi

echo "---Start the Job---"
echo ${ROOTCOREBIN}
echo ${ROOTCOREDIR}
if [ "x${ROOTCOREBIN}" == "x" ]; then
    shift $#
    source ${MonoJetCodeDir}/rcSetup.sh 
else
. /cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase/x86_64/Gcc/gcc484_x86_64_slc6/slc6/x86_64-slc6-gcc48-opt/setup.sh /afs/cern.ch/sw/lcg/contrib
fi
which root
which gcc

ps_file_name=${run_number}_ps.txt
ps_dir="/afs/cern.ch/user/x/xju/work/monoJet/jetSmearing/ps_weight_per_lb/"
cp ${ps_dir}${ps_file_name} .
jetsmearing.py $ps_file_name $input_file "$neventsPerThread"

hadd ${out_file_name} jj*root

if [ ! -d ${basedir} ];then
    mkdir -vp ${basedir}
fi
echo "save outputs to ${basedir}"

cp ${out_file_name} ${basedir}/${out_file_name}
echo "---End of Job---"
