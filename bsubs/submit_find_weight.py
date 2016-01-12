#!/usr/bin/env python

import os
import sys
import commands

totalJobs = ["Wenu", "ttbar", "Zee", "singleT", "Diboson", "Wtaunu", "Znunu",
          "Wmunu", "Zmumu", "Ztautau"]

monojetDir=os.getenv("MonoJetCodeDir")

workdir = os.getcwd()
outdir = os.getcwd() + "/weights/" 

goodjobs = []
badjobs = []
for job in  totalJobs:
    input_name = workdir+"/"+job+".list"
    outname = job+'_weight.txt'
    run_cmd = monojetDir+"/MonoJet/bsubs/run_find_weight.sh "+\
            input_name+ " "+outdir+" "+outname
    print run_cmd
    #-G u_zp -q 8nh for atlas sources
    #-G ATLASWISC_GEN -q wisc for wisconsin sources
    bsubs_cmd = "bsub -G ATLASWISC_GEN -q wisc  -R 'pool>4000' -C 0 -o" + \
            workdir+ "/output "+ run_cmd

    status,output=commands.getstatusoutput(bsubs_cmd)
    if status != 0:
        badjobs.append(0)
    else:
        goodjobs.append(1)

print "Good jobs: "+ str(len(goodjobs))+", "+str(len(badjobs))+" failed!"
