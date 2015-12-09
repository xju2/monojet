#!/bin/bash
samples="data signal Znunu Wmunu Wenu Wtaunu Zmumu Zee Ztautau ttbar Diboson"
outyield_name="yield_input.list"
if [ -f $outyield_name ]; then
    rm $outyield_name
fi
touch $outyield_name
for sample in ${samples}
do
    echo $sample
    echo ${sample} combined/${sample}_combined.root >> ${outyield_name}
done
