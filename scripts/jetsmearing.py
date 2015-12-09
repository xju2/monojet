#!/usr/bin/env python

import ROOT
import AtlasStyle
import os
import math

ROOT.gROOT.SetBatch()
if not hasattr(ROOT, "loader"):
    ROOT.gROOT.LoadMacro("/afs/cern.ch/user/x/xju/tool/loader.c") 

ROOT.gSystem.Load(os.getenv("MonoJetCodeDir")+"RootCoreBin/lib/x86_64-slc6-gcc48-opt/libWiscAnalysis.so")
triggers = ["HLT_j100",  "HLT_j110",  "HLT_j150",  "HLT_j175",  "HLT_j200",
            "HLT_j260",  "HLT_j300",  "HLT_j320",  "HLT_j360"]
# unit: ipb
lumi_list = [979.768*1E-3, 1441.75*1E-3,  5357.85*1E-3, 10698.3*1E-3,
              19701.4*1E-3, 68386.7*1E-3, 135.87, 191.304, 3316.68] 
unprescaled_lumi = 3316.68

class jetsmearing:
    def __init__(self):
        print "Using Jet Smearing"
        self.seed_met_sig_list = []
        self.seed_met_list = []
        self.seed_jetpt_list = []
        self.seed_njets_list = []
        self.seed_dphi_list = []

        self.met_sig_list = []
        self.met_list = []
        self.jetpt_list = []
        self.njets_list = []
        self.dphi_list = []
        for trig in triggers:
            self.seed_met_list.append(
                ROOT.TH1F("h_seed_met_"+trig, "E_{T}^{miss} GeV;Seed MET",50, 0, 500)
            )
            self.seed_met_sig_list.append(
                ROOT.TH1F("h_seed_met_sig_"+trig, "S;MET significance", 100, 0, 10)
            )
            self.seed_jetpt_list.append(
                ROOT.TH1F("h_seed_jetpt_"+trig,"", 100, 0, 1000)
            )
            self.seed_njets_list.append(
                ROOT.TH1F("h_seed_njets_"+trig, " ", 10, 0, 10)
            )
            self.seed_dphi_list.append(
                ROOT.TH1F("h_seed_dphi_"+trig, " ", 64, 0, 3.2)
            )

            self.met_list.append(
                ROOT.TH1F("h_met_"+trig, "E_{T}^{miss} GeV;Seed MET",50, 0, 500)
            )
            self.met_sig_list.append(
                ROOT.TH1F("h_met_sig_"+trig, "S;MET significance", 100, 0, 10)
            )
            self.jetpt_list.append(
                ROOT.TH1F("h_jetpt_"+trig,"", 100, 0, 1000)
            )
            self.njets_list.append(
                ROOT.TH1F("h_njets_"+trig, " ", 10, 0, 10)
            )
            self.dphi_list.append(
                ROOT.TH1F("h_dphi_"+trig, " ", 64, 0, 3.2)
            )

        self.smeared_met = ROOT.TH1F("smeared_met", "Smeared MET", 200, 0, 2000)
        self.smeared_jetpt = ROOT.TH1F("smeared_jetpt", "Smeared leading jet pt", 200, 0, 2000)
        self.smeared_dphi = ROOT.TH1F("smeared_dphi", "Smeared dphi ", 64, 0, 3.2)
        self.smeared_njets = ROOT.TH1F("smeared_njets", "Smeared njets", 10, 0, 10)
        self.smeared_dphiEP = ROOT.TH1F("smeared_dphiEP", "Smeared dphi(MET,MPT)", 
                                        64, 0, 32)

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
            #self.fill_pseudo_data()
            icount = 0
            for trig,lumi in zip(triggers, lumi_list):
                weight = unprescaled_lumi/lumi
                if self.pass_trig_only(trig):
                    jet_pt = ch.jet_p4[0].Pt()/1E3;
                    if jet_pt < 150: continue
                    met_sig = ch.MET_et/1e3/math.sqrt(ch.MET_sumet/1e3)
                    self.met_list[icount].Fill(ch.MET_et/1e3, weight)
                    self.jetpt_list[icount].Fill(jet_pt, weight)
                    self.njets_list[icount].Fill(ch.n_good_jet, weight)
                    self.met_sig_list[icount].Fill(met_sig, weight)
                    self.dphi_list[icount].Fill(ch.min_dphi_jetMET, weight)
                    if self.is_seed_events():
                        self.seed_met_list[icount].Fill(ch.MET_et/1e3, weight)
                        self.seed_jetpt_list[icount].Fill(jet_pt, weight)
                        self.seed_njets_list[icount].Fill(ch.n_good_jet, weight)
                        self.seed_met_sig_list[icount].Fill(met_sig, weight)
                        self.seed_dphi_list[icount].Fill(ch.min_dphi_jetMET, weight)
        fout = ROOT.TFile.Open(out_name, "recreate")
        self.add_hist(self.seed_met_list, "seed_met").Write()
        self.add_hist(self.seed_jetpt_list, "seed_jetpt").Write()
        self.add_hist(self.seed_njets_list, "seed_njets").Write()
        self.add_hist(self.seed_met_sig_list, "seed_met_sig").Write()
        self.add_hist(self.seed_dphi_list, "seed_dphi").Write()
        self.add_hist(self.met_list, "met").Write()
        self.add_hist(self.jetpt_list, "jetpt").Write()
        self.add_hist(self.njets_list, "njets").Write()
        self.add_hist(self.met_sig_list, "met_sig").Write()
        self.add_hist(self.dphi_list, "dphi").Write()
        self.smeared_met.Write()
        self.smeared_jetpt.Write()
        self.smeared_dphi.Write()
        self.smeared_njets.Write()
        self.smeared_dphiEP.Write()
        fout.Close()

    def fill_pseudo_data(self):
        for data in self.chain.pseudoData:
            self.smeared_met.Fill(data.met_/1e3)
            self.smeared_jetpt.Fill(data.leading_jet_pt_/1e3)
            self.smeared_dphi.Fill(data.min_jets_met_)
            self.smeared_njets.Fill(data.n_good_jets_)
            self.smeared_dphiEP.Fill(data.dphi_EP_)

    def save_hist(self, out_name):
        file_out = ROOT.TFile.Open(out_name, "RECREATE")
        for key, value in self.__dict__.items():
            if value.InheritsFrom("TH1"): value.Write()
        file_out.Close()

    def process(self, file_name):
        out_name = "smeared_hist.root"
        self.read_file(file_name, out_name)
   
def save_plot(file_name):
    file_in = ROOT.TFile.Open(file_name, "read")
    canvas = ROOT.TCanvas("canvas", "canvas", 600, 600)
    for key in file_in.GetListOfKeys():
        h1 = file_in.Get(key.GetName())
        if not h1.InheritsFrom("TH1"): continue
        h1.Draw()
        canvas.SaveAs("eps/"+key.GetName()+".eps")

if __name__ == "__main__":
    file_name = ""
    jetsmear = jetsmearing()
    jetsmear.process('data.list')
    #jetsmear.process('reduced_ntup.root')
