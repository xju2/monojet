//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Thu Nov 12 01:19:12 2015 by ROOT version 6.02/12
// from TTree physics/physics
// found on file: reduced_ntup.root
//////////////////////////////////////////////////////////

#ifndef physics_h
#define physics_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TLorentzVector.h>

// Header file for the classes stored in the TTree if any.
#include <vector>

using namespace std;
class physics {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   Int_t           RunNumber;
   Int_t           EventNumber;
   Int_t           mc_channel_number;
   Float_t         MCWeight;
   Float_t         PUWeight;
   Float_t         actualIPC;
   Float_t         averageIPC;
   UInt_t          bcid;
   UInt_t          lumiblock;
   Int_t           n_good_trk;
   Bool_t          trig_e120_lhloose;
   Bool_t          trig_e17_lhloose_L1EM15;
   Bool_t          trig_e24_lhmedium_L1EM18VH;
   Bool_t          trig_e24_lhmedium_iloose_L1EM18VH;
   Bool_t          trig_e24_lhmedium_iloose_L1EM20VH;
   Bool_t          trig_e24_lhtight_iloose;
   Bool_t          trig_e24_lhtight_iloose_L1EM20VH;
   Bool_t          trig_e60_lhmedium;
   Bool_t          trig_g140_loose;
   Bool_t          trig_j100;
   Bool_t          trig_j100_xe80_dphi10;
   Bool_t          trig_j110;
   Bool_t          trig_j150;
   Bool_t          trig_j150_xe90;
   Bool_t          trig_j175;
   Bool_t          trig_j200;
   Bool_t          trig_j200_xe80;
   Bool_t          trig_j260;
   Bool_t          trig_j300;
   Bool_t          trig_j320;
   Bool_t          trig_j360;
   Bool_t          trig_j380;
   Bool_t          trig_j400;
   Bool_t          trig_j420;
   Bool_t          trig_j440;
   Bool_t          trig_j460;
   Bool_t          trig_mu20_iloose_L1MU15;
   Bool_t          trig_mu26_imedium;
   Bool_t          trig_mu40;
   Bool_t          trig_mu50;
   Bool_t          trig_xe100;
   Bool_t          trig_xe70;
   Bool_t          trig_xe80;
   vector<TLorentzVector> *jet_p4;
   vector<float>   *jet_dphi_MET;
   vector<float>   *jet_jvf0;
   vector<float>   *jet_jvf1;
   vector<float>   *jet_jvt;
   Int_t           n_base_jet;
   Int_t           n_good_jet;
   Int_t           n_jet_btagged;
   Float_t         Ht;
   Float_t         HtV;
   Int_t           n_base_el;
   Int_t           n_good_el;
   Int_t           n_good_iso_el;
   Double_t        mt_elnu;
   Double_t        m_elel;
   Double_t        px_zee;
   Double_t        py_zee;
   vector<TLorentzVector> *el_p4;
   vector<bool>    *el_is_good;
   vector<bool>    *el_medium_LLH;
   vector<bool>    *el_iso_trackonly;
   vector<float>   *el_d0;
   vector<float>   *el_z0;
   vector<float>   *el_zp;
   vector<float>   *el_charge;
   vector<float>   *el_sf;
   vector<float>   *el_sf_up;
   vector<float>   *el_sf_down;
   Int_t           n_base_mu;
   Int_t           n_good_mu;
   Double_t        mt_munu;
   Double_t        m_mumu;
   vector<TLorentzVector> *mu_p4;
   vector<bool>    *mu_is_good;
   vector<bool>    *mu_iso_trackonly;
   vector<float>   *mu_d0;
   vector<float>   *mu_z0;
   vector<float>   *mu_zp;
   vector<float>   *mu_charge;
   vector<float>   *mu_sf;
   vector<float>   *mu_stat_up;
   vector<float>   *mu_stat_down;
   vector<float>   *mu_sys_up;
   vector<float>   *mu_sys_down;
   vector<float>   *mu_trigger_up;
   vector<float>   *mu_trigger_down;
   Double_t        MET_etx;
   Double_t        MET_ety;
   Double_t        MET_phi;
   Double_t        MET_et;
   Double_t        MET_ecorr;
   Double_t        MET_sumet;
   Double_t        MET_mu_etx;
   Double_t        MET_mu_ety;
   Double_t        MET_mu_sumet;
   Double_t        MET_ph_etx;
   Double_t        MET_ph_ety;
   Double_t        MET_ph_sumet;
   Double_t        MET_trk_etx;
   Double_t        MET_trk_ety;
   Double_t        MET_trk_phi;
   Double_t        MET_trk_sumet;
   Double_t        MET_noEM_et;
   Double_t        MET_noEM_phi;
   Double_t        MET_noEM_sumet;
   Float_t         min_dphi_jetMET;
   Float_t         min_dphi_jetMET_zee;
   Float_t         max_dphi_jetjet;
   Float_t         average_dphi_jetjet;
   Float_t         average_dphi_jetjet_now;
   Float_t         max_dr_jetjet;
   Float_t         average_dr_jetjet;
   Float_t         average_dr_jetjet_now;
   Bool_t          flag_dphi_jetMET;
   Bool_t          flag_el_veto;
   Bool_t          flag_mu_veto;
   Bool_t          flag_trk_veto;
   Bool_t          flag_4thjet_veto;
   vector<float>   *track_sum_pt;
   vector<int>     *track_n;
   vector<float>   *track_sum_px;
   vector<float>   *track_sum_py;
   vector<float>   *track_sum_phi;
   vector<float>   *track_pt;
   vector<float>   *track_eta;
   vector<float>   *track_phi;
   vector<float>   *track_theta;
   vector<float>   *track_d0;
   vector<float>   *track_z0;
   vector<float>   *track_beamspot_z0;
   vector<float>   *track_match_vz;
   vector<bool>    *track_LooseP;
   vector<bool>    *track_TightP;
   Int_t           n_base_ph;
   vector<bool>    *ph_is_tight;
   vector<bool>    *ph_is_medium;
   vector<float>   *ph_topoetcone40;
   vector<TLorentzVector> *ph_p4;
   vector<float>   *ph_etaBE;
   vector<float>   *jet_emf;
   vector<float>   *jet_hecf;
   vector<float>   *jet_LArQ;
   vector<float>   *jet_hecQ;
   vector<float>   *jet_sumpt_trk;
   vector<float>   *jet_fracSamplingMax;
   vector<float>   *jet_negE;
   vector<float>   *jet_avgLArQF;
   vector<int>     *jet_fracSamplingMaxIndex;
   vector<bool>    *jet_isBadTight;
   vector<float>   *jet_timing;

   // List of branches
   TBranch        *b_RunNumber;   //!
   TBranch        *b_EventNumber;   //!
   TBranch        *b_mc_channel_number;   //!
   TBranch        *b_MCWeight;   //!
   TBranch        *b_PUWeight;   //!
   TBranch        *b_actualIPC;   //!
   TBranch        *b_averageIPC;   //!
   TBranch        *b_bcid;   //!
   TBranch        *b_lumiblock;   //!
   TBranch        *b_n_good_trk;   //!
   TBranch        *b_trig_e120_lhloose;   //!
   TBranch        *b_trig_e17_lhloose_L1EM15;   //!
   TBranch        *b_trig_e24_lhmedium_L1EM18VH;   //!
   TBranch        *b_trig_e24_lhmedium_iloose_L1EM18VH;   //!
   TBranch        *b_trig_e24_lhmedium_iloose_L1EM20VH;   //!
   TBranch        *b_trig_e24_lhtight_iloose;   //!
   TBranch        *b_trig_e24_lhtight_iloose_L1EM20VH;   //!
   TBranch        *b_trig_e60_lhmedium;   //!
   TBranch        *b_trig_g140_loose;   //!
   TBranch        *b_trig_j100;   //!
   TBranch        *b_trig_j100_xe80_dphi10;   //!
   TBranch        *b_trig_j110;   //!
   TBranch        *b_trig_j150;   //!
   TBranch        *b_trig_j150_xe90;   //!
   TBranch        *b_trig_j175;   //!
   TBranch        *b_trig_j200;   //!
   TBranch        *b_trig_j200_xe80;   //!
   TBranch        *b_trig_j260;   //!
   TBranch        *b_trig_j300;   //!
   TBranch        *b_trig_j320;   //!
   TBranch        *b_trig_j360;   //!
   TBranch        *b_trig_j380;   //!
   TBranch        *b_trig_j400;   //!
   TBranch        *b_trig_j420;   //!
   TBranch        *b_trig_j440;   //!
   TBranch        *b_trig_j460;   //!
   TBranch        *b_trig_mu20_iloose_L1MU15;   //!
   TBranch        *b_trig_mu26_imedium;   //!
   TBranch        *b_trig_mu40;   //!
   TBranch        *b_trig_mu50;   //!
   TBranch        *b_trig_xe100;   //!
   TBranch        *b_trig_xe70;   //!
   TBranch        *b_trig_xe80;   //!
   TBranch        *b_jet_p4;   //!
   TBranch        *b_jet_dphi_MET;   //!
   TBranch        *b_jet_jvf0;   //!
   TBranch        *b_jet_jvf1;   //!
   TBranch        *b_jet_jvt;   //!
   TBranch        *b_n_base_jet;   //!
   TBranch        *b_n_good_jet;   //!
   TBranch        *b_n_jet_btagged;   //!
   TBranch        *b_Ht;   //!
   TBranch        *b_HtV;   //!
   TBranch        *b_n_base_el;   //!
   TBranch        *b_n_good_el;   //!
   TBranch        *b_n_good_iso_el;   //!
   TBranch        *b_mt_elnu;   //!
   TBranch        *b_m_elel;   //!
   TBranch        *b_px_zee;   //!
   TBranch        *b_py_zee;   //!
   TBranch        *b_el_p4;   //!
   TBranch        *b_el_is_good;   //!
   TBranch        *b_el_medium_LLH;   //!
   TBranch        *b_el_iso_trackonly;   //!
   TBranch        *b_el_d0;   //!
   TBranch        *b_el_z0;   //!
   TBranch        *b_el_zp;   //!
   TBranch        *b_el_charge;   //!
   TBranch        *b_el_sf;   //!
   TBranch        *b_el_sf_up;   //!
   TBranch        *b_el_sf_down;   //!
   TBranch        *b_n_base_mu;   //!
   TBranch        *b_n_good_mu;   //!
   TBranch        *b_mt_munu;   //!
   TBranch        *b_m_mumu;   //!
   TBranch        *b_mu_p4;   //!
   TBranch        *b_mu_is_good;   //!
   TBranch        *b_mu_iso_trackonly;   //!
   TBranch        *b_mu_d0;   //!
   TBranch        *b_mu_z0;   //!
   TBranch        *b_mu_zp;   //!
   TBranch        *b_mu_charge;   //!
   TBranch        *b_mu_sf;   //!
   TBranch        *b_mu_stat_up;   //!
   TBranch        *b_mu_stat_down;   //!
   TBranch        *b_mu_sys_up;   //!
   TBranch        *b_mu_sys_down;   //!
   TBranch        *b_mu_trigger_up;   //!
   TBranch        *b_mu_trigger_down;   //!
   TBranch        *b_MET_etx;   //!
   TBranch        *b_MET_ety;   //!
   TBranch        *b_MET_phi;   //!
   TBranch        *b_MET_et;   //!
   TBranch        *b_MET_ecorr;   //!
   TBranch        *b_MET_sumet;   //!
   TBranch        *b_MET_mu_etx;   //!
   TBranch        *b_MET_mu_ety;   //!
   TBranch        *b_MET_mu_sumet;   //!
   TBranch        *b_MET_ph_etx;   //!
   TBranch        *b_MET_ph_ety;   //!
   TBranch        *b_MET_ph_sumet;   //!
   TBranch        *b_MET_trk_etx;   //!
   TBranch        *b_MET_trk_ety;   //!
   TBranch        *b_MET_trk_phi;   //!
   TBranch        *b_MET_trk_sumet;   //!
   TBranch        *b_MET_noEM_et;   //!
   TBranch        *b_MET_noEM_phi;   //!
   TBranch        *b_MET_noEM_sumet;   //!
   TBranch        *b_min_dphi_jetMET;   //!
   TBranch        *b_min_dphi_jetMET_zee;   //!
   TBranch        *b_max_dphi_jetjet;   //!
   TBranch        *b_average_dphi_jetjet;   //!
   TBranch        *b_average_dphi_jetjet_now;   //!
   TBranch        *b_max_dr_jetjet;   //!
   TBranch        *b_average_dr_jetjet;   //!
   TBranch        *b_average_dr_jetjet_now;   //!
   TBranch        *b_flag_dphi_jetMET;   //!
   TBranch        *b_flag_el_veto;   //!
   TBranch        *b_flag_mu_veto;   //!
   TBranch        *b_flag_trk_veto;   //!
   TBranch        *b_flag_4thjet_veto;   //!
   TBranch        *b_track_sum_pt;   //!
   TBranch        *b_track_n;   //!
   TBranch        *b_track_sum_px;   //!
   TBranch        *b_track_sum_py;   //!
   TBranch        *b_track_sum_phi;   //!
   TBranch        *b_track_pt;   //!
   TBranch        *b_track_eta;   //!
   TBranch        *b_track_phi;   //!
   TBranch        *b_track_theta;   //!
   TBranch        *b_track_d0;   //!
   TBranch        *b_track_z0;   //!
   TBranch        *b_track_beamspot_z0;   //!
   TBranch        *b_track_match_vz;   //!
   TBranch        *b_track_LooseP;   //!
   TBranch        *b_track_TightP;   //!
   TBranch        *b_n_base_ph;   //!
   TBranch        *b_ph_is_tight;   //!
   TBranch        *b_ph_is_medium;   //!
   TBranch        *b_ph_topoetcone40;   //!
   TBranch        *b_ph_p4;   //!
   TBranch        *b_ph_etaBE;   //!
   TBranch        *b_jet_emf;   //!
   TBranch        *b_jet_hecf;   //!
   TBranch        *b_jet_LArQ;   //!
   TBranch        *b_jet_hecQ;   //!
   TBranch        *b_jet_sumpt_trk;   //!
   TBranch        *b_jet_fracSamplingMax;   //!
   TBranch        *b_jet_negE;   //!
   TBranch        *b_jet_avgLArQF;   //!
   TBranch        *b_jet_fracSamplingMaxIndex;   //!
   TBranch        *b_jet_isBadTight;   //!
   TBranch        *b_jet_timing;   //!

   physics(TTree *tree=0);
   virtual ~physics();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef physics_cxx
physics::physics(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("reduced_ntup.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("reduced_ntup.root");
      }
      f->GetObject("physics",tree);

   }
   Init(tree);
}

physics::~physics()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t physics::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t physics::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void physics::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   jet_p4 = 0;
   jet_dphi_MET = 0;
   jet_jvf0 = 0;
   jet_jvf1 = 0;
   jet_jvt = 0;
   el_p4 = 0;
   el_is_good = 0;
   el_medium_LLH = 0;
   el_iso_trackonly = 0;
   el_d0 = 0;
   el_z0 = 0;
   el_zp = 0;
   el_charge = 0;
   el_sf = 0;
   el_sf_up = 0;
   el_sf_down = 0;
   mu_p4 = 0;
   mu_is_good = 0;
   mu_iso_trackonly = 0;
   mu_d0 = 0;
   mu_z0 = 0;
   mu_zp = 0;
   mu_charge = 0;
   mu_sf = 0;
   mu_stat_up = 0;
   mu_stat_down = 0;
   mu_sys_up = 0;
   mu_sys_down = 0;
   mu_trigger_up = 0;
   mu_trigger_down = 0;
   track_sum_pt = 0;
   track_n = 0;
   track_sum_px = 0;
   track_sum_py = 0;
   track_sum_phi = 0;
   track_pt = 0;
   track_eta = 0;
   track_phi = 0;
   track_theta = 0;
   track_d0 = 0;
   track_z0 = 0;
   track_beamspot_z0 = 0;
   track_match_vz = 0;
   track_LooseP = 0;
   track_TightP = 0;
   ph_is_tight = 0;
   ph_is_medium = 0;
   ph_topoetcone40 = 0;
   ph_p4 = 0;
   ph_etaBE = 0;
   jet_emf = 0;
   jet_hecf = 0;
   jet_LArQ = 0;
   jet_hecQ = 0;
   jet_sumpt_trk = 0;
   jet_fracSamplingMax = 0;
   jet_negE = 0;
   jet_avgLArQF = 0;
   jet_fracSamplingMaxIndex = 0;
   jet_isBadTight = 0;
   jet_timing = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("RunNumber", &RunNumber, &b_RunNumber);
   fChain->SetBranchAddress("EventNumber", &EventNumber, &b_EventNumber);
   fChain->SetBranchAddress("mc_channel_number", &mc_channel_number, &b_mc_channel_number);
   fChain->SetBranchAddress("MCWeight", &MCWeight, &b_MCWeight);
   fChain->SetBranchAddress("PUWeight", &PUWeight, &b_PUWeight);
   fChain->SetBranchAddress("actualIPC", &actualIPC, &b_actualIPC);
   fChain->SetBranchAddress("averageIPC", &averageIPC, &b_averageIPC);
   fChain->SetBranchAddress("bcid", &bcid, &b_bcid);
   fChain->SetBranchAddress("lumiblock", &lumiblock, &b_lumiblock);
   fChain->SetBranchAddress("n_good_trk", &n_good_trk, &b_n_good_trk);
   fChain->SetBranchAddress("trig_e120_lhloose", &trig_e120_lhloose, &b_trig_e120_lhloose);
   fChain->SetBranchAddress("trig_e17_lhloose_L1EM15", &trig_e17_lhloose_L1EM15, &b_trig_e17_lhloose_L1EM15);
   fChain->SetBranchAddress("trig_e24_lhmedium_L1EM18VH", &trig_e24_lhmedium_L1EM18VH, &b_trig_e24_lhmedium_L1EM18VH);
   fChain->SetBranchAddress("trig_e24_lhmedium_iloose_L1EM18VH", &trig_e24_lhmedium_iloose_L1EM18VH, &b_trig_e24_lhmedium_iloose_L1EM18VH);
   fChain->SetBranchAddress("trig_e24_lhmedium_iloose_L1EM20VH", &trig_e24_lhmedium_iloose_L1EM20VH, &b_trig_e24_lhmedium_iloose_L1EM20VH);
   fChain->SetBranchAddress("trig_e24_lhtight_iloose", &trig_e24_lhtight_iloose, &b_trig_e24_lhtight_iloose);
   fChain->SetBranchAddress("trig_e24_lhtight_iloose_L1EM20VH", &trig_e24_lhtight_iloose_L1EM20VH, &b_trig_e24_lhtight_iloose_L1EM20VH);
   fChain->SetBranchAddress("trig_e60_lhmedium", &trig_e60_lhmedium, &b_trig_e60_lhmedium);
   fChain->SetBranchAddress("trig_g140_loose", &trig_g140_loose, &b_trig_g140_loose);
   fChain->SetBranchAddress("trig_j100", &trig_j100, &b_trig_j100);
   fChain->SetBranchAddress("trig_j100_xe80_dphi10", &trig_j100_xe80_dphi10, &b_trig_j100_xe80_dphi10);
   fChain->SetBranchAddress("trig_j110", &trig_j110, &b_trig_j110);
   fChain->SetBranchAddress("trig_j150", &trig_j150, &b_trig_j150);
   fChain->SetBranchAddress("trig_j150_xe90", &trig_j150_xe90, &b_trig_j150_xe90);
   fChain->SetBranchAddress("trig_j175", &trig_j175, &b_trig_j175);
   fChain->SetBranchAddress("trig_j200", &trig_j200, &b_trig_j200);
   fChain->SetBranchAddress("trig_j200_xe80", &trig_j200_xe80, &b_trig_j200_xe80);
   fChain->SetBranchAddress("trig_j260", &trig_j260, &b_trig_j260);
   fChain->SetBranchAddress("trig_j300", &trig_j300, &b_trig_j300);
   fChain->SetBranchAddress("trig_j320", &trig_j320, &b_trig_j320);
   fChain->SetBranchAddress("trig_j360", &trig_j360, &b_trig_j360);
   fChain->SetBranchAddress("trig_j380", &trig_j380, &b_trig_j380);
   fChain->SetBranchAddress("trig_j400", &trig_j400, &b_trig_j400);
   fChain->SetBranchAddress("trig_j420", &trig_j420, &b_trig_j420);
   fChain->SetBranchAddress("trig_j440", &trig_j440, &b_trig_j440);
   fChain->SetBranchAddress("trig_j460", &trig_j460, &b_trig_j460);
   fChain->SetBranchAddress("trig_mu20_iloose_L1MU15", &trig_mu20_iloose_L1MU15, &b_trig_mu20_iloose_L1MU15);
   fChain->SetBranchAddress("trig_mu26_imedium", &trig_mu26_imedium, &b_trig_mu26_imedium);
   fChain->SetBranchAddress("trig_mu40", &trig_mu40, &b_trig_mu40);
   fChain->SetBranchAddress("trig_mu50", &trig_mu50, &b_trig_mu50);
   fChain->SetBranchAddress("trig_xe100", &trig_xe100, &b_trig_xe100);
   fChain->SetBranchAddress("trig_xe70", &trig_xe70, &b_trig_xe70);
   fChain->SetBranchAddress("trig_xe80", &trig_xe80, &b_trig_xe80);
   fChain->SetBranchAddress("jet_p4", &jet_p4, &b_jet_p4);
   fChain->SetBranchAddress("jet_dphi_MET", &jet_dphi_MET, &b_jet_dphi_MET);
   fChain->SetBranchAddress("jet_jvf0", &jet_jvf0, &b_jet_jvf0);
   fChain->SetBranchAddress("jet_jvf1", &jet_jvf1, &b_jet_jvf1);
   fChain->SetBranchAddress("jet_jvt", &jet_jvt, &b_jet_jvt);
   fChain->SetBranchAddress("n_base_jet", &n_base_jet, &b_n_base_jet);
   fChain->SetBranchAddress("n_good_jet", &n_good_jet, &b_n_good_jet);
   fChain->SetBranchAddress("n_jet_btagged", &n_jet_btagged, &b_n_jet_btagged);
   fChain->SetBranchAddress("Ht", &Ht, &b_Ht);
   fChain->SetBranchAddress("HtV", &HtV, &b_HtV);
   fChain->SetBranchAddress("n_base_el", &n_base_el, &b_n_base_el);
   fChain->SetBranchAddress("n_good_el", &n_good_el, &b_n_good_el);
   fChain->SetBranchAddress("n_good_iso_el", &n_good_iso_el, &b_n_good_iso_el);
   fChain->SetBranchAddress("mt_elnu", &mt_elnu, &b_mt_elnu);
   fChain->SetBranchAddress("m_elel", &m_elel, &b_m_elel);
   fChain->SetBranchAddress("px_zee", &px_zee, &b_px_zee);
   fChain->SetBranchAddress("py_zee", &py_zee, &b_py_zee);
   fChain->SetBranchAddress("el_p4", &el_p4, &b_el_p4);
   fChain->SetBranchAddress("el_is_good", &el_is_good, &b_el_is_good);
   fChain->SetBranchAddress("el_medium_LLH", &el_medium_LLH, &b_el_medium_LLH);
   fChain->SetBranchAddress("el_iso_trackonly", &el_iso_trackonly, &b_el_iso_trackonly);
   fChain->SetBranchAddress("el_d0", &el_d0, &b_el_d0);
   fChain->SetBranchAddress("el_z0", &el_z0, &b_el_z0);
   fChain->SetBranchAddress("el_zp", &el_zp, &b_el_zp);
   fChain->SetBranchAddress("el_charge", &el_charge, &b_el_charge);
   fChain->SetBranchAddress("el_sf", &el_sf, &b_el_sf);
   fChain->SetBranchAddress("el_sf_up", &el_sf_up, &b_el_sf_up);
   fChain->SetBranchAddress("el_sf_down", &el_sf_down, &b_el_sf_down);
   fChain->SetBranchAddress("n_base_mu", &n_base_mu, &b_n_base_mu);
   fChain->SetBranchAddress("n_good_mu", &n_good_mu, &b_n_good_mu);
   fChain->SetBranchAddress("mt_munu", &mt_munu, &b_mt_munu);
   fChain->SetBranchAddress("m_mumu", &m_mumu, &b_m_mumu);
   fChain->SetBranchAddress("mu_p4", &mu_p4, &b_mu_p4);
   fChain->SetBranchAddress("mu_is_good", &mu_is_good, &b_mu_is_good);
   fChain->SetBranchAddress("mu_iso_trackonly", &mu_iso_trackonly, &b_mu_iso_trackonly);
   fChain->SetBranchAddress("mu_d0", &mu_d0, &b_mu_d0);
   fChain->SetBranchAddress("mu_z0", &mu_z0, &b_mu_z0);
   fChain->SetBranchAddress("mu_zp", &mu_zp, &b_mu_zp);
   fChain->SetBranchAddress("mu_charge", &mu_charge, &b_mu_charge);
   fChain->SetBranchAddress("mu_sf", &mu_sf, &b_mu_sf);
   fChain->SetBranchAddress("mu_stat_up", &mu_stat_up, &b_mu_stat_up);
   fChain->SetBranchAddress("mu_stat_down", &mu_stat_down, &b_mu_stat_down);
   fChain->SetBranchAddress("mu_sys_up", &mu_sys_up, &b_mu_sys_up);
   fChain->SetBranchAddress("mu_sys_down", &mu_sys_down, &b_mu_sys_down);
   fChain->SetBranchAddress("mu_trigger_up", &mu_trigger_up, &b_mu_trigger_up);
   fChain->SetBranchAddress("mu_trigger_down", &mu_trigger_down, &b_mu_trigger_down);
   fChain->SetBranchAddress("MET_etx", &MET_etx, &b_MET_etx);
   fChain->SetBranchAddress("MET_ety", &MET_ety, &b_MET_ety);
   fChain->SetBranchAddress("MET_phi", &MET_phi, &b_MET_phi);
   fChain->SetBranchAddress("MET_et", &MET_et, &b_MET_et);
   fChain->SetBranchAddress("MET_ecorr", &MET_ecorr, &b_MET_ecorr);
   fChain->SetBranchAddress("MET_sumet", &MET_sumet, &b_MET_sumet);
   fChain->SetBranchAddress("MET_mu_etx", &MET_mu_etx, &b_MET_mu_etx);
   fChain->SetBranchAddress("MET_mu_ety", &MET_mu_ety, &b_MET_mu_ety);
   fChain->SetBranchAddress("MET_mu_sumet", &MET_mu_sumet, &b_MET_mu_sumet);
   fChain->SetBranchAddress("MET_ph_etx", &MET_ph_etx, &b_MET_ph_etx);
   fChain->SetBranchAddress("MET_ph_ety", &MET_ph_ety, &b_MET_ph_ety);
   fChain->SetBranchAddress("MET_ph_sumet", &MET_ph_sumet, &b_MET_ph_sumet);
   fChain->SetBranchAddress("MET_trk_etx", &MET_trk_etx, &b_MET_trk_etx);
   fChain->SetBranchAddress("MET_trk_ety", &MET_trk_ety, &b_MET_trk_ety);
   fChain->SetBranchAddress("MET_trk_phi", &MET_trk_phi, &b_MET_trk_phi);
   fChain->SetBranchAddress("MET_trk_sumet", &MET_trk_sumet, &b_MET_trk_sumet);
   fChain->SetBranchAddress("MET_noEM_et", &MET_noEM_et, &b_MET_noEM_et);
   fChain->SetBranchAddress("MET_noEM_phi", &MET_noEM_phi, &b_MET_noEM_phi);
   fChain->SetBranchAddress("MET_noEM_sumet", &MET_noEM_sumet, &b_MET_noEM_sumet);
   fChain->SetBranchAddress("min_dphi_jetMET", &min_dphi_jetMET, &b_min_dphi_jetMET);
   fChain->SetBranchAddress("min_dphi_jetMET_zee", &min_dphi_jetMET_zee, &b_min_dphi_jetMET_zee);
   fChain->SetBranchAddress("max_dphi_jetjet", &max_dphi_jetjet, &b_max_dphi_jetjet);
   fChain->SetBranchAddress("average_dphi_jetjet", &average_dphi_jetjet, &b_average_dphi_jetjet);
   fChain->SetBranchAddress("average_dphi_jetjet_now", &average_dphi_jetjet_now, &b_average_dphi_jetjet_now);
   fChain->SetBranchAddress("max_dr_jetjet", &max_dr_jetjet, &b_max_dr_jetjet);
   fChain->SetBranchAddress("average_dr_jetjet", &average_dr_jetjet, &b_average_dr_jetjet);
   fChain->SetBranchAddress("average_dr_jetjet_now", &average_dr_jetjet_now, &b_average_dr_jetjet_now);
   fChain->SetBranchAddress("flag_dphi_jetMET", &flag_dphi_jetMET, &b_flag_dphi_jetMET);
   fChain->SetBranchAddress("flag_el_veto", &flag_el_veto, &b_flag_el_veto);
   fChain->SetBranchAddress("flag_mu_veto", &flag_mu_veto, &b_flag_mu_veto);
   fChain->SetBranchAddress("flag_trk_veto", &flag_trk_veto, &b_flag_trk_veto);
   fChain->SetBranchAddress("flag_4thjet_veto", &flag_4thjet_veto, &b_flag_4thjet_veto);
   fChain->SetBranchAddress("track_sum_pt", &track_sum_pt, &b_track_sum_pt);
   fChain->SetBranchAddress("track_n", &track_n, &b_track_n);
   fChain->SetBranchAddress("track_sum_px", &track_sum_px, &b_track_sum_px);
   fChain->SetBranchAddress("track_sum_py", &track_sum_py, &b_track_sum_py);
   fChain->SetBranchAddress("track_sum_phi", &track_sum_phi, &b_track_sum_phi);
   fChain->SetBranchAddress("track_pt", &track_pt, &b_track_pt);
   fChain->SetBranchAddress("track_eta", &track_eta, &b_track_eta);
   fChain->SetBranchAddress("track_phi", &track_phi, &b_track_phi);
   fChain->SetBranchAddress("track_theta", &track_theta, &b_track_theta);
   fChain->SetBranchAddress("track_d0", &track_d0, &b_track_d0);
   fChain->SetBranchAddress("track_z0", &track_z0, &b_track_z0);
   fChain->SetBranchAddress("track_beamspot_z0", &track_beamspot_z0, &b_track_beamspot_z0);
   fChain->SetBranchAddress("track_match_vz", &track_match_vz, &b_track_match_vz);
   fChain->SetBranchAddress("track_LooseP", &track_LooseP, &b_track_LooseP);
   fChain->SetBranchAddress("track_TightP", &track_TightP, &b_track_TightP);
   fChain->SetBranchAddress("n_base_ph", &n_base_ph, &b_n_base_ph);
   fChain->SetBranchAddress("ph_is_tight", &ph_is_tight, &b_ph_is_tight);
   fChain->SetBranchAddress("ph_is_medium", &ph_is_medium, &b_ph_is_medium);
   fChain->SetBranchAddress("ph_topoetcone40", &ph_topoetcone40, &b_ph_topoetcone40);
   fChain->SetBranchAddress("ph_p4", &ph_p4, &b_ph_p4);
   fChain->SetBranchAddress("ph_etaBE", &ph_etaBE, &b_ph_etaBE);
   fChain->SetBranchAddress("jet_emf", &jet_emf, &b_jet_emf);
   fChain->SetBranchAddress("jet_hecf", &jet_hecf, &b_jet_hecf);
   fChain->SetBranchAddress("jet_LArQ", &jet_LArQ, &b_jet_LArQ);
   fChain->SetBranchAddress("jet_hecQ", &jet_hecQ, &b_jet_hecQ);
   fChain->SetBranchAddress("jet_sumpt_trk", &jet_sumpt_trk, &b_jet_sumpt_trk);
   fChain->SetBranchAddress("jet_fracSamplingMax", &jet_fracSamplingMax, &b_jet_fracSamplingMax);
   fChain->SetBranchAddress("jet_negE", &jet_negE, &b_jet_negE);
   fChain->SetBranchAddress("jet_avgLArQF", &jet_avgLArQF, &b_jet_avgLArQF);
   fChain->SetBranchAddress("jet_fracSamplingMaxIndex", &jet_fracSamplingMaxIndex, &b_jet_fracSamplingMaxIndex);
   fChain->SetBranchAddress("jet_isBadTight", &jet_isBadTight, &b_jet_isBadTight);
   fChain->SetBranchAddress("jet_timing", &jet_timing, &b_jet_timing);
   Notify();
}

Bool_t physics::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void physics::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t physics::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef physics_cxx
