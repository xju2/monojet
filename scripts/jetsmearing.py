#!/usr/bin/env python

import ROOT
import AtlasStyle

import os
import sys
import threading
import math
from array import array 

ROOT.gROOT.SetBatch()
if not hasattr(ROOT, "loader"):
    ROOT.gROOT.LoadMacro("/afs/cern.ch/user/x/xju/tool/loader.c") 

ROOT.gROOT.LoadMacro(os.getenv("ROOTCOREBIN")+"/lib/x86_64-slc6-gcc49-opt/libCxxUtils.so")
ROOT.gROOT.LoadMacro(os.getenv("ROOTCOREBIN")+"/lib/x86_64-slc6-gcc49-opt/libAthContainers.so")
ROOT.gROOT.LoadMacro(os.getenv("ROOTCOREBIN")+"/lib/x86_64-slc6-gcc49-opt/libMonoJet.so")

triggers = ["HLT_j60",  
            "HLT_j110",  
            "HLT_j150",  #"HLT_j175",  
            "HLT_j200",
            "HLT_j260",  #"HLT_j300",  
            "HLT_j320",  "HLT_j360", "HLT_j400",
            #"HLT_j380", "HLT_j400", "HLT_j420", "HLT_j440", "HLT_j460"
           ]
trigger_plateau = [130, 170, 220, 280, 350, 390, 430]
# unit: ipb
lumi_list = [979.768*1E-3, 1441.75*1E-3,  5357.85*1E-3, 10698.3*1E-3,
              19701.4*1E-3, 68386.7*1E-3, 135.87, 191.304, 3316.68] 
unprescaled_lumi = 3316.68


debug = False

class MiniTree:
    def __init__(self):
        pass

    def load_ps(self, file_name):
        with open(file_name, 'r') as f:
            self.ps_dic = {}
            for line in f:
                if line[0] == '[':
                    trigger = line[1:-2]
                    #print trigger
                else:
                    two_part = line[:-1].split(',')
                    lb = float(two_part[0].split()[1])
                    ps = float(two_part[1].split()[1])
                    #self.add_name_to_dictionary(self.ps_dic, (trigger, lb, ps))
                    self.add_tup_to_dictionary(self.ps_dic, (trigger, lb, ps))
        #print self.ps_dic
    
    @staticmethod
    def add_name_to_dictionary(d, tup):
        if tup[0] not in d:
            d[tup[0]] = {}
        d[tup[0]][tup[1]] = tup[2]

    @staticmethod
    def add_tup_to_dictionary(d, tup):
        if tup[0] not in d:
            d[tup[0]] = []
        d[tup[0]].append((tup[1], tup[2]))

    def find_weight(self, trigger, lb_in):
        result = 0.0
        try:
            for lb,ps in self.ps_dic[trigger]:
                if lb_in < lb:
                    break
                else:
                    result = ps
        except KeyError:
            print "I don't know: ", trigger
        if debug:
                print trigger," ",lb_in," weight: ",result
        #if result > 1: result /= (55/1.5)
        return result
    
    @staticmethod
    def find_right_trigger(chain):
        pt = chain.jet_p4[0].Pt()/1E3
        if chain.trig_j400 and pt > 430:
            return "HLT_j400"
        elif chain.trig_j360 and pt > 390 and pt <= 430:
            return "HLT_j360"
        elif chain.trig_j320 and pt > 350 and pt <= 390:
            return "HLT_j320"
        elif chain.trig_j260 and pt > 280 and pt <= 350:
            return "HLT_j260"
        elif chain.trig_j200 and pt > 220 and pt <= 280:
            return "HLT_j200"
        elif chain.trig_j150 and pt > 170 and pt <= 220:
            return "HLT_j150"
        elif chain.trig_j110 and pt > 130 and pt <= 170:
            return "HLT_j110"
        elif chain.trig_j60 and pt > 80 and pt <= 130:
            return "HLT_j60"
        else:
            return None

    @staticmethod
    def get_met_sig(chain):
        return (chain.MET_et/1E3 - 8)/math.sqrt(chain.MET_sumet/1E3)
        #return (chain.MET_et/1E3)/math.sqrt(chain.MET_sumet/1E3)

    def change_file(self, file_name, start_n, nevents_be_processed):
        chain = ROOT.loader(file_name, "physics")
        nentries = chain.GetEntries()
        
        out_name = "jj_"+str(start_n)+".root"
        outfile = ROOT.TFile.Open(out_name, "recreate")
        #outtree = chain.CloneTree(0)
        outtree = ROOT.TTree("smeared", "smeared")
        seedtree = ROOT.TTree("seed", "seed")
        met = array('f', [0])
        jetpt = array('f', [0])
        jeteta = array('f', [0])
        subjetpt = array('f', [0])
        subjeteta = array('f', [0])
        njets = array('i', [0])
        dphi = array('f', [0])
        dphiEP = array('f', [0])
        rmet_pt = array('f', [0])
        run_number = array('i', [0])
        event_number = array('i', [0])
        lb = array('i', [0])
        weight = array('f', [0])
        outtree.Branch('met_et', met, 'met_et/F')
        outtree.Branch('njets', njets, 'njets/I')
        outtree.Branch('leading_jet_pt', jetpt, 'leading_jet_pt/F')
        outtree.Branch('leading_jet_eta', jeteta, 'leading_jet_eta/F')
        outtree.Branch('sub_leading_jet_pt', subjetpt, 'sub_leading_jet_pt/F')
        outtree.Branch('sub_leading_jet_eta', subjeteta, 'sub_leading_jet_eta/F')
        outtree.Branch('min_dphi', dphi, 'min_dphi/F')
        outtree.Branch("dphi_ep", dphiEP, 'dphi_ep/F')
        outtree.Branch('rmet_pt', rmet_pt, 'rmet_pt/F')
        outtree.Branch('run', run_number, 'run/I')
        outtree.Branch('event', event_number, 'event/I')
        outtree.Branch('lb', lb, 'lb/I')
        outtree.Branch('weight', weight, 'weight/F')
    
        seedtree.Branch('met_et', met, 'met_et/F')
        seedtree.Branch('njets', njets, 'njets/I')
        seedtree.Branch('leading_jet_pt', jetpt, 'leading_jet_pt/F')
        seedtree.Branch('leading_jet_eta', jeteta, 'leading_jet_eta/F')
        seedtree.Branch('min_dphi', dphi, 'min_dphi/F')
        #seedtree.Branch("dphi_ep", dphiEP, 'dphi_ep/F')
        #seedtree.Branch('rmet_pt', rmet_pt, 'rmet_pt/F')
        seedtree.Branch('run', run_number, 'run/I')
        seedtree.Branch('event', event_number, 'event/I')
        seedtree.Branch('lb', lb, 'lb/I')
        seedtree.Branch('weight', weight, 'weight/F')

        met_sig_br = array('f', [0])
        frac_soft_br = array('f', [0])
        seedtree.Branch("met_sig", met_sig_br, 'met_sig/F')
        seedtree.Branch("frac_soft", frac_soft_br, 'frac_soft/F')

        for ientry in range(nevents_be_processed):
            ientry = start_n + ientry
            if chain.LoadTree(ientry) < 0:
                break
            chain.GetEntry(ientry)
            #check trigger
            trigger = self.find_right_trigger(chain)
            if trigger:
                weight[0] = self.find_weight(trigger, chain.lumiblock)
            else:
                continue
            #apply lepton veto
            if chain.n_base_el > 0 or chain.n_base_mu > 0:
                continue
            #select the seed events
            met_sig =  self.get_met_sig(chain)
            frac_soft = chain.MET_et_soft/chain.MET_et
            frac_soft_br[0] = frac_soft

            met_sig_br[0] = met_sig
            run_number[0] = chain.RunNumber
            event_number[0] = chain.EventNumber
            lb[0] = chain.lumiblock
            jet_pt_origin = chain.jet_p4[0].Pt()
            met[0] = chain.MET_et/1E3
            jetpt[0] = jet_pt_origin/1E3
            jeteta[0] = chain.jet_p4[0].Eta()
            njets[0] = chain.n_good_jet
            dphi[0] = chain.min_dphi_jetMET
            seedtree.Fill()


            if met_sig < 0.7:
                for data in chain.pseudoData:
                    met[0] = data.met_/1E3
                    jetpt[0] = data.leading_jet_pt_/1E3
                    jeteta[0] = data.leading_jet_eta_
                    subjetpt[0] = data.sub_leading_jet_pt_/1E3
                    subjeteta[0] = data.sub_leading_jet_eta_
                    njets[0] = data.n_good_jets_
                    dphi[0] = data.min_jets_met_
                    dphiEP[0] = data.dphi_EP_
                    rmet_pt[0] = data.met_/data.leading_jet_pt_
                    outtree.Fill()
        outfile.cd()
        outtree.Write()
        seedtree.Write()
        outfile.Close()

def test_minitree(start_n, data_list, ps_file_name, nevents):
    mini = MiniTree()
    mini.load_ps(ps_file_name)
    mini.change_file(data_list, start_n, nevents)

if __name__ == "__main__":
    file_name = ""
    threads = []
    ps_file_name = "284484_ps.txt"
    data_list = "../testarea/data_smeared.list"
    nevents_per_thread = 1000

    if len(sys.argv) > 2:
        ps_file_name = sys.argv[1]
        data_list = sys.argv[2]
    else:
        print sys.argv[0]," ps_file data_list"
        sys.exit(0)

    if len(sys.argv) > 3:
        nevents_per_thread = int(sys.argv[3])
    
    print ps_file_name,data_list
    ch = ROOT.loader(data_list, "physics")
    nentries = ch.GetEntries()

    if nentries < 1:
        sys.exit(1)

    total_jobs = nentries/nevents_per_thread + 1
    print "total entries: ",nentries," jobs: ",total_jobs
    for i_thread in range(total_jobs):
        start_n = i_thread*1000
        t = threading.Thread(target=test_minitree, args=(start_n,
                                                         data_list,
                                                         ps_file_name, 
                                                        nevents_per_thread))
        threads.append(t)
        t.start()
