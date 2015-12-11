#!/usr/bin/env python

import ROOT
import AtlasStyle
import os
import math

ROOT.gROOT.SetBatch()
if not hasattr(ROOT, "loader"):
    ROOT.gROOT.LoadMacro("/afs/cern.ch/user/x/xju/tool/loader.c") 

ROOT.gROOT.LoadMacro(os.getenv("ROOTCOREBIN")+"/lib/x86_64-slc6-gcc48-opt/libCxxUtils.so")
ROOT.gROOT.LoadMacro(os.getenv("ROOTCOREBIN")+"/lib/x86_64-slc6-gcc48-opt/libAthContainers.so")
ROOT.gROOT.LoadMacro(os.getenv("ROOTCOREBIN")+"/lib/x86_64-slc6-gcc48-opt/libMonoJet.so")

triggers = ["HLT_j100",  "HLT_j110",  "HLT_j150",  "HLT_j175",  "HLT_j200",
            "HLT_j260",  "HLT_j300",  "HLT_j320",  "HLT_j360"]
# unit: ipb
lumi_list = [979.768*1E-3, 1441.75*1E-3,  5357.85*1E-3, 10698.3*1E-3,
              19701.4*1E-3, 68386.7*1E-3, 135.87, 191.304, 3316.68] 
unprescaled_lumi = 3316.68

class HistMgr:
    def __init__(self, tag_name):
        self.tag = tag_name
        all_list = []
        all_list.append(self.create_hist_tuple("met", 50, 0, 500))
        all_list.append(self.create_hist_tuple("jetpt", 100, 0, 1000))
        all_list.append(self.create_hist_tuple("njets", 11, -0.5, 10.5))
        all_list.append(self.create_hist_tuple("met_sig", 100, 0, 10))
        all_list.append(self.create_hist_tuple("dphi", 64, 0, 3.2))
        all_list.append(self.create_hist_tuple("dphiEP", 64, 0, 3.2))
        self.list_dic = dict(all_list)

    def create_hist_tuple(self, name, nbins, low, high):
        res_list = []
        for trig in triggers:
            hist_name = "h_"+self.tag+"_"+name+"_"+trig
            res_list.append(
                ROOT.TH1F(hist_name, hist_name, nbins, low, high)
            )
        return (name, res_list)

    def fill(self, itrig, weight, met, jet_pt, njets, met_sig, dphi, dphiEP):
        self.met_list[itrig].Fill(met, weight)
        self.jetpt_list[itrig].Fill(jet_pt, weight)
        self.njets_list[itrig].Fill(njets, weight)
        self.met_sig_list[itrig].Fill(met_sig, weight)
        self.dphi_list[itrig].Fill(dphi, weight)
        self.dphi_EP_list[itrig].Fill(dphiEP, weight)
    
    def merged_hists(self):
        hist_list = []
        hist_list.append(self.add_hist(self.met_list, self.tag+"_met"))
        hist_list.append(self.add_hist(self.jetpt_list, self.tag+"_jetpt"))
        hist_list.append(self.add_hist(self.njets_list, self.tag+"_njets"))
        hist_list.append(self.add_hist(self.met_sig_list, self.tag+"_met_sig"))
        hist_list.append(self.add_hist(self.dphi_list, self.tag+"_dphi"))
        hist_list.append(self.add_hist(self.dphi_EP_list, self.tag+"_dphiEP"))
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
            for trig,lumi in zip(triggers, lumi_list):
                weight = unprescaled_lumi/lumi
                if self.pass_trig_only(trig):
                    jet_pt = ch.jet_p4[0].Pt()/1E3;
                    if jet_pt < 150: continue
                    dphi_EP = abs(ROOT.TVector2.Phi_mpi_pi(ch.MET_phi-ch.MET_trk_phi))
                    met_sig = ch.MET_et/1e3/math.sqrt(ch.MET_sumet/1e3)

                    self.all_hists.fill(icount, weight,
                                       ch.MET_et/1e3, jet_pt, ch.n_good_jet,
                                       met_sig,ch.min_dphi_jetMET, dphi_EP)
                    if self.is_seed_events():
                        self.seed_hists.fill(icount, weight,
                                       ch.MET_et/1e3, jet_pt, ch.n_good_jet,
                                       met_sig,ch.min_dphi_jetMET, dphi_EP)
                        self.fill_pseudo_data(icount, weight, met_sig)

        fout = ROOT.TFile.Open(out_name, "recreate")
        for hist in self.all_hists.merged_hists():
            hist.Write()
        for hist in self.seed_hists.merged_hists():
            hist.Write()
        for hist in self.smeared_hists.merged_hists():
            hist.Write()
        fout.Close()

    def fill_pseudo_data(self, icount, weight, met_sig):
        for data in self.chain.pseudoData:
            self.smeared_hists.fill(icount, weight, 
                                    data.met_/1e3, data.leading_jet_pt_/1e3,
                                    data.n_good_jets_, met_sig, 
                                    data.min_jets_met_,data.dphi_EP_)

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

if __name__ == "__main__":
    file_name = ""
    jetsmear = jetsmearing()
    jetsmear.compare_all_smeared("smeared_hist.root")
    #jetsmear.process('data_smeared.list')
    #jetsmear.process('reduced_ntup.root')
