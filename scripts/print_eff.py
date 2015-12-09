#!/usr/bin/env python
import ROOT
import glob
hist_name = "dphiEP_SR_10"
def print_eff(file_name):
    in_file = ROOT.TFile.Open(file_name,"READ")
    h1 = in_file.Get(hist_name)
    cut = 2.0
    ntotal = h1.Integral()
    cut_bin = h1.FindBin(cut)
    nsub = h1.Integral(1,cut_bin) 
    ratio = nsub*1.0/ntotal
    #print file_name,"{:.4f}".format(ratio)
    in_file.Close()
    return ratio

if __name__ == "__main__":
    file_lists = glob.glob("*.root")
    ratio_list = []
    for file_ in file_lists:
        ratio_list.append(print_eff(file_))
    print "(min, max)",min(ratio_list), max(ratio_list)
