#!/bin/bash
file_dir="$GROUPEOSDIR/monojet/minitrees/mc15_13TeV_v5/data/"
eos ls $file_dir | sed 's#^#'"$file_dir"'#' > data.list
