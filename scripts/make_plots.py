#!/usr/bin/env python

import ROOT
import AtlasStyle
import PyROOTUtils

import sys
import os

ROOT.gROOT.SetBatch()
if not hasattr(ROOT, "loader"):
    ROOT.gROOT.LoadMacro("/afs/cern.ch/user/x/xju/tool/loader.c") 

if True:
    seed_cut = ROOT.TCut("weight*(met_sig<0.7 && leading_jet_pt > 150 && abs(leading_jet_eta) < 2.4)")
    no_cut = ROOT.TCut("weight*(leading_jet_pt > 150 && abs(leading_jet_eta) < 2.4)")
    smeared_cut = ROOT.TCut("weight*(leading_jet_pt > 150 && abs(leading_jet_eta) < 2.4)")
else:
    seed_cut = ROOT.TCut("(met_sig<0.7 && leading_jet_pt > 150 && abs(leading_jet_eta) < 2.4)")
    no_cut = ROOT.TCut("(leading_jet_pt > 150 && abs(leading_jet_eta) < 2.4)")
    smeared_cut = ROOT.TCut("(leading_jet_pt > 150 && abs(leading_jet_eta) < 2.4)")

def plot_smeared_data( file_name, br_name, title, hist_name, nbins, low, hi ):
    canvas = ROOT.TCanvas("canvas", "canvas", 600, 600)
    ch_seed = ROOT.loader(file_name, "seed")
    ch_smeared = ROOT.loader(file_name, "smeared")
    jet_pt_seed = ROOT.draw_hist_from_chain(ch_seed, br_name,\
                                            seed_cut, hist_name+"_seed", 
                                            nbins, low, hi)
    jet_pt_title = title
    jet_pt_seed.SetXTitle(jet_pt_title)
    jet_pt_all = ROOT.draw_hist_from_chain(ch_seed, br_name,\
                                            no_cut, hist_name+"_all", 
                                          nbins, low, hi)
    jet_pt_smeared = ROOT.draw_hist_from_chain(ch_smeared, br_name,\
                                            no_cut, hist_name+"_smeared",
                                               nbins, low, hi)
    pt_scale = jet_pt_all.Integral()/jet_pt_seed.Integral()
    scale2 = jet_pt_all.Integral()/jet_pt_smeared.Integral()
    jet_pt_seed.Scale(pt_scale)
    jet_pt_smeared.Scale(scale2)
    jet_pt_all.Scale(1.0)

    jet_pt_seed.SetLineColor(4)
    jet_pt_smeared.SetLineColor(2)

    marker_size = 0.5
    jet_pt_all.SetMarkerSize(marker_size)
    jet_pt_seed.SetMarkerSize(marker_size)
    jet_pt_smeared.SetMarkerSize(marker_size)

    jet_pt_seed.SetMarkerColor(4)
    jet_pt_smeared.SetMarkerColor(2)

    bkg_list = ROOT.TList()
    bkg_list.Add(jet_pt_seed)
    bkg_list.Add(jet_pt_smeared)
    pad = ROOT.add_ratio_pad(jet_pt_all, bkg_list)
    pad.SetLogy()
    min_y = ROOT.GetMinOfHist(jet_pt_seed)*0.1
    max_y = jet_pt_seed.GetMaximum() * 50
    jet_pt_seed.GetYaxis().SetRangeUser(min_y, max_y)

    jet_pt_seed.Draw("EP")
    jet_pt_all.Draw("same EP")
    jet_pt_smeared.Draw("same EP")

    legend = ROOT.myLegend(0.65, 0.7, 0.85, 0.9)
    legend.AddEntry(jet_pt_all, "all events", 'f')
    legend.AddEntry(jet_pt_seed, "seed events", 'f')
    legend.AddEntry(jet_pt_smeared, "smeared events", 'f')
    legend.Draw()

    base_name = os.path.basename(file_name)
    tag = "data_susy11_jrf_p1886_v4.list"
    canvas.SaveAs(base_name.replace(tag, hist_name)+"_log.pdf")
    
    outfile = ROOT.TFile.Open(base_name.replace(tag,"hist"), 'update')
    jet_pt_all.Write()
    jet_pt_smeared.Write()
    jet_pt_seed.Write()
    outfile.Close()


class HistMaker:
    """
        make histograms for the QCD in CR and SR
    """
    def __init__(self, file_name):
        self.chain = ROOT.loader(file_name, "smeared")
        self.cut = ROOT.TCut("weight*(met_et > 250 && leading_jet_pt > 250 \
                             && njets < 5 && min_dphi < 0.4)")

    def make_hist2(self, br_name, hist_name, bin_list):
        return PyROOTUtils.draw_hist_from_chain(self.chain, br_name,\
                                                self.cut, hist_name, bin_list)

    def make_hist(self, br_name, hist_name, nbins, low, hi):
        return ROOT.draw_hist_from_chain(self.chain, br_name,\
                                         self.cut, hist_name, nbins, low, hi)
    
    def process(self, out_name):
        canvas = ROOT.TCanvas("canvas", "canvas", 600, 600)
        fout = ROOT.TFile.Open(out_name, "recreate")
        met_xbins = [50, 100, 125, 150, 175, 
                 200, 225, 250, 275, 300, 350, 400,
                 450, 500, 600, 800, 1000]
        region = "SR"
        tag = 21
        appendix = region+"_"+str(tag)
        jet_pt = self.make_hist2("leading_jet_pt", "ljetPt_"+appendix, met_xbins)
        met = self.make_hist2("met_et", "met_"+appendix, met_xbins)
        njets = self.make_hist("njets", "njets_"+appendix, 11, -0.5, 10.5) 
        dphi = self.make_hist("min_dphi", "dphi_"+appendix, 64, 0, 3.2)
        fout.cd()
        jet_pt.Write()
        met.Write()
        njets.Write()
        dphi.Write()
        fout.Close()


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print sys.argv[0]," file_name"
        sys.exit(0)
    else:
        file_name = sys.argv[1]
    #plot_smeared_jet_pt(file_name)
    plot_smeared_data(file_name, "leading_jet_pt", "leading jet p_{t}",
                      "jet_pt", 150, 0, 1500)
    plot_smeared_data(file_name, "njets", "N_{jets}", "njets", 11, -0.5, 10.5)
    plot_smeared_data(file_name, "met_et", "E_{T}^{miss}",
                     "met_et", 150, 0, 1500)

    #hist = HistMaker(file_name)
    #hist.process("Dijets_combined.root")
