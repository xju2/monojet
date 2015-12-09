#!/usr/bin/env python
"""
It deals with the root file created by CountSingleProcess in StackProcess
In inputlist each line is: name_of_the_sample path_to_the_sample
"""
import ROOT
import os
#import numpy as np
import array
import sys
import math
sample_list = ["data", "signal", "bkg", "Znunu", 
               "Wmunu", "Wenu", "Wtaunu", "Zmumu",
               "Zee", "Ztautau", "ttbar", "Diboson"]
regions = ["SR","WMUNU","WENU","ZMM","ZEE"]
#cuts = [1, 20, 250, 400, 600]
cuts = [10]
data_dic = {276262: 6.50, 276329: 12.38, 276336: 0.69, 276416: 4.39,
            276511: 9.35, 276689: 11.28, 
            #276731: 25.75, 
            276778: 0.70,
            276790: 1.66, 276952: 6.9,  276954: 0.77,  278880: 22.68,
            278912: 20.74, 278968: 10.34, 279169: 57.77, 279259: 7.62,
            279279: 17.79, 279284: 33.33, 279345: 54.92, 279515: 0.362, 
            279598: 71.41, 279685: 78.25,
            279764: 2.49, 279813: 50.38, 279867: 31.26, 279928: 1.06,
            279932: 46.91, 279984: 68.74, 280231: 93.25, 280319: 98.6,
            280368: 8.74, 280423: 72.33, 280464: 61.51, 280500: 7.72,
            280520: 12.88, 280614: 25.77,  #G2
            280673: 156.80, 280753: 47.3258657, 280853: 7.4278,
            280862: 137.813, 280950: 131.31, 280977: 36.415, 
            281070: 12.886, 281074: 47.97, 281075:0.8848, 281317: 24.858,
            281385: 96.89,
            #"all": 1037.23
            281411: 141.728, 282625: 10.89, 282631: 27.41, 282712: 96.13,
            282784: 3.04, 282992: 112.00,
            283074: 58.84,
            283155: 30.23,
            283270: 13.89,
            283429: 243.00,
            283608: 24.90,
            283780: 151.30,
            284006: 40.56,
            284154: 10.44,
            284213: 207.60,
            284285: 255.80,
            284420: 58.78,
            284427: 36.13,
            284484: 107.10,
            "all": 3342.00
           }
#hist_location = "combined_523_met250_withJetC/"
hist_location = "combined/"
class Yield:
    def __init__(self):
        self.gr_y = {}
        self.gr_x = {}

    def readHist(self, filename):
        #print filename
        file_in = ROOT.TFile.Open(filename)
        if not file_in or file_in.IsZombie():
            return
        dic = {}
        for cut in cuts:
            for region in regions:
                histname = "met_"+region+"_"+str(cut)
                hist = file_in.Get(histname)
                if not hist or "TH1" not in str(type(hist)):
                    print histname," not exist"
                    continue
                arr_x = [0., 0., 0.]
                errorArray = array.array('d', arr_x)
                nxbins = hist.GetXaxis().GetNbins()
                if "data" in filename:
                    totalN = hist.IntegralAndError(1,nxbins, errorArray)
                else:
                    totalN = hist.IntegralAndError(1,nxbins, errorArray)*self.lumi
                    errorArray[0] *= self.lumi
                dic[histname] = (totalN, errorArray[0])
        return dic

    def get(self, data_key):
        self.lumi = data_dic[data_key]
        print "data: ", data_key, "total lumi: ", self.lumi
        sample_yield_dic = {}
        for sample in sample_list:
            file_name = hist_location+"/"+sample+"_combined.root"
            if "data" in sample:
                if data_key != "all":
                    file_name = "data/data_"+str(data_key)+".root"
                else:
                    file_name = "data/combined.root"
            res = self.readHist(file_name)
            if res is None: return
            sample_yield_dic[sample] = res
        out = ""
        out += str(data_key) + " " + str(self.lumi) + "ipb\n"
        all_ratios = []
        for cut in cuts:
            out += "\\multicolumn{"+str(len(sample_list))+\
                "}{c}{MET $>$ "+str(cut)+" GeV} \\\\ \\hline \n"
            out += "\t"
            for sample in sample_list:
                out += "\t & "+ sample
            out += "\\\\ \\hline \n"
            ratio = {}
            for region in regions:
                out += region
                histname = "met_"+region+"_"+str(cut)
                
                for sample in sample_list:
                    expected,error  = sample_yield_dic[sample][histname]
                    out += "\t & %.3f $\\pm$ %.4f"%(expected, error)
                    if sample == "data": n_data = expected
                    if sample == "bkg": n_bkg = expected
                r_v = n_data*1.0/n_bkg
                if n_data == 0 or n_bkg == 0: r_e = 0
                else: r_e = r_v*math.sqrt((n_data+n_bkg)/n_data/n_bkg)
                ratio[region] = (data_key, r_v, r_e)
                out += " \\\\ \n"
            all_ratios.append(ratio)
            out += "\n"
        out_name = "yields.txt"
        with open(out_name,'a') as f:
            f.write(out)
        print out_name," is written"
        return all_ratios

def draw_ratio_vs_channel():
    yields = Yield()
    all_info = []
    ROOT.gROOT.SetBatch(True)
    for data_key in sorted(data_dic.keys()):
        if data_key == "all": continue
        all_info.append(yields.get(data_key))

    for region in regions:
        h1 = ROOT.TH1F("h1"+region, "h1", len(all_info)+1, -0.5, len(all_info)+0.5);
        ibin = 1
        h_lumi = ROOT.TH1F("h_lumi"+region, "h1", len(all_info)+1, -0.5, len(all_info)+0.5);
        for info in all_info:
            x_value,y_value,y_error = info[0][region]
            h1.SetBinContent(ibin, y_value)
            h1.SetBinError(ibin, y_error)
            h1.GetXaxis().SetBinLabel(ibin, str(x_value))
            h_lumi.SetBinContent(ibin, data_dic[x_value])
            ibin += 1
        canvas = ROOT.TCanvas("canvas"+region, "canvas", 600, 600)
        h1.LabelsOption("v")
        h1.Draw()
        right_max = h_lumi.GetMaximum()
        #scale = h1.GetYaxis().GetXmax()/right_max
        #scale = ROOT.gPad.GetUymax()/right_max
        scale = h1.GetMaximum()/right_max
        h_lumi.SetLineColor(2)
        h_lumi.Scale(scale)
        h_lumi.Draw("HIST SAME")
        new_axis = ROOT.TGaxis(h_lumi.GetXaxis().GetXmax(),
                               h_lumi.GetYaxis().GetXmin(),
                               h_lumi.GetXaxis().GetXmax(),
                               h_lumi.GetYaxis().GetXmax(),
                               0, right_max, h1.GetNdivisions("Y"),"+L")
        new_axis.SetLineColor(2)
        new_axis.Draw()
        canvas.SaveAs("test_ratio_"+region+".eps")

def get_all():
    yields = Yield()
    ROOT.gROOT.SetBatch(True)
    yields.get("all")

if __name__ == '__main__':
    #draw_ratio_vs_channel()
    get_all()
