#!/usr/bin/python
import ROOT
from ROOT import TH1, TChain

import sys
import numpy as np

from analyse_mini_tree import Analizer
class CutFlow:
    def __init__(self, filename):
        print "Cut Flow is created"
        self.file_name = filename
        self.total_events_dic = {}

    def get_total_events_dic(self):
        chain = TChain("associate","associate")
        mc_channel_number = np.zeros(1, dtype=int32)
        mcweight = np.zeros(1, dtype=float64)
        chain.SetBranchAddress("mc_channel_number", mc_channel_number)
        chain.SetBranchAddress("MCWeight", mcweight)

    
    def print_from_hist(self, hist_name):
        file = ROOT.TFile.Open(self.file_name)
        hist_cut_flow =  file.Get(hist_name)
        for i in range(hist_cut_flow.GetNbinsX()):
            label = hist_cut_flow.GetXaxis().GetBinLabel(i+1)
            if label != "":
                print label,hist_cut_flow.GetBinContent(i+1)

    def print_from_tree(self):
        analizer = Analizer(5000.)
        analizer.load_xsection()

if __name__ == '__main__':
    if len(sys.argv) < 3:
        print sys.argv[0]," input histname"
        exit(1)
    cutflow = CutFlow(sys.argv[1])
    cutflow.print_from_hist(sys.argv[2])
