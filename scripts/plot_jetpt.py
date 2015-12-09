#!/usr/bin/env python
# -*- coding: ascii -*-

import ROOT
ROOT.gROOT.SetBatch()
if not hasattr(ROOT, "loader"):
    ROOT.gROOT.LoadMacro("/afs/cern.ch/user/x/xju/tool/loader.c") 


def get_TCut(trig_name):
    out = ""
    icount = 0
    for trig in triggers:
        if icount != 0:
            out += " && "
        cut_name = trig.replace("HLT","trig")
        if trig_name == trig:
            out += cut_name
        else: out += "!"+cut_name
        icount += 1
    return out

def add_hist(hist_list):
    icount = 0
    for hist in hist_list:
        if icount == 0:
            all_hist = hist.Clone("all_hist")
        else:
            all_hist.Add(hist)
        icount += 1
    return all_hist

def plot_jet_pt(file_name):
    tree = ROOT.loader(file_name, "physics")
    hists_low_sig = []
    hists_high_sig = []
    canvas = ROOT.TCanvas("canvas", "canvas", 600, 600)
    sr_cuts = "n_base_el == 0 && n_base_mu == 0"
    for trig,lumi in zip(triggers, lumi_list):
        weight = unprescaled_lumi/lumi
        hist_name = "h_"+trig
        h1 = ROOT.TH1F(hist_name, "h1", 100, 0, 1000)
        cut = get_TCut(trig)+" && "+sr_cuts
        #print weight, hist_name, cut
        tree.Draw("jet_p4[0].Pt()*1E-3>>"+hist_name, str(weight)+"*("+cut+")")
        print h1.Integral()
        hists_low_sig.append(h1)

    all_hist = add_hist(hist_list)
    fout = ROOT.TFile.Open("test.root", "recreate")
    all_hist.Write()
    fout.Close()

if __name__ == "__main__":
    plot_jet_pt("data.list")
