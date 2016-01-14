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

ROOT.gROOT.LoadMacro(os.getenv("ROOTCOREBIN")+"/lib/x86_64-slc6-gcc48-opt/libCxxUtils.so")
ROOT.gROOT.LoadMacro(os.getenv("ROOTCOREBIN")+"/lib/x86_64-slc6-gcc48-opt/libAthContainers.so")
ROOT.gROOT.LoadMacro(os.getenv("ROOTCOREBIN")+"/lib/x86_64-slc6-gcc48-opt/libMonoJet.so")

triggers = [#"HLT_j100",  
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
class HistMgr:
    def __init__(self, tag_name):
        self.tag = tag_name
        all_list = []
        met_xbins = [50, 100, 125, 150, 175, 
                     200, 225, 250, 275, 300, 350, 400,
                     450, 500, 600, 800, 1000]
        all_list.append(self.create_hist_tuple("met", met_xbins))
        all_list.append(self.create_hist_tuple("jetpt", met_xbins))
        all_list.append(self.create_hist_tuple("njets", 11, -0.5, 10.5))
        all_list.append(self.create_hist_tuple("met_sig", 100, 0, 10))
        all_list.append(self.create_hist_tuple("dphi", 64, 0, 3.2))
        all_list.append(self.create_hist_tuple("dphiEP", 64, 0, 3.2))
        all_list.append(self.create_hist_tuple("Rmet_pt", 40, 0, 4))
        self.list_dic = dict(all_list)

    def create_hist_tuple(self, name, nbins, low, high):
        res_list = []
        for trig in triggers:
            hist_name = "h_"+self.tag+"_"+name+"_"+trig
            res_list.append(
                ROOT.TH1F(hist_name, hist_name, nbins, low, high)
            )
        return (name, res_list)

    def create_hist_tuple(self, name, bin_list):
        res_list = []
        nbins = len(bin_list) - 1 
        for trig in triggers:
            hist_name = "h_"+self.tag+"_"+name+"_"+trig
            res_list.append(
                ROOT.TH1F(hist_name, hist_name, nbins, array('f', bin_list))
            )
        return (name, res_list)
        

    def fill(self, itrig, weight, **data):
        for key,value in data.iteritems():
            all_list[key][itrig].Fill(value, weight)
    
    def merged_hists(self):
        hist_list = []
        for name, hist_trig_list in self.list_dic.iteritems():
            hist_list.append(self.add_hist(hist_trig_list, self.tag+"_"+name))
        return hist_list

    @staticmethod
    def add_hist(hist_list, hist_name):
        icount = 0
        for hist in hist_list:
            if icount == 0:
                all_hist = hist.Clone(hist_name)
            else:
                all_hist.Add(hist)
            icount += 1
        return all_hist

class jetsmearing:
    def __init__(self):
        print "Using Jet Smearing"
        self.seed_hists = HistMgr("seed")
        self.all_hists = HistMgr("all")
        self.smeared_hists = HistMgr("smeared")

    def pass_trigger(self):
        chain = self.chain
        return chain.trig_j100 or chain.trig_j110 or chain.trig_j150 or\
                chain.trig_j175 or chain.trig_j200 or chain.trig_j260 or\
                chain.trig_j300 or chain.trig_j320\
                or chain.trig_j360 or chain.trig_j400

    def pass_trig_only(self, trig_name):
        icount = 0
        out = True
        for trig in triggers:
            res = getattr(self.chain, trig.replace("HLT","trig"))
            if trig == trig_name:
                out = res and out
            else:
                out = not res and out
        return out

    def is_seed_events(self):
        ch = self.chain
        met_sig = ch.MET_et/1e3/math.sqrt(ch.MET_sumet/1e3)
        return met_sig < 0.7 and ch.n_base_el == 0 and ch.n_base_mu == 0 
        #and ch.n_good_jet < 5 and ch.n_good_jet > 0 

    def read_file(self, file_name, out_name):
        self.chain = ROOT.loader(file_name, "physics")
        ch = self.chain
        nentries = ch.GetEntries()
        hists_met = []
        for ientry in range(nentries):
        #for ientry in range(10000):
            ch.GetEntry(ientry)
            ch.LoadTree(ientry)

            #if ientry % 10000 == 0: print ientry
            icount = 0
            jet_pt = ch.jet_p4[0].Pt()/1E3
            dphi_EP = abs(ROOT.TVector2.Phi_mpi_pi(ch.MET_phi-ch.MET_trk_phi))
            met_sig = ch.MET_et/1e3/math.sqrt(ch.MET_sumet/1e3)
            met = ch.MET_et/1E3
            data_dic = {"met": met,
                        "jetpt": jet_pt,
                        "njets": ch.n_good_jet,
                        "met_sig": met_sig,
                        "dphi": ch.min_dphi_jetMET,
                        "dphiEP": dphi_EP,
                        "Rmet_pt": met/jet_pt,
                       }
            for trig,lumi in zip(triggers, lumi_list):
                weight = unprescaled_lumi/lumi
                if self.pass_trig_only(trig):
                    if jet_pt < 150: continue

                    self.all_hists.fill(icount, weight, data_dic)
                    if self.is_seed_events():
                        self.seed_hists.fill(icount, weight, data_dic)
                        self.fill_pseudo_data(icount, weight)

        fout = ROOT.TFile.Open(out_name, "recreate")
        for hist in self.all_hists.merged_hists():
            hist.Write()
        for hist in self.seed_hists.merged_hists():
            hist.Write()
        for hist in self.smeared_hists.merged_hists():
            hist.Write()
        fout.Close()

    def fill_pseudo_data(self, icount, weight):
        for data in self.chain.pseudoData:
            met_sig = data.met_/1E3/math.sqrt(data_sumet_/1E3)
            data_dic = {"met": data.met_/1E3,
                        "jetpt": data.leading_jet_pt_/1E3,
                        "njets": data.n_good_jets_,
                        "met_sig": met_sig,
                        "dphi": data.min_jets_met_,
                        "dphiEP": data.dphi_EP_,
                        "Rmet_pt": data.met_/data.leading_jet_pt_,
                       }
            self.smeared_hists.fill(icount, weight, data_dic)

    def process(self, file_name):
        out_name = "smeared_hist.root"
        self.read_file(file_name, out_name)
  
    @staticmethod
    def save_plot(file_name):
        file_in = ROOT.TFile.Open(file_name, "read")
        canvas = ROOT.TCanvas("canvas", "canvas", 600, 600)
        for key in file_in.GetListOfKeys():
            h1 = file_in.Get(key.GetName())
            if not h1.InheritsFrom("TH1"): continue
            h1.Draw()
            canvas.SaveAs("eps/"+key.GetName()+".eps")

    @staticmethod
    def compare_all_smeared(file_name):
        file_in = ROOT.TFile.Open(file_name, "read")
        hist_base_names = ["met", "jetpt", "njets", "met_sig", "dphi", "dphiEP"]
        shape_only = True
        for hist_name in hist_base_names:
            h1_name = "all_"+hist_name
            h2_name = "smeared_"+hist_name
            h3_name = "seed_"+hist_name
            h1 = file_in.Get(h1_name)
            h2 = file_in.Get(h2_name)
            h3 = file_in.Get(h3_name)
            h1.SetMarkerSize(0.05)
            h2.SetMarkerSize(0.05)
            h3.SetMarkerSize(0.05)
            hist_tlist = ROOT.TList()
            hist_tlist.Add(h2) 
            hist_tlist.Add(h3) 
            hist_tlist.Add(h1)
            ROOT.compare_hists(hist_tlist, hist_name, shape_only, True, True)
   
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

    def find_right_trigger(self, chain):
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
        else:
            return None
        
    def change_file(self, file_name, start_n):
        chain = ROOT.loader(file_name, "physics")
        nentries = chain.GetEntries()
        
        out_name = "jj_"+str(start_n)+".root"
        outfile = ROOT.TFile.Open(out_name, "recreate")
        #outtree = chain.CloneTree(0)
        outtree = ROOT.TTree("smeared", "smeared")
        met = array('f', [0])
        jetpt = array('f', [0])
        njets = array('i', [0])
        dphi = array('f', [0])
        dphiEP = array('f', [0])
        rmet_pt = array('f', [0])
        run_number = array('i', [0])
        event_number = array('i', [0])
        lb = array('i', [0])
        outtree.Branch('met_et', met, 'met_et/F')
        outtree.Branch('njets', njets, 'njets/I')
        outtree.Branch('leading_jet_pt', jetpt, 'leading_jet_pt/F')
        outtree.Branch('min_dphi', dphi, 'min_dphi/F')
        outtree.Branch("dphi_ep", dphiEP, 'dphi_ep/F')
        outtree.Branch('rmet_pt', rmet_pt, 'rmet_pt/F')
        outtree.Branch('run', run_number, 'run/I')
        outtree.Branch('event', event_number, 'event/I')
        outtree.Branch('lb', lb, 'lb/I')
        weight = array('f', [0])
        outtree.Branch('weight', weight, 'weight/F')

        for ientry in range(1000):
            ientry = start_n + ientry
            if chain.LoadTree(ientry) < 0:
                break
            chain.GetEntry(ientry)
            #select the seed events
            met_sig =  chain.MET_et/1E3/math.sqrt(chain.MET_sumet/1E3)
            run_number[0] = chain.RunNumber
            event_number[0] = chain.EventNumber
            lb[0] = chain.lumiblock
            jet_pt_origin = chain.jet_p4[0].Pt()
            if met_sig < 0.7 and chain.n_base_el ==0 and chain.n_base_mu ==0:
                for data in chain.pseudoData:
                    met[0] = data.met_/1E3
                    jetpt[0] = data.leading_jet_pt_/1E3
                    njets[0] = data.n_good_jets_
                    dphi[0] = data.min_jets_met_
                    dphiEP[0] = data.dphi_EP_
                    rmet_pt[0] = data.met_/data.leading_jet_pt_
                    trigger = self.find_right_trigger(chain)
                    if trigger:
                        weight[0] = self.find_weight(trigger, chain.lumiblock)
                    else:
                        weight[0] = 0.
                    outtree.Fill()
        outfile.cd()
        outtree.Write()
        outfile.Close()

def test_minitree(start_n, data_list, ps_file_name):
    mini = MiniTree()
    mini.load_ps(ps_file_name)
    mini.change_file(data_list, start_n)

if __name__ == "__main__":
    file_name = ""
    ##jetsmear = jetsmearing()
    #jetsmear.compare_all_smeared("smeared_hist.root")
    #jetsmear.process('data_smeared.list')
    #jetsmear.process('reduced_ntup.root')
    threads = []
    ps_file_name = "284484_ps.txt"
    data_list = "../testarea/data_smeared.list"
    if len(sys.argv) > 2:
        ps_file_name = sys.argv[1]
        data_list = sys.argv[2]
    
    print ps_file_name,data_list
    ch = ROOT.loader(data_list, "physics")
    nentries = ch.GetEntries()
    if nentries < 1:
        sys.exit(1)
    total_jobs = nentries/1000+1
    print "total entries: ",nentries," jobs: ",total_jobs
    for i_thread in range(total_jobs):
        start_n = i_thread*1000
        t = threading.Thread(target=test_minitree, args=(start_n, data_list, ps_file_name))
        threads.append(t)
        t.start()
