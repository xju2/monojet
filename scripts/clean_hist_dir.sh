#!/bin/bash
samples="Wenu ttbar Zee singleT Diboson Wtaunu Znunu Wmunu Zmumu Ztautau Dijets"
echo "cleaning directories"
for sample in ${samples}
do
    echo $sample
    \rm -rf ${sample}
done
\rm Lists/*
\rm data/*
