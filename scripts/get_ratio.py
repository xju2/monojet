#!/usr/bin/python
import ROOT
import math

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
def get_ratio_error(a, b):
    ratio = a*1.0/b
    if a==0 or b==0: error = 0
    else: error = ratio * math.sqrt((a+b)/a/b)
    return (ratio, error)

def get_ratio(data_key):
    file1 = ROOT.TFile.Open("data_withJetClean/data_"+str(data_key)+".root","read")
    file2 = ROOT.TFile.Open("data/data_"+str(data_key)+".root","read")
    h1 = file1.Get("met_SR_10")
    h2 = file2.Get("met_SR_10")
    #ratio,error = get_ratio_error(h1.Integral(),h2.Integral())
    ratio,error = get_ratio_error(h2.Integral()-h1.Integral(),h2.Integral())
    file1.Close()
    file2.Close()
    return (ratio,error)

if __name__ == '__main__':

    ROOT.gROOT.SetBatch(True)
    h_final = ROOT.TH1F("h_final", "BIB fraction", 
                        len(data_dic.keys())+1, -0.5, 
                        len(data_dic.keys())+0.5);
    ibin = 1
    for data_key in sorted(data_dic.keys()):
        if data_key == "all": continue
        ratio,error = get_ratio(data_key)
        h_final.SetBinContent(ibin, ratio)
        h_final.SetBinError(ibin, error)
        h_final.GetXaxis().SetBinLabel(ibin, str(data_key))
        ibin += 1
    canvas = ROOT.TCanvas("canvas", "canvas", 600, 600)
    h_final.LabelsOption("v")
    h_final.Draw()
    canvas.SaveAs("test_jetclean_bib.eps")
