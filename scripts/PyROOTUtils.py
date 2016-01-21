#!/usr/bin/env python

__author__ = "Xiangyang Ju"
__version__ = "0.1"
__doc__ = """
"""

import ROOT
from array import array

def draw_hist_from_chain(chain, br_name, cut, hist_name, bin_list):
    nbins = len(bin_list) - 1
    h1 = ROOT.TH1F(hist_name, hist_name, nbins, array('f', bin_list))
    chain.Draw(br_name+">>"+hist_name, cut)
    h1.SetDirectory(0)
    return h1
