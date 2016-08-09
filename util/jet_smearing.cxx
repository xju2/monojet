#include <TLorentzVector.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>

#include <iostream>
#include <string>
#include <vector>

#include "MonoJet/SmearedInfo.h"
#include "MonoJet/PrescaleFactorTool.h"
#include "CPAnalysisExamples/errorcheck.h"
#include "MyXAODTools/Helper.h"

using namespace std;

int main(int argc, char* argv[])
{
    const char* APP_NAME = argv[ 0 ];
    if ((argc > 1 && string(argv[1]) == "help") || argc < 2)
    {
        Info(APP_NAME, " input.root outname.root");
        exit(0);
    }
    const string& file_name(argv[1]);
    const string& out_name(argv[2]);
    cout << "file name: " << file_name << endl;
    cout << "out name: " << out_name << endl;

    TChain* tin = MyXAODTools::Helper::loader(file_name.c_str(), "physics");
    if(!tin) exit(1);

    vector<TLorentzVector>* jet_p4 = nullptr;
    double triggerWeight;
    int n_base_mu;
    int n_base_el;
    double MET_et;
    double MET_sumet;
    double MET_et_soft;
    int n_good_jet;
    float min_dphi_jetMET;
    UInt_t n_vertices;
    int n_jet_btagged;
    int RunNumber;
    int EventNumber;
    UInt_t lumiblock;
    tin->SetBranchAddress("jet_p4", &jet_p4);
    tin->SetBranchAddress("triggerWeight", &triggerWeight);
    tin->SetBranchAddress("n_base_mu", &n_base_mu);
    tin->SetBranchAddress("n_base_el", &n_base_el);
    tin->SetBranchAddress("MET_et", &MET_et);
    tin->SetBranchAddress("MET_sumet", &MET_sumet);
    tin->SetBranchAddress("MET_et_soft", &MET_et_soft);
    tin->SetBranchAddress("n_good_jet", &n_good_jet);
    tin->SetBranchAddress("min_dphi_jetMET", &min_dphi_jetMET);
    tin->SetBranchAddress("n_vertices", &n_vertices);
    tin->SetBranchAddress("n_jet_btagged", &n_jet_btagged);
    vector<SmearedInfo>* pseudoData = nullptr;
    tin->SetBranchAddress("pseudoData", &pseudoData);
    tin->SetBranchAddress("RunNumber", &RunNumber);
    tin->SetBranchAddress("EventNumber", &EventNumber);
    tin->SetBranchAddress("lumiblock", &lumiblock);

    tin->SetBranchStatus("*",0);
    tin->SetBranchStatus("jet_p4", 1);
    tin->SetBranchStatus("triggerWeight", 1);
    tin->SetBranchStatus("n_base_mu", 1);
    tin->SetBranchStatus("n_base_el", 1);
    tin->SetBranchStatus("MET_et", 1);
    tin->SetBranchStatus("MET_sumet", 1);
    tin->SetBranchStatus("MET_et_soft", 1);
    tin->SetBranchStatus("n_good_jet", 1);
    tin->SetBranchStatus("min_dphi_jetMET", 1);
    tin->SetBranchStatus("n_vertices", 1);
    tin->SetBranchStatus("n_jet_btagged", 1);
    tin->SetBranchStatus("pseudoData*", 1); // Important!
    tin->SetBranchStatus("RunNumber", 1);
    tin->SetBranchStatus("EventNumber", 1);
    tin->SetBranchStatus("lumiblock", 1);

    // book trigger information
    bool trig_j400;
    bool trig_j360;
    bool trig_j320;
    bool trig_j260;
    bool trig_j200;
    bool trig_j150;
    bool trig_j110;
    bool trig_j60;
    tin->SetBranchAddress("trig_j400", &trig_j400);
    tin->SetBranchAddress("trig_j360", &trig_j360);
    tin->SetBranchAddress("trig_j320", &trig_j320);
    tin->SetBranchAddress("trig_j260", &trig_j260);
    tin->SetBranchAddress("trig_j200", &trig_j200);
    tin->SetBranchAddress("trig_j150", &trig_j150);
    tin->SetBranchAddress("trig_j110", &trig_j110);
    tin->SetBranchAddress("trig_j60", &trig_j60);
    tin->SetBranchStatus("trig_j400", 1);
    tin->SetBranchStatus("trig_j360", 1);
    tin->SetBranchStatus("trig_j320", 1);
    tin->SetBranchStatus("trig_j260", 1);
    tin->SetBranchStatus("trig_j200", 1);
    tin->SetBranchStatus("trig_j150", 1);
    tin->SetBranchStatus("trig_j110", 1);
    tin->SetBranchStatus("trig_j60",  1);

    // book branches for output tree
    auto outfile = TFile::Open(out_name.c_str(), "recreate");
    auto smeared_tree = new TTree("smeared", "smeared");
    auto seed_tree = new TTree("seed", "seed");

    int _run;
    int _lbn;
    float _met;
    float _sumet;
    float _jet_pt;
    float _jet_eta;
    float _jet_phi;
    float _subjet_pt;
    float _subjet_eta;
    float _subjet_phi;
    int _njets;
    float _dphi;
    float _dphi_ep;
    float _rmet_pt;
    float _weight;
    float _weightOrg;
    float _ht;
    float _l3rd_jet_pt;
    float _l3rd_jet_eta;
    float _l3rd_jet_phi;
    float _l4th_jet_pt;
    float _l4th_jet_eta;
    float _l4th_jet_phi;
    int _n_vertices;
    float _mass_eff;
    float _met_sig;
    float _frac_soft;

    smeared_tree->Branch("run", &_run, "run/I");
    smeared_tree->Branch("lb", &_lbn, "lb/I");
    smeared_tree->Branch("met_et", &_met, "met_et/F");
    smeared_tree->Branch("sumet", &_sumet, "sumet/F");
    smeared_tree->Branch("leading_jet_pt", &_jet_pt, "leading_jet_pt/F");
    smeared_tree->Branch("leading_jet_eta", &_jet_eta, "leading_jet_eta/F");
    smeared_tree->Branch("leading_jet_phi", &_jet_phi, "leading_jet_phi/F");
    smeared_tree->Branch("sub_leading_jet_pt", &_subjet_pt, "sub_leading_jet_pt/F");
    smeared_tree->Branch("sub_leading_jet_eta", &_subjet_eta, "sub_leading_jet_eta/F");
    smeared_tree->Branch("sub_leading_jet_phi", &_subjet_phi, "sub_leading_jet_phi/F");
    smeared_tree->Branch("njets", &_njets, "njets/I");
    smeared_tree->Branch("min_dphi", &_dphi, "min_dphi/F");
    smeared_tree->Branch("dphi_ep", &_dphi_ep, "dphi_ep/F");
    smeared_tree->Branch("rmet_pt", &_rmet_pt, "rmet_pt/F");
    smeared_tree->Branch("weight", &_weight, "weight/F");
    smeared_tree->Branch("weightOrg", &_weightOrg, "weightOrg/F");
    smeared_tree->Branch("Ht", &_ht, "Ht/F");
    smeared_tree->Branch("l3rd_jet_pt",  &_l3rd_jet_pt, "l3rd_jet_pt/F");
    smeared_tree->Branch("l3rd_jet_eta", &_l3rd_jet_eta, "l3rd_jet_eta/F");
    smeared_tree->Branch("l3rd_jet_phi", &_l3rd_jet_phi, "l3rd_jet_phi/F");
    smeared_tree->Branch("l4th_jet_pt",  &_l4th_jet_pt, "l4th_jet_pt/F");
    smeared_tree->Branch("l4th_jet_eta", &_l4th_jet_eta, "l4th_jet_eta/F");
    smeared_tree->Branch("l4th_jet_phi", &_l4th_jet_phi, "l4th_jet_phi/F");
    smeared_tree->Branch("n_vertices", &_n_vertices, "n_vertices/I");
    smeared_tree->Branch("mass_eff", &_mass_eff, "mass_eff/F");

    seed_tree->Branch("run", &_run, "run/I");
    seed_tree->Branch("lb", &_lbn, "lb/I");
    seed_tree->Branch("met_et", &_met, "met_et/F");
    seed_tree->Branch("njets", &_njets, "njets/I");
    seed_tree->Branch("leading_jet_pt", &_jet_pt, "leading_jet_pt/F");
    seed_tree->Branch("leading_jet_eta", &_jet_eta, "leading_jet_eta/F");
    seed_tree->Branch("min_dphi", &_dphi, "min_dphi/F");
    //seed_tree->Branch("dphi_ep", dphiEP, "dphi_ep/F");
    //seed_tree->Branch("rmet_pt", rmet_pt, "rmet_pt/F");
    seed_tree->Branch("weight", &_weight, "weight/F");
    seed_tree->Branch("weightOrg", &_weightOrg, "weightOrg/F");
    seed_tree->Branch("met_sig", &_met_sig, "met_sig/F");
    seed_tree->Branch("frac_soft", &_frac_soft, "frac_soft/F");

    // book some plots for comparison

    float met_xbins[17] = {
        50, 100, 125, 150, 175,
        200, 225, 250, 275, 300,
        350, 400, 450, 500, 600, 800, 1000
    };

    int met_nbins = 16;
    auto h_jet_pt_temp = new TH1F("jet_pt_temp","jet_pt_temp", met_nbins, met_xbins);
    auto h_jet_pt_smeared = (TH1F*) h_jet_pt_temp->Clone("leading_jet_pt_smeared");
    auto h_jet_pt_seed = (TH1F*) h_jet_pt_temp->Clone("leading_jet_pt_seed");
    auto h_jet_pt_all  = (TH1F*) h_jet_pt_temp->Clone("leading_jet_pt_all");
    auto h_met_smeared = (TH1F*) h_jet_pt_temp->Clone("met_smeared");
    auto h_met_seed    = (TH1F*) h_jet_pt_temp->Clone("met_seed");
    auto h_met_all     = (TH1F*) h_jet_pt_temp->Clone("met_all");

    auto h_njets_temp = new TH1F("njets_temp", "njets_temp", 11, -0.5, 10.5);
    auto h_njets_smeared = (TH1F*) h_njets_temp->Clone("njets_smeared");
    auto h_njets_seed    = (TH1F*) h_njets_temp->Clone("njets_seed");
    auto h_njets_all     = (TH1F*) h_njets_temp->Clone("njets_all");

    auto h_dphi_temp = new TH1F("dphi_temp", "mini dphi", 64, 0, 3.2);
    auto h_dphi_smeared = (TH1F*) h_dphi_temp->Clone("dphi_smeared");
    auto h_dphi_seed    = (TH1F*) h_dphi_temp->Clone("dphi_seed");
    auto h_dphi_all     = (TH1F*) h_dphi_temp->Clone("dphi_all");


    Long64_t nentries = (Long64_t) tin->GetEntries();
    Info(APP_NAME, "total entries %d", (int) nentries);
    int seed_events = 0;
    auto pft = new PrescaleFactorTool();
    // for( Long64_t ientry =0; ientry < 10; ientry ++ )
    for( Long64_t ientry =0; ientry < nentries; ientry ++ )
    {
        tin->GetEntry(ientry);
        if(n_base_el > 0 || n_base_mu > 0) continue;
        _weightOrg = triggerWeight;
        map<string, bool> trigger_input;
        trigger_input["HLT_j400"] = trig_j400;
        trigger_input["HLT_j360"] = trig_j360;
        trigger_input["HLT_j320"] = trig_j320;
        trigger_input["HLT_j260"] = trig_j260;
        trigger_input["HLT_j200"] = trig_j200;
        trigger_input["HLT_j150"] = trig_j150;
        trigger_input["HLT_j110"] = trig_j110;
        trigger_input["HLT_j60"]  = trig_j60;
        _run = RunNumber;
        _lbn = lumiblock;
        _jet_pt = (float) jet_p4->at(0).Pt()/1E3;

        _weight = pft->getWeight(_run,
                PrescaleFactorTool::getTrigger(trigger_input, _jet_pt),
                _lbn);
        if(_weight == 0){
            // pft->test();
            // cout <<_run<<" " <<EventNumber << " " << _lbn <<" "<< _jet_pt << " has zero weight" << endl;
            // break;
            continue;
        }

        // before apply seed cuts
        _met = (float) MET_et/1E3;
        _sumet = (float) MET_sumet/1E3;
        _met_sig = (_met - 8) / sqrt(_sumet);
        _njets = n_good_jet;
        _dphi = min_dphi_jetMET;

        h_jet_pt_all->Fill(_jet_pt, _weight);
        h_met_all   ->Fill(_met, _weight);
        h_njets_all ->Fill((float)_njets, _weight);
        h_dphi_all  ->Fill(_dphi, _weight);


        if (_met_sig >= 0.5 + 0.1 * n_jet_btagged) {
            continue;
        }
        seed_events ++;

        // after apply met significant cuts, Fill seed tree
        h_jet_pt_seed->Fill(_jet_pt, _weight);
        h_met_seed   ->Fill(_met, _weight);
        h_njets_seed ->Fill((float)_njets, _weight);
        h_dphi_seed  ->Fill(_dphi, _weight);

        _frac_soft = MET_et_soft/1E3/MET_et;
        _jet_eta = jet_p4->at(0).Pt();
        _n_vertices = n_vertices;
        seed_tree->Fill();

        // Fill smeared tree
        // cout << "Size of pseudoData: " << pseudoData->size() << endl;
        for(int ips = 0; ips < (int) pseudoData->size(); ips++)
        { 
            SmearedInfo data = pseudoData->at(ips);
            _jet_pt = data.leading_jet_pt_/1E3;
            _met = data.met_/1E3;
            _sumet =  data.sum_et_/1E3;
            _njets = data.n_good_jets_;

            h_jet_pt_smeared->Fill(_jet_pt, _weight);
            h_met_smeared   ->Fill(_met, _weight);
            h_njets_smeared ->Fill((float)_njets, _weight);
            h_dphi_smeared  ->Fill(_dphi, _weight);
            // cout << "MET: " << _met << " " << data.met_ << endl;
            if (_met < 100)  continue;

            _jet_eta = data.leading_jet_eta_;
            _jet_phi = data.leading_jet_phi_;
            _subjet_pt = data.sub_leading_jet_pt_/1E3;
            _subjet_eta = data.sub_leading_jet_eta_;
            _subjet_phi = data.sub_leading_jet_phi_;
            _dphi = data.min_jets_met_;
            _dphi_ep = data.dphi_EP_;
            _rmet_pt = data.met_/data.leading_jet_pt_;
            _ht = data.HT_/1E3;
            _l3rd_jet_pt = data.l3rd_jet_pt_/1E3;
            _l3rd_jet_eta = data.l3rd_jet_eta_;
            _l3rd_jet_phi = data.l3rd_jet_phi_;
            _l4th_jet_pt = data.l4th_jet_pt_/1E3;
            _l4th_jet_eta = data.l4th_jet_eta_;
            _l4th_jet_phi = data.l4th_jet_phi_;
            _mass_eff = _jet_pt+_subjet_pt+_l3rd_jet_pt+_l4th_jet_pt+_met;
            smeared_tree->Fill();
        }
    }
    delete pft;
    Info(APP_NAME, "seed events: %d", seed_events);

    outfile->cd();
    smeared_tree->Write();
    seed_tree->Write();
    h_jet_pt_smeared->Write();
    h_jet_pt_seed   ->Write();
    h_jet_pt_all    ->Write();
    h_met_smeared ->Write();
    h_met_seed    ->Write();
    h_met_all     ->Write();
    h_njets_smeared ->Write();
    h_njets_seed    ->Write();
    h_njets_all     ->Write();

    h_dphi_smeared->Write();
    h_dphi_seed   ->Write();
    h_dphi_all    ->Write();

    return 0;
}
