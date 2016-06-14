#!/usr/bin/env python

import os
import sys
import commands
from optparse import OptionParser

def get_run_number(input_list):
    with open(input_list,'r') as f:
        for line in f:
            return line.split('/')[-2].split('.')[2][2:]

usage="usage: "+sys.argv[0]+" input.list outDir"
parser = OptionParser(usage, version="0.1")
parser.add_option("--filesPerJob", default=1, dest="files_per_job",
                  help="files per job")
parser.add_option('-n', default=1000,
                  dest="events_per_thread", 
                  help="events per thread")
options,args = parser.parse_args()


workdir=os.getcwd()
zz4lDir=os.getenv("MonoJetCodeDir")
outdir=workdir

#config the job
maxFile = options.files_per_job
nevents = options.events_per_thread

if len(args) > 1:
    myFileList = args[0]
    outdirname = args[1]
else:
    print usage
    exit(1)

print "max file(s) per job: ",maxFile
print "file list: ",myFileList
print "out put directory: ",outdirname
print "Number of events per thread: ",nevents

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
    run_number = get_run_number(inputlist)
    outname = myFileList+"_"+str(job)+".root"
    run_cmd = zz4lDir+"/MonoJet/bsubs/run_smear.sh "+\
            outdirname+" "+inputlist+" "+run_number+" "+outname+" "+str(nevents)
    #print run_cmd
    #-G u_zp -q 8nh for atlas sources
    #-G ATLASWISC_GEN -q wisc for wisconsin sources
    #continue
    bsubs_cmd = "bsub -G ATLASWISC_GEN -q wisc  -R 'pool>4000' -C 0 -o" + \
            workdir+ "/output "+ run_cmd

    status,output=commands.getstatusoutput(bsubs_cmd)
    if status != 0:
        badjobs.append(0)
    else:
        goodjobs.append(1)

print "Good jobs: "+ str(len(goodjobs))+", "+str(len(badjobs))+" failed!"
