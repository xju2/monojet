#include <stdlib.h>
#include <iostream>
#include <vector>
#include <memory>

#include <TH1F.h>
#include <TChain.h>

#include "SUSYTools/SUSYObjDef_xAOD.h"

#include "MonoJet/SysHandle.h"
#include "MonoJet/physics.h"
#include "MonoJet/Helper.h"

using namespace std;
SysHandle::SysHandle(const char* sample_name){
    debug_ = false;
    sample_name_ = string(sample_name);
    met_cut_ = 250e3;
    jet_pt_cut_ = 250e3;
    myalg = new EventCounter();
    region = new Regions();
    exp_events_ = new vector<float>();
    sys_names_ = new vector<string>();
    append_name_ = "_obs_met";
}

SysHandle::~SysHandle(){
    delete myalg;
    delete region;
    delete exp_events_;
    delete sys_names_;
}

void SysHandle::SetDebug(){
    debug_ = true;
}

void SysHandle::SetCuts(float met_cut, float jet_pt_cut, int njet_cut){
    region->SetCuts(met_cut, jet_pt_cut, njet_cut);
}

void SysHandle::GetYields(const string& norm_dir, const string& sys_dir)
{
    myalg->GetTotalEventsDic(norm_dir.c_str(), "associate");
    myalg->printTotalEvents();
    if(exp_events_->size() > 0) exp_events_->clear();
    if(sys_names_->size() > 0) sys_names_->clear();

    ST::SUSYObjDef_xAOD objTool("SUSYObjDef_xAOD");
    std::cout << " ABOUT TO INITIALIZE SUSYTOOLS " << std::endl;
    objTool.msg().setLevel(MSG::ERROR);
    int data_source = ST::FullSim;
    objTool.setProperty("DataSource", data_source);

    if( objTool.initialize() != StatusCode::SUCCESS ){
        exit(-1);
    }else{
        cout << "SUSYObjDef_xAOD initialized... " << endl;
    }
    vector<ST::SystInfo> sysInfoList = objTool.getSystInfoList();
    // systematic events
    for (const auto& sysInfo : sysInfoList){
        const CP::SystematicSet& sys = sysInfo.systset;
        if (!sysInfo.affectsKinematics) continue;
        if (!MonoJet::Helper::IsSysNeeded(sysInfo)) continue;
        string tree_name = MonoJet::Helper::parse_sys_name(sys.name().c_str());
        if (tree_name == "none") {
            cout << sys.name() << " does not fit the pattern" << endl;
            continue;
        }

        if(! ProcessTree(tree_name, sys_dir)) continue;
        sys_names_->push_back(tree_name);
    }
    // norminal events;
    ProcessTree("physics", norm_dir);
}

bool SysHandle::ProcessTree(const string& tree_name, const string& dir)
{
    if(debug_) cout << "In ProcessTree " << endl;
    TChain* fc = EventCounter::getChain(dir.c_str(), tree_name.c_str());
    Long64_t nentries = fc->GetEntries() ;
    if(nentries < 1) {
        cout << "WARNNING: " << dir << " does not have " << tree_name << " or  the tree is empty" << endl;
        return false;
    }

    physics* _p = new physics(fc);
    region->SetNtuple(_p);
    size_t nsize = exp_events_->size();
    for(int i = 0; i < NUM_REGIONS; i++){
        exp_events_->push_back(0);
    }
    if(debug_) cout << nsize << ", Booked exp_events: " << exp_events_->size() << endl;
    //TODO
    double lumi = 5000.0; 
	for ( Long64_t ientry =0; ientry < nentries ; ientry ++ )
    {
		_p->fChain->LoadTree(ientry );
		_p->fChain->GetEntry(ientry );
        int mc_id = _p->mc_channel_number;
        float weight =(float) lumi*myalg->getCrossSection(mc_id)/myalg->getTotalEvents(mc_id)*_p->MCWeight;
        //Different regions have to orthognal!
        int type = -1;
        if(region->IsSR()){ 
            type = 0;
        } else if (region->IsCRWmn()) { 
            type = 1; 
        } else if (region->IsCRWen()) { 
            type = 2;
        } else if (region->IsCRZmm()) { 
            type = 3; 
        } else if (region->IsCRZee()) { 
            type = 4; 
        } else { type = -1; }
        if (type < 0) continue;
        int real_index =  type + nsize;
        exp_events_->at(real_index) += weight;
    }
    if(debug_) cout << "After the loop" << endl;
    delete _p;
    return true;
}

void SysHandle::PrintYields(){
    if(sys_names_->size() < 1 || exp_events_->size() < 1){
        cout << "SysHandle::PrintYields(), Please run GetYields first" << endl;
        return;
    }
    cout << "Sample: " << sample_name_ << endl;
    cout << "total systematics: " << sys_names_->size() << endl;
    cout << "total yield entries: " << exp_events_->size() << endl;
    for(int i = 0; i < (int)sys_names_->size(); i++){
        cout << sys_names_->at(i) << ": ";
        for(int j = 0; j < NUM_REGIONS; j++){
            int real_index = i*NUM_REGIONS + j;
            cout << " " << exp_events_->at(real_index);
        }
        cout << endl;
    }
    cout << "norminal values: ";
    for(int i = 0; i < NUM_REGIONS; i++){ // norminal values
        cout << " " << exp_events_->at(i+sys_names_->size()) ;
    }
    cout << endl;
}

void SysHandle::GetHist(const char* output_name)
{
    if(sys_names_->size() < 1 || exp_events_->size() < 1){
        cout << "SysHandle::GetHist(), Please run GetYields first" << endl;
        return;
    }
    TFile* file = TFile::Open(output_name, "recreate");
    for(int i = 0; i < (int) sys_names_->size(); i++){
        for(int j = 0; j < NUM_REGIONS; j++){
            TH1F* h1 = CreateSysHist(i, j);
            if(h1){ 
                h1->Write();
                delete h1;
            }
        }
    }
    // Get Norm Histograms
    for(int i = 0; i < NUM_REGIONS; i++){ // norminal values
        string region_name(Regions::GetRegionName((RegionType)i));
        string hist_name = "h"+sample_name_+"Nom_"+region_name+append_name_;
        TH1F* h1 = CreateHist(exp_events_->at(i+sys_names_->size()), hist_name);
        h1->Write();
        delete h1;
    }
    file->Close();
}

TH1F* SysHandle::CreateSysHist(int i , int j)
{
    // i is systematic index
    // j is region index
    int real_index = i * NUM_REGIONS + j;
    string region_name(Regions::GetRegionName((RegionType)j));
    string line(sys_names_->at(i));
    size_t delim_pos = line.find("-");
    string sys_name;
    string var_name;
    if (delim_pos != string::npos) {
        sys_name = line.substr(0, delim_pos);
        var_name = line.substr(delim_pos+1, line.size());
    } else {
        return NULL;
    }
    if (var_name == "up") var_name = "High";
    else if (var_name == "down") var_name = "Low";
    else {
        cout << "Don't understand vary name: " << var_name << endl;
        return NULL;
    }
    string hist_name = "h"+sample_name_+sys_name+var_name+"_"+region_name+append_name_;
    return CreateHist(exp_events_->at(real_index), hist_name);
}

TH1F* SysHandle::CreateHist(float yield, const string& name){
    TH1F* res = new TH1F("res", "res", 1, 0., 1.);
    res->Fill(0.5, yield);
    res->SetName(name.c_str());
    return res;
}
