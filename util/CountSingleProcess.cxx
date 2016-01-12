#include "TMath.h"
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include <TVector2.h>

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <typeinfo>
#include <algorithm>

#include "MonoJet/physics.h"
#include "MyXAODTools/EventCounter.h"
#include "MonoJet/Regions.h"

using namespace std;

bool cmd_line(int argc, char** argv, map<string, string>& opts);
bool has_run(const char* file_name, int run_number);
int main( int argc, char** argv)
{
    map<string, string> opts;
    string inputFileName = "toberun.txt";

    cmd_line(argc, argv, opts);
    if((argc > 1 && string(argv[1]).find("help")!= string::npos))
    {
        cout<<argv[0]<<" filelist -num_evts= -isData -mc_id=xx -jet_cut=xx -use_weight= -nodphi -debug -noweight -signal -run_id=xx"<<endl;
        return 0;
    }

    if(argc > 1){
        inputFileName = string(argv[1]);
    }

    
    float lumi = 1.0; //5fb-1
    bool is_data = (bool) atoi(opts["-isData"].c_str());
    bool debug = (bool) atoi(opts["-debug"].c_str());
    bool no_dphi_cut = (bool) atoi(opts["-nodphi"].c_str()); 
    bool not_use_weight = (bool) atoi(opts["-noweight"].c_str());
    bool is_signal = (bool) atoi(opts["-signal"].c_str());

    EventCounter* myalg = NULL;
    if( !is_data ) {
        myalg = new EventCounter();
        if(opts["-use_weight"] == "none") {
            cout <<"No weight files provided!"<<endl;
            myalg->GetTotalEventsDic(inputFileName.c_str(), "associate");
        } else {
            myalg->ReadEventInfo(opts["-use_weight"].c_str());
        }
        // myalg->printTotalEvents();
    }
    int required_mc_id = atoi(opts["-mc_id"].c_str());
    int jet_cut = atoi(opts["-jet_cut"].c_str());
    int required_run_id = atoi(opts["-run_id"].c_str());

    TChain* fc = new TChain("physics", "physics");
    if(inputFileName.find("root") != string::npos){
        fc->AddFile(inputFileName.c_str());
    } else {
        ifstream input(inputFileName.c_str(), ifstream::in);
        TString name;
        while(input >> name){
            if((required_run_id > 0 && has_run(name.Data(), required_run_id))
                    || required_run_id <= 0){
                fc->AddFile(name.Data()); 
                cout <<"file: " << name << " added" << endl;
            } 
        }
    }
    Long64_t nentries = fc->GetEntries() ;
    cout<<"total events: "<< nentries <<endl;
    

    if(true){
        const Long64_t e = atoll(opts["-num_evts"].c_str());
        if(e > 0 && e < nentries) nentries = e;
    }

    //print a summary
    cout<<"Input file: "<< inputFileName << endl;
    cout<<"Is MC: "<< !is_data << endl;
    cout<<"Events to process: "<< nentries <<endl;
    cout<<"Requried mc_channel_number: "<< required_mc_id <<endl;
    cout<<"Requried RunNumber: "<< required_run_id << endl;
    cout<<"Jet Cuts: "<< jet_cut <<endl;
    cout<<"Debug: " << debug << endl;
    cout<<"No Dphi Cut: " << no_dphi_cut << endl;
    cout<<"Weight File: " << opts["-use_weight"] << endl;
    float dphi_cut = 0.4;
    float dphi_ep_cut = 2.0;
    float met_pt_ratio_cut = 0.3;

    physics* _p = new physics(fc);
    TFile* output = new TFile("output.root","recreate");
    vector<int>* met_cuts =  new vector<int>();

    met_cuts->push_back(10); // VR
    met_cuts->push_back(11); // QCD
    met_cuts->push_back(12); // no-dphi

    met_cuts->push_back(20); // SR (dphi>0.4)
    met_cuts->push_back(21); // QCD dphi<0.4
    met_cuts->push_back(22); // all dphi


    int total_cuts = (int) met_cuts->size();

    vector<string>* region_types = new vector<string>();
    for(int i = 0; i < NUM_REGIONS; i++){
        region_types->push_back(string(Regions::GetRegionName((RegionType)i)));
    }
    
    int total_types = (int) region_types->size();

    double met_xbins[] = {50, 100, 125, 150, 175, 200, 225, 250, 275, 300, 350, 400, 450, 500, 600, 800, 1000};
    int met_nbins = 16;
    TH1F* MET_template = new TH1F("MET_template","E_{T}^{miss};E_{T}^{miss} [GeV];Events", met_nbins, met_xbins);
    TH1F* njet_template = new TH1F("njet_template","njet;N_{jets};Events",11,-0.5,10.5);
    TH1F* dphi_template = new TH1F("dphi_template", "#Delta#phi;#Delta#phi;Events", 64, 0, 3.2);
    TH1F* ljet_pt_template = new TH1F("ljet_pt_template","ljet_pt;P^{t}_{jet} [GeV];Events", met_nbins,met_xbins);
    TH1F* ratio_met_ljet_pt = new TH1F("ratio_met_ljet_pt", "ratio_met_jetpt;E_{T}^{miss}/P_{jet}^{t}", 40, 0, 4);

    map<string, vector<TH1F*>* > hists_map;
    hists_map["n_jets"] = new vector<TH1F*>();
    hists_map["dphi"] = new vector<TH1F*>();
    hists_map["met"] = new vector<TH1F*>();
    hists_map["ljet_pt"] = new vector<TH1F*>();
    hists_map["dphi_ep"] = new vector<TH1F*>();
    hists_map["ratio_met_pt"] = new vector<TH1F*>();

    // TH2D 
    TH2F* dphi_2d_template = new TH2F("dphi_2d_template", "dphi_2d_template", 330, -0.1, 3.2, 330, -0.1, 3.2);
    vector<TH2F*>* hist_2d = new vector<TH2F*>();
    for(int i=0; i < total_cuts; i++){
        int met_cut = met_cuts->at(i);
        for(int j=0; j < total_types; j++){
            string region_name = region_types->at(j);
            TH1F* njets   = dynamic_cast<TH1F*>(njet_template->Clone(Form("njets_%s_%d", region_name.c_str(), met_cut)));
            TH1F* ljet_pt = dynamic_cast<TH1F*>(ljet_pt_template->Clone(Form("ljetPt_%s_%d", region_name.c_str(), met_cut)));
            TH1F* met     = dynamic_cast<TH1F*>(MET_template->Clone(Form("met_%s_%d", region_name.c_str(), met_cut)));
            TH1F* dphi    = dynamic_cast<TH1F*>(dphi_template->Clone(Form("dphi_%s_%d", region_name.c_str(), met_cut)));
            TH1F* dphi_ep    = dynamic_cast<TH1F*>(dphi_template->Clone(Form("dphiEP_%s_%d", region_name.c_str(), met_cut)));

            hists_map["n_jets"]->push_back(njets);
            hists_map["ljet_pt"]->push_back(ljet_pt);
            hists_map["met"]->push_back(met);
            hists_map["dphi"]->push_back(dphi);
            hists_map["dphi_ep"]->push_back(dphi_ep);

            TH1F* ratio_met_pt = dynamic_cast<TH1F*>(ratio_met_ljet_pt->Clone(Form("ratioMETpt_%s_%d", region_name.c_str(), met_cut))); 
            hists_map["ratio_met_pt"]->push_back(ratio_met_pt);

            TH2F* dphi_2d = dynamic_cast<TH2F*>(dphi_2d_template->Clone(Form("dphi2D_%s_%d", region_name.c_str(), met_cut)));
            hist_2d->push_back(dphi_2d);
        }
    }
    // cout<<"total hists for one variable: "<< hists_map["n_jets"]->size()<<endl;

    _p->fChain->GetEntry(0);
    Regions* region = new Regions(_p);

    if(debug) region->Verbose();
    if(no_dphi_cut) region->SetNoDphiCut();
    if(is_data) region->SetDataFlag();

    auto* exclude_lists = new vector<int>();
    exclude_lists->push_back(361020);
    exclude_lists->push_back(361022);
    exclude_lists->push_back(361023);
    exclude_lists->push_back(361024);
    exclude_lists->push_back(361025);

    for ( Long64_t ientry = 0; ientry < nentries ; ientry ++ )
    {
        _p->fChain->LoadTree(ientry );
        _p->fChain->GetEntry(ientry );
        int mc_id = _p->mc_channel_number;
        if (required_mc_id > 0 && mc_id != required_mc_id) continue;
        bool pass_run_id = required_run_id < 0 || required_run_id == _p->RunNumber ||
            (required_run_id == 271744 && _p->RunNumber <= required_run_id); 
        if (!pass_run_id) continue;

        // cout << _p->RunNumber << " " << _p->EventNumber << endl;
        // if(find(exclude_lists->begin(), exclude_lists->end(), mc_id) != exclude_lists->end()) continue;

        double weight = 1.0;
        if(!is_data){
            if(is_signal){
                weight = lumi*_p->MCWeight/myalg->getTotalEventsNoWeight(mc_id);
            } else {
                if(not_use_weight) {
                    weight = lumi*myalg->getCrossSection(mc_id)/myalg->getTotalEventsNoWeight(mc_id);
                } else {
                    weight = lumi*myalg->getCrossSection(mc_id)/myalg->getTotalEvents(mc_id)*_p->MCWeight;
                }
            }
        }

        region->CorrectMET();
        for(int j=0; j < total_cuts; j++){
            int cut_index = met_cuts->at(j);
            float met_cut = cut_index * 1e3;
            float jet_pt_cut = (float) met_cut;
            
            if( cut_index >= 10 && cut_index < 20) {
                met_cut = 150e3;
                jet_pt_cut = 250e3;
                region->UseVRCuts();
            }
            if( cut_index >= 20 && cut_index < 30) {
                jet_pt_cut = met_cut = 250e3;
                region->UseVRCuts(false);
            } 
            region->SetCuts(jet_pt_cut, met_cut, jet_cut);

            //Different regions are orthognal!
            int type = -1;
            if(region->IsSR()){ type = 0;
            } else if (region->IsCRWen()) { 
                type = 1; 
            } else if (region->IsCRWmn()) { 
                type = 2;
            } else if (region->IsCRZmm()) { 
                type = 3; 
            } else if (region->IsCRZee()) { 
                type = 4; 
            } else { type = -1; }
            if(debug) cout << "Type: " << type << endl;
            if (type < 0) continue;
            int real_index =  type+j*total_types;

            float dphi_et_pt = fabs(TVector2::Phi_mpi_pi(_p->MET_phi - _p->MET_trk_phi)); 
            float min_dphi = type==4? _p->min_dphi_jetMET_zee: _p->min_dphi_jetMET;
            double ljet_pt = (_p->jet_p4->at(0)).Pt()/1e3;
            double MET = type==4? _p->MET_ecorr/1e3: _p->MET_et/1e3;

            bool pass_ratio = MET/ljet_pt > met_pt_ratio_cut;
            bool pass_dphi = min_dphi > dphi_cut;

            if (cut_index == 11 || cut_index == 21) {
                if(pass_dphi) continue;
            } else if(cut_index == 12 || cut_index == 22){
                ;
            } else if(!pass_dphi) { 
                continue; 
            } else {;}

            hists_map["n_jets"]->at(real_index) ->Fill(_p->n_good_jet, weight);
            hists_map["ljet_pt"]->at(real_index) ->Fill(ljet_pt, weight);
            hists_map["met"]->at(real_index) ->Fill(MET, weight);
            hists_map["dphi"]->at(real_index) ->Fill(min_dphi, weight);
            hists_map["ratio_met_pt"]->at(real_index)->Fill(MET/ljet_pt, weight);
            hists_map["dphi_ep"]->at(real_index)->Fill(dphi_et_pt, weight);
            hist_2d->at(real_index)->Fill(min_dphi, dphi_et_pt, weight);
        }
    }
    delete exclude_lists;
    delete region;

    output->cd();
    for(map<string,vector<TH1F*>* >::iterator it = hists_map.begin(); it != hists_map.end(); it++){
        vector<TH1F*>* tmp = it->second;
        for(int i=0; i < (int)tmp->size(); i++){
            tmp->at(i)->Write();
        }
    }
    for(const auto& h2d : *hist_2d){
        h2d->Write();
    }
    output->Close();
    //clean up
    delete region_types;
    delete met_cuts;
    if(!is_data) delete myalg;
    delete _p;
    delete hist_2d;

    return 1;
}

bool cmd_line(int argc, char** argv, map<string, string>& opt_dic)
{
    opt_dic["-num_evts"] = "-1";
    opt_dic["-isData"] = "0";
    opt_dic["-mc_id"] = "-1";
    opt_dic["-run_id"] = "-1";
    opt_dic["-jet_cut"] = "4";
    opt_dic["-use_weight"] = "none";
    opt_dic["-debug"] = "0";
    opt_dic["-nodphi"] = "0";
    opt_dic["-noweight"] = "0";
    opt_dic["-signal"] = "0";
    for(int i = 0; i < argc; i ++)
    {
        if( argv[i][0] != '-' ) {
            continue;
        }
        string option(argv[i]);
        if(option == "-isData"){
            opt_dic["-isData"] = "1";
        }
        if(option == "-debug"){
            opt_dic["-debug"] = "1";
        }
        if(option == "-nodphi"){
            opt_dic["-nodphi"] = "1";
        }
        if(option == "-noweight") {
            opt_dic["-noweight"] = "1";
        }
        if(option == "-signal") {
            opt_dic["-signal"] = "1";
        }
        size_t delim_pos = option.find('=');
        // other options needs a value
        if(delim_pos != string::npos)
        {
            string tagName = option.substr(0, delim_pos);
            string token = option.substr(delim_pos+1, option.size());
            if(opt_dic.find(tagName) != opt_dic.end()) {
                opt_dic[tagName] = token;
            } else {
                cout <<"tag not recognized: " << tagName << endl;
            }
        }
    }
    return true;
}

bool has_run(const char* file_name, int run_number)
{
    TChain* chain = EventCounter::getChain(file_name, "physics"); 
    int br_run_number;
    if(chain && chain->GetEntries() > 0)
    {
        chain->SetBranchAddress("RunNumber", &br_run_number);
        chain->LoadTree(0);
        chain->GetEntry(0);
        bool result = run_number == 271744?(br_run_number <= run_number):(br_run_number == run_number);
        chain->GetCurrentFile()->Close();
        delete chain;
        return result;
    } else { return false; }
}
