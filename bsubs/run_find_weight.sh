#!/bin/bash
inputname=$1
basedir=$2
outname=$3

echo "---Start the Job---"
echo ${ROOTCOREBIN}
echo ${ROOTCOREDIR}
if [ "x${ROOTCOREBIN}" == "x" ]; then
    shift $#
    source ${MonoJetCodeDir}/rcSetup.sh 
else
. /cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase/x86_64/Gcc/gcc493_x86_64_slc6/slc6/gcc49/setup.sh /afs/cern.ch/sw/lcg/contrib
fi
which root
which gcc
cat $inputname
${ROOTCOREBIN}/bin/x86_64-slc6-gcc49-opt/get_lumi_weight ${inputname}  ${outname}

if [ ! -d ${basedir} ];then
    mkdir -vp ${basedir}
fi
echo "save outputs to ${basedir}"

cp ${outname} ${basedir}/
echo "---End of Job---"
