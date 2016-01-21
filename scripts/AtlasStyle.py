import ROOT
import os
file_dir = os.getenv("MonoJetCodeDir")+"MonoJet/scripts/AtlasStyle.C"
if not hasattr(ROOT, "SetAtlasStyle"):
    ROOT.gROOT.LoadMacro(file_dir)
ROOT.SetAtlasStyle()
