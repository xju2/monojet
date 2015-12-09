#!/usr/bin/env python
import ROOT
import os
import glob
import numpy as np
import subprocess
class Analizer:
    def __init__(self, lumi):
        self.xsection = {}
        self.total_events = {}
        self.lumi = lumi
        #self.rootcorebin = os.getenv("ROOTCOREBIN")
        self.rootcorebin = "/afs/cern.ch/work/x/xju/monoJet/code/RootCoreBin"
        if self.rootcorebin == "":
            print "Error: ROOTCOREBIN is not defined"
    
    def load_xsection(self):
        if len(self.xsection.keys()) > 0:
            return
        xsection_file = self.rootcorebin+"/data/SUSYTools/susy_crosssections_13TeV.txt"
        print xsection_file
        with open(xsection_file) as f:
            next(f)
            for line in f:
                if line[0] == '#':
                    continue
                try: 
                    id,name,xsec,kfac,eff,reclunc = line.split()
                    id = int(id)
                    self.xsection[id] = float(xsec)*float(kfac)*float(eff)*float(reclunc)
                except:
                    print line," cannot be inteperted"
        print "xsections are loaded"

    def load_total_events(self):
        if len(self.total_events.keys()) > 0:
            return
        datasets_lists = glob.glob(self.rootcorebin+"/data/WiscAnalysis/*.dataset")
        print datasets_lists
        for dataset in datasets_lists:
            with open(dataset) as f:
                for line in f:
                    try:
                        name,nfiles,nevents = line.split()
                        id = float(name.split('.')[1])
                        self.total_events[id] = float(nevents)
                    except:
                        print line," cannot be interpted"
        print "total events are loaded"

    def read_files(self, input):
        self.load_xsection()
        self.load_total_events()
        args = ['xrdcp',input,'.']
        p = subprocess.Popen(args).communicate()
        inputname = os.path.basename(input)
        file = ROOT.TFile.Open(inputname, 'update')
        chain = file.Get('physics')
        nentries = chain.GetEntries()
        lumi_weight = np.zeros(1, dtype=float)
        weight_br = chain.Branch("lumi_weight",lumi_weight,"lumi_weight/D")
        print 'processing: ',inputname," total: ",nentries
        for ientry in range(nentries):
            chain.LoadTree(ientry)
            chain.GetEntry(ientry)
            chn = chain.mc_channel_number
            xs = self.xsection[chn]
            totalE = self.total_events[chn]
            lumi_weight[0] = 1.0*self.lumi*xs/totalE
            #print chn,xs,totalE,lumi_weight[0]
            weight_br.Fill()
        file.Write("",ROOT.TObject.kOverwrite)
        file.Close()
        args = ['xrdcp', '-f',inputname, input]
        p = subprocess.Popen(args).communicate()
        q = subprocess.Popen(['rm',inputname]).communicate()

if __name__ == '__main__':
    inputbase = os.getenv("GROUPEOSDIR")+"monojet/minitrees/mc14_13TeV_v1/"
    inputs_ls = [
    "Wenu_combined_mono_jet_mc14_13TeV.a.root",
    "Wmunu_combined_mono_jet_mc14_13TeV.a.root",
    "Wtaunu_combined_mono_jet_mc14_13TeV.a.root",
    "Zee_combined_mono_jet_mc14_13TeV.a.root",
    "Zmumu_combined_mono_jet_mc14_13TeV.a.root",
    "Znunu_combined_mono_jet_mc14_13TeV.a.root",
    "Ztautau_combined_mono_jet_mc14_13TeV.a.root"
    ]

    analysis = Analizer(lumi=5.0e6)
    for input in inputs_ls:
        analysis.read_files(inputbase+input)

