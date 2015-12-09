#!/usr/bin/env python
import subprocess
import glob
import os

path_cmb = "combined"
if not os.path.exists(path_cmb):
    os.mkdir(path_cmb)
    
samples= ["Wenu", "ttbar", "Zee", "Diboson", "Wtaunu", "Znunu",
          "Wmunu", "Zmumu", "Ztautau", "Dijets", "signal"]
#samples = ["Wenu"]
bkg_lists = []
for sample in samples:
    print sample
    allfiles = glob.glob(sample+"/*root")
    # check if we have all the files
    count_list = subprocess.check_output(["wc", sample+".list"]).split()[0]
    count_files = len(allfiles)
    if True:
        args = ["hadd"]
        out_name = path_cmb+"/"+sample+"_combined.root"
        args.append(out_name)
        args += allfiles
        p = subprocess.Popen(args, stdout=subprocess.PIPE).communicate()
        if "signal" not in sample:
            bkg_lists.append(out_name)
    else:
        print "expected: ",str(count_list)," obtained: ",str(count_files)

cmd_bkg = ['hadd', path_cmb+'/bkg_combined.root']
p = subprocess.Popen(cmd_bkg+bkg_lists, stdout=subprocess.PIPE).communicate()
# data
cmd_data = ['hadd',path_cmb+'/data_combined.root']
all_data = glob.glob("data/*root")
cmd_data += all_data
p = subprocess.Popen(cmd_data, stdout=subprocess.PIPE).communicate()

# merge sub-sample
p = subprocess.Popen(['hadd',path_cmb+'/W_combined.root',
                      path_cmb+'/Wenu_combined.root',
                      path_cmb+'/Wtaunu_combined.root',
                      path_cmb+'/Wmunu_combined.root'], 
                     stdout=subprocess.PIPE).communicate()
p = subprocess.Popen(['hadd',
                      path_cmb+'/Z_combined.root',
                      path_cmb+'/Zmumu_combined.root',
                      path_cmb+'/Zee_combined.root',
                      path_cmb+'/Ztautau_combined.root'], 
                     stdout=subprocess.PIPE).communicate()
p = subprocess.Popen(['hadd',
                      path_cmb+'/top_combined.root',
                      #path_cmb+'/singleT_combined.root',
                      path_cmb+'/ttbar_combined.root'], 
                     stdout=subprocess.PIPE).communicate()
