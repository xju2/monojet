from configManager import configMgr
from ROOT import kBlack,kWhite,kGray,kRed,kPink,kMagenta,kViolet,kBlue,kAzure,kCyan,kTeal,kGreen,kSpring,kYellow,kOrange
from configWriter import fitConfig,Measurement,Channel,Sample
from systematic import Systematic

import math
from array import array
import os

# Setup for ATLAS plotting
from ROOT import TH1F
import ROOT
ROOT.gROOT.LoadMacro("/afs/cern.ch/user/x/xju/tool/AtlasStyle.C")
ROOT.SetAtlasStyle()

input_dir =\
        "/afs/cern.ch/user/x/xju/work/monoJet/process/do_minitree/mc15_13TeV_v3/no_dphi/combined_dphi04_met250/"
regions = ["SR","WMUNU","WENU","ZMM"]
sample_names = ["data", "signal", "Znunu", "Wmunu", "Wenu", "Wtaunu", "Zmumu",
                "Zee", "Ztautau", "ttbar", "Diboson", "Dijets"]
#sample_colors = [kBlack,kWhite,kGray,kRed,kPink,kMagenta,
#          kViolet,kBlue,kAzure,kCyan,kTeal,kGreen,
#          kSpring,kYellow,kOrange]
sample_colors = [kBlack, 2, 206, 64, 64, 64, 95, 95, 95, 209, 28, 29] 
weight = 78.7
in_file_path = ""
class SampleHist:

    def __init__(self, cut):
        self.cut = cut

        self.bkg_samples = []
        self.sys_common = []
        self.signal_sample = None 

    def read_hist(self, filename):
        """return yields of the sample in different regions"""
        file_in = ROOT.TFile.Open(filename)
        dic = {}
        for region in regions:
            histname = "met_"+region+"_"+str(self.cut)
            #print histname
            hist = file_in.Get(histname)
            #print type(hist)
            tmp_array = [0, 0]
            errorArray = array('d', tmp_array)
            exp_and_error = hist.IntegralAndError(1, 50, errorArray)
            if math.fabs(exp_and_error) < 1e-6: # fill phantom data
                dic[region] = (1e-6, 1e-6)
            else:
                dic[region] = (exp_and_error, errorArray[0])
        file_in.Close()
        return dic

    def create_common_sys(self):
        sys_list_file = "data/monojet_sys.list"
        with open(sys_list_file, 'r') as f:
            for line in f:
                sys_name = line[:-1]
                systematic = Systematic(sys_name, "_NoSys",
                                        "_up","_down","tree","histoSys")
                self.sys_common.append(systematic)

    def create_samples(self):
        print "total samples:",len(sample_names)
        for index in range(len(sample_names)):
            sample_name = sample_names[index]
            sample = Sample(sample_name, sample_colors[index])
            
            file_name = input_dir+sample_name+"_combined.root"
            yields_dic = self.read_hist(file_name)
            print "Sample:", sample_name+";",
            if "data" in sample_name:
                sample.setData()
                self.data_sample = sample
                for region in regions:
                    nevts, nerror = yields_dic[region]
                    sample.buildHisto([nevts], region, "cuts", 0.5)
                    print region,str(round(nevts,3))+";",
                print
                continue

            sample.setNormByTheory()
            for region in regions:
                nevts, nerror = yields_dic[region]
                nevts *= weight
                nerror *= weight
                if "Dijets" in sample_name and "SR" in region:
                    if self.cut == 10:
                        nevts = 4.07
                        nerror = math.sqrt(nevts)
                    if self.cut == 14:
                        nevts = 2.42
                        nerror = math.sqrt(nevts)
                sample.buildHisto([nevts], region, "cuts", 0.5)
                sample.buildStatErrors([nerror], region, "cuts")
                print region,str(round(nevts,3))+";",
            print
            #sample.setStatConfig(True)
            sample.setFileList([in_file_path])
            ## add systematic??
            sample.addSystematic(Systematic(sample_name+"_stats",\
                        configMgr.weights, 1.2, 0.8, "user", "userOverallSys"))
            #for systematic in self.sys_common:
                #sample.addSystematic(systematic)
            self.set_norm_factor(sample)


    def set_norm_factor(self, sample):
        if "data" in sample.name:
            return
        if "signal" in sample.name:
            sample.setNormFactor("mu", 1., 0., 100.)
            self.signal_sample = sample
            return 
        if sample.name == "Wenu" or sample.name == "Wtaunu":
            sample.setNormFactor("mu_wen", 1., 0., 5.)
            sample.setNormRegions([("WENU", "cuts")])
        elif sample.name == "Wmunu":
            sample.setNormFactor("mu_wmn", 1., 0., 5.)
            sample.setNormRegions([("WMUNU", "cuts")])
        elif sample.name == "Zee" or sample.name == "Zmumu" or\
             sample.name == "Ztautau" or sample.name == "Znunu": 
            sample.setNormFactor("mu_zll", 1., 0., 5.)
            sample.setNormRegions([("ZMM", "cuts")])
        else:
            pass
        self.bkg_samples.append(sample)


class MonoJet:

    def __init__(self):
        print "Hello World"
        self.use_stat = False

    def process(self, cut):
        configMgr.analysisName = "MonoJet_"+str(cut)+"GeV"
        in_file_path = "data/"+configMgr.analysisName+".root"
        configMgr.histBackupCacheFile = "data/"+configMgr.analysisName+"_template.root"
        configMgr.outputFileName = "results/%s_Output.root"%configMgr.analysisName

        # define unnecessary cuts
        configMgr.cutsDict["SR"] = "1."
        configMgr.cutsDict["WMUNU"] = "1."
        configMgr.cutsDict["WENU"] = "1."
        configMgr.cutsDict["ZMM"] = "1."
        configMgr.weights = "1."

        yield_mgr = SampleHist(cut)
        # get systematics
        yield_mgr.create_common_sys()
        # create MC samples
        yield_mgr.create_samples()

        # define background-only
        bkg = configMgr.addFitConfig("BkgOnly")
        if self.use_stat:
            bkg.statErrThreshold = 0.05
        else:
            bkg.statErrThreshold = None
        all_samples = yield_mgr.bkg_samples[:]
        all_samples.append(yield_mgr.data_sample)
        bkg.addSamples(all_samples)
        meas = bkg.addMeasurement(name="Normal", lumi=1.0, lumiErr=0.039)
        meas.addPOI("mu")
        meas.addParamSetting("Lumi", True, 1)
        constraint_chan = []
        for region in regions:
            if "SR" in region:
                continue
            cstr_ch = bkg.addChannel("cuts", [region], 1, 0.5,1.5)
            cstr_ch.minY = 1e-4
            cstr_ch.maxY = 400
            constraint_chan.append(cstr_ch)
        bkg.setBkgConstrainChannels(constraint_chan)
        
        #Discovery 
        if myFitType == FitType.Discovery:
            discovery = configMgr.addFitConfigClone(bkg, "Discovery")
            discovery.addSamples(yield_mgr.signal_sample)
            discovery.setSignalSample(yield_mgr.signal_sample)
            sig_channel = discovery.addChannel("cuts",["SR"], 1, 0.5, 1.5)
            sig_channel.minY = 1e-4
            sig_channel.maxY = 2500
            discovery.setSignalChannels([sig_channel])

        if myFitType == FitType.Exclusion:
            print "In exclusion mode"
            exclusion = configMgr.addFitConfigClone(bkg, "Exclusion")
            exclusion.addSamples(yield_mgr.signal_sample)
            exclusion.setSignalSample(yield_mgr.signal_sample)
            sig_channel = exclusion.addChannel("cuts",["SR"], 1, 0.5, 1.5)
            sig_channel.minY = 1e-4
            sig_channel.maxY = 2500
            exclusion.setSignalChannels([sig_channel])

        if configMgr.executeHistFactory:
            pass
            #if os.path.isfile("data/%s.root"%configMgr.analysisName):
                #os.remove("data/%s.root"%configMgr.analysisName) 


monojet = MonoJet()
# Setting the parameters of the hypothesis test
configMgr.calculatorType = 2 # 2=asymptotic calculator, 0=frequentist calculator
configMgr.writeXML = True
#configMgr.doExclusion = True # True=exclusion, False=discovery
# configMgr.blindSR = False 
# configMgr.blindCR = False 
# configMgr.blindVR = False 
# configMgr.nTOYs = -1
# configMgr.lumiUnits = 1.0 # 1=fb-1, 1000=pb-1
# configMgr.muValGen = 0.0 # mu_sig used for toy generation
# configMgr.useAsimovSet = False # Use the Asimov dataset
#configMgr.doHypoTest = True

configMgr.useCacheToTreeFallback = False # enable fallback to trees
configMgr.useHistBackupCacheFile = True # enable use of an alternate data file

monojet.process(14)
