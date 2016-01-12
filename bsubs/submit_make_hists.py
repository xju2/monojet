#!/usr/bin/env python

import os
import sys
import commands
from datetime import datetime 
from optparse import OptionParser

usage="usage: "+sys.argv[0]+" background.list outDir"
parser = OptionParser(usage, version="0.1")
parser.add_option("--isData", action="store_true", dest="is_data", 
                  default=False, help="is data?")
parser.add_option("--nentries", default="-1", dest="nentries", help="number of entries")
parser.add_option("--mc_id", default="-1", dest="mc_id", help="require which mc")
parser.add_option("--jetCut", default="4", dest="jetcut", help="jet cuts")
parser.add_option("--use_weight", default="none", dest="use_weight", 
                  help="use weightfile")
parser.add_option("--filesPerJob", default=2, dest="files_per_job", 
                  help="files per job")
parser.add_option("--noDphiCut", action="store_true", default=False, dest="nodphi", 
                  help="no Dphi Cut")
parser.add_option("--no_weight", action="store_true", default=False,
                  dest="noweight", help="no Dphi Cut")
options,args = parser.parse_args()


workdir=os.getcwd()
zz4lDir=os.getenv("MonoJetCodeDir")
outdir=workdir

#config the job
maxFile = options.files_per_job

if len(args) > 1:
    myFileList = args[0]
    outdirname = args[1]
else:
    print usage
    exit(1)

otheroptions = ""
if options.is_data:
    otheroptions += " -isData "

if options.nentries != "-1":
    otheroptions += " -num_evts="+options.nentries

if options.mc_id != "-1":
    otheroptions += " -mc_id="+options.mc_id

if options.nodphi:
    otheroptions += " -nodphi"

if options.noweight:
    otheroptions += " -noweight"

if options.use_weight != "none":
    otheroptions += " -use_weight="+options.use_weight

#print the configuration
print "max file(s) per job: ",maxFile
print "file list: ",myFileList
print "out put directory: ",outdirname
print "other options: *", otheroptions,"*"

prefix_name = myFileList.split('.')[0]

outdir = workdir+"/"+outdirname
os.system('mkdir -vp ./Lists')
split_cmd = '/afs/cern.ch/user/x/xju/tool/splitFile.py\
        ./Lists --list='+ myFileList + ' --nFiles='+str(maxFile)+\
        " --prefix="+prefix_name

status,output=commands.getstatusoutput(split_cmd)
print output

totalJobs = int(output.split(':')[5])
goodjobs = []
badjobs = []
for job in  range(totalJobs):
    inputlist = workdir+"/Lists/"+prefix_name+"_n"+str(job)+'.list'
    outname = myFileList+"_"+str(job)+".root"
    run_cmd = zz4lDir+"/MonoJet/bsubs/run_make_hists.sh "+\
            inputlist+ " "+outdir+" "+outname+' "'+otheroptions+'"'
    #print run_cmd
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
