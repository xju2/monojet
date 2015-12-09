#include <stdlib.h>

#include "TVector2.h"

#include "MonoJet/Regions.h"

#include <iostream>

using namespace std;
Regions::Regions():
    p_(nullptr), verbose_(false), 
    nodphi_(false), is_data_(false),
    trigger_(true)
{
    SetCuts(250e3, 250e3, 4);
    cp_helper_ = new CPToolsHelper();
    clear();
}

Regions::Regions(physics* p):
    p_(p), verbose_(false), 
    nodphi_(false), is_data_(false),
    trigger_(true)
{
    SetCuts(250e3, 250e3, 4);
    cp_helper_ = new CPToolsHelper();
    clear();
}

Regions::Regions(physics* p, float jet_pt_cut, 
        float met_cut, int njet_cut):
    p_(p) 
{
    SetCuts(jet_pt_cut, met_cut, njet_cut);
    verbose_ = false;
    nodphi_ = false;
    is_data_ = false;
    trigger_ = false;
    cp_helper_ = new CPToolsHelper();
    clear();
}

Regions::~Regions(){
    p_ = NULL;
    delete cp_helper_;
}

void Regions::SetNtuple(physics* p){
    p_ = p;
}

void Regions::SetCuts(float jet_pt_cut, float met_cut, int njet_cut)
{
    clear();
    jet_pt_cut_ = jet_pt_cut;
    met_cut_ = met_cut;
    njet_cut_ = njet_cut;
    if(verbose_){
        cout <<"Set Cuts: MET("<< met_cut_ << "), njet(" << njet_cut_ << "), jet_pt(" << jet_pt_cut << ")." <<endl;
    }
}

void Regions::clear(){
    n_good_mu_iso = -1;
    n_good_mu_d0z0 = -1;
    n_good_mu_all = -1;
    n_good_el_iso = -1;
    n_good_el_d0z0 = -1;
    n_good_el_all = -1;
}

bool Regions::PassCommonCuts(double met_value, double dphi_jet_met)
{
    if(!PassGRL()) return false;

    // pt, MET, dphi, n_jets 
    if(p_->n_good_jet < 1 || 
       p_->n_good_jet > njet_cut_ || 
       (p_->jet_p4->at(0)).Pt() <= jet_pt_cut_)
    { 
        if(verbose_) cout << "No jets" << endl;
        return false;
    }
    if(met_value <= met_cut_) {
        if(verbose_) cout << p_->MET_et << ", Failed MET(Common)"  << endl;
        return false;
    }

    if(!nodphi_ && dphi_jet_met <= 0.4 ){ 
        if(verbose_) cout << "Failed dphi(jets, MET)(Common)" << endl;
        return false;
    }
    if(!PassBadTight()) return false;

    return true;
}

bool Regions::IsSR()
{
    if(!PassMETTrigger()) return false;
    if(!PassCommonCuts(p_->MET_et, p_->min_dphi_jetMET)) return false;
    return (
           p_->n_base_el == 0 && 
           p_->n_base_mu == 0
           ) ;
}

bool Regions::IsCRWen()
{
    if(!PassMETTrigger()) return false;
    if(!PassCommonCuts(p_->MET_et, p_->min_dphi_jetMET)) return false;

    return (
            p_->n_good_iso_el == 1 && 
            p_->n_base_el == 1 &&
            p_->n_good_mu < 1 
            ) ;
}

bool Regions::IsCRWmn()
{
    if(!PassMETTrigger()) return false;
    if(!PassCommonCuts(p_->MET_et, p_->min_dphi_jetMET)) return false;
    double mt_mu_nu = p_->mt_munu/1e3;

    return (
            p_->n_good_el == 0 && 
            p_->n_good_mu == 1 && 
            p_->n_base_mu == 1 &&
            mt_mu_nu > 30 &&
            mt_mu_nu < 100
            ) ;
}

bool Regions::IsCRZmm()
{
    if(!PassMETTrigger()) return false;
    if(!PassCommonCuts(p_->MET_et, p_->min_dphi_jetMET)) return false;
    double m_mumu = p_->m_mumu/1e3;

    return (
            p_->n_good_el == 0 && 
            p_->n_good_mu == 2 && 
            p_->n_base_mu == 2 && 
            m_mumu > 66 &&
            m_mumu < 116
            ) ;
}

bool Regions::IsCRZee()
{
    if(!PassElTrigger()) return false;
    if(!PassCommonCuts(p_->MET_noEM_et, p_->min_dphi_jetMET_zee)) return false;
    double m_zee = p_->m_elel/1e3;

    return (
            p_->n_good_mu == 0 && 
            p_->n_good_el == 2 && 
            p_->n_base_el == 2 && 
            m_zee > 66 &&
            m_zee < 116 
            ) ;
}

RegionType Regions::GetRegionType(){
    RegionType type = NUM_REGIONS;
    if(IsSR()){ type = SR;
    } else if (IsCRWmn()) { 
        type = WMUNU; 
    } else if (IsCRWen()) { 
        type = WENU;
    } else if (IsCRZmm()) { 
        type = ZMM; 
    } else if (IsCRZee()) { 
        type = ZEE; 
    } else { }
    return type;
}

const char* Regions::GetRegionName(RegionType type)
{
    string result = "NONE";
    switch(type){
        case SR:
            result = "SR";
            break;
        case WENU:
            result = "WENU";
            break;
        case WMUNU:
            result = "WMUNU";
            break;
        case ZMM:
            result = "ZMM";
            break;
        case ZEE:
            result = "ZEE";
            break;
        default:
            result = "NONE";
            break;
    }
    return result.c_str();
}

void Regions::CorrectMET()
{
}

void Regions::AddIsolation2Lep()
{
    // add isolation cuts on leptons
    n_good_mu_iso =  0;
    n_good_mu_d0z0 = 0;
    n_good_mu_all = 0;
    for(int i = 0; i < p_->n_good_mu; i++){
        if(p_->mu_iso_trackonly->at(i)) n_good_mu_iso ++;
        bool pass_d0z0 = fabs(p_->mu_d0->at(i)) <= 3.0 && fabs(p_->mu_z0->at(i)*TMath::Sin(p_->mu_p4->at(i).Theta())) <= 0.5;
        if(pass_d0z0) n_good_mu_d0z0 ++;
        if(p_->mu_iso_trackonly->at(i) && pass_d0z0) n_good_mu_all ++;
    }

    n_good_el_iso = 0;
    n_good_el_d0z0 = 0;
    n_good_el_all = 0;
    for(int i = 0; i < p_->n_good_el; i++){
        if(p_->el_iso_trackonly->at(i)) n_good_el_iso ++;
        bool pass_d0z0 = fabs(p_->el_d0->at(i)) <= 3.0 && fabs(p_->el_z0->at(i)*TMath::Sin(p_->el_p4->at(i).Theta())) <= 0.5;
        if(pass_d0z0) n_good_el_d0z0 ++;
        if(p_->el_iso_trackonly->at(i) && pass_d0z0) n_good_el_all ++;
    }
    return;
}

void Regions::ApplyIso(){
    if(n_good_mu_iso < 0) AddIsolation2Lep();
    // p_->n_good_mu = n_good_mu_iso;
    p_->n_good_el = n_good_el_iso;
}

void Regions::ApplyTrack(){
    if(n_good_mu_d0z0 < 0) AddIsolation2Lep();
    p_->n_good_mu = n_good_mu_d0z0;
    p_->n_good_el = n_good_el_d0z0;
}

void Regions::ApplyIsoTrack(){
    if(n_good_mu_all < 0) AddIsolation2Lep();
    p_->n_good_mu = n_good_mu_all;
    p_->n_good_el = n_good_el_all;
}

bool Regions::PassElTrigger()
{
    if(trigger_ 
            && !p_->trig_e24_lhmedium_iloose_L1EM18VH
            && !p_->trig_e24_lhmedium_iloose_L1EM20VH
            // && !p_->trig_e24_lhtight_iloose_L1EM20VH
            // && !p_->trig_e24_lhtight_iloose
            && !p_->trig_e60_lhmedium
            && !p_->trig_e120_lhloose
            ){ 
        if(verbose_) cout << "Failed electron trigger" << endl;
        return false;
    }
    return true;
}

bool Regions::PassMuTrigger(){
    if(trigger_ && !p_->trig_mu20_iloose_L1MU15 
            && !p_->trig_mu26_imedium
            && !p_->trig_mu50
            ){ 
        if(verbose_) cout << "Failed muon trigger" << endl;
        return false;
    }
    return true;
}

bool Regions::PassMETTrigger(){
    if(trigger_ 
            && !p_->trig_xe80 
            && !p_->trig_xe70){ 
        if(verbose_) cout << "Failed xe trigger" << endl;
        return false;
    }
    return true;
}

bool Regions::PassGRL(){
    if(is_data_ && ! cp_helper_->PassGRL(p_->RunNumber, (int)p_->lumiblock)){ 
        if(verbose_) cout << "Failed GRL" << endl;
        return false;
    }
    return true;
}

bool Regions::PassBadTight(){
    if(p_->n_good_jet < 1) return false;
    if(p_->jet_isBadTight->at(0) != 1 || 
            fabs((p_->jet_p4->at(0)).Eta()) >= 2.4
      ){ 
        if(verbose_) cout << "Failed jet cleaning on first Jet" << endl;
        return false;
    }
    return true;
}
