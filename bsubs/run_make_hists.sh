#!/bin/bash
inputlist=$1
basedir=$2
outname=$3
otherOpt=$4
echo "---Start the Job---"
echo ${ROOTCOREBIN}
echo ${ROOTCOREDIR}
if [ "x${ROOTCOREBIN}" == "x" ]; then
    shift $#
    source ${MonoJetCodeDir}/rcSetup.sh 
else
. /cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase/x86_64/Gcc/gcc481_x86_64_slc6/slc6/x86_64-slc6-gcc48-opt/setup.sh /afs/cern.ch/sw/lcg/contrib
fi
which root
which gcc
#cat $inputlist
echo "${ROOTCOREBIN}/bin/x86_64-slc6-gcc48-opt/CountSingleProcess $inputlist ${otherOpt}"
echo "other options: ${otherOpt}"
${ROOTCOREBIN}/bin/x86_64-slc6-gcc48-opt/CountSingleProcess "$inputlist" ${otherOpt}

if [ ! -d ${basedir} ];then
    mkdir -vp ${basedir}
fi
echo "save outputs to ${basedir}"

cp output.root ${basedir}/${outname}
echo "---End of Job---"
