#!/bin/bash
basedir=$1
input_file=$2
#run_number=$3
out_file_name=$3
neventsPerThread=1000
if [ "$#" -gt "4" ]; then
    neventsPerThread=$5
fi

echo "---Start the Job---"
echo ${ROOTCOREBIN}
echo ${ROOTCOREDIR}

if [ "x${ROOTCOREBIN}" == "x" ]; then
    shift $#
    curr_dir=$PWD
    cd ${MonoJetCodeDir}
    source rcSetup.sh
    cd $curr_dir
else
    . /cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase/x86_64/root/6.04.16-x86_64-slc6-gcc49-opt/bin/thisroot.sh
    . /cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase/x86_64/Gcc/gcc493_x86_64_slc6/slc6/gcc49/setup.sh /afs/cern.ch/sw/lcg/contrib
fi

which root
which gcc

#ps_file_name=${run_number}_ps.txt
#ps_dir="/afs/cern.ch/user/x/xju/work/monoJet/jetSmearing/ps_per_lb/"
#cp ${ps_dir}${ps_file_name} .
#jetsmearing.py $ps_file_name $input_file "$neventsPerThread"
#jetsmearing.py $input_file "$neventsPerThread"
jet_smearing $input_file  ${out_file_name}

#if [ ! -d ${basedir} ];then
#    mkdir -vp ${basedir}
#fi
echo "save outputs to ${basedir}"

#cp ${out_file_name} ${basedir}/${out_file_name}
xrdcp ${out_file_name} root://eosatlas//eos/atlas/unpledged/group-wisc/users/xju//monojet/smearing/smearing_tool_v20/${basedir}/${out_file_name}
echo "---End of Job---"
