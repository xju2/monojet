#include <stdlib.h>
#include <algorithm>
#include <iostream>
#include <vector>

#include <TString.h>
#include "CPAnalysisExamples/errorcheck.h"

#include "MonoJet/OutputTreeMan.h"
#include "MonoJet/Helper.h"
#include "MonoJet/dbg.h"
#include "MyXAODTools/CPToolsHelper.h"

OutputTreeMan::OutputTreeMan(bool with_sys)
{
    with_sys_ = with_sys;
    APP_NAME = "OutputTreeMan";
    trigger_map_ = {
        {"HLT_xe70", false},
        {"HLT_xe80", false},
        {"HLT_xe100", false},
        {"HLT_j200_xe80", false},
        {"HLT_j150_xe90", false},
        {"HLT_j100_xe80_dphi10", false},
        {"HLT_e17_lhloose_L1EM15", false},
        {"HLT_e24_lhmedium_iloose_L1EM18VH", false},
        {"HLT_e24_lhmedium_iloose_L1EM20VH", false},
        {"HLT_e24_lhmedium_L1EM18VH", false},
        {"HLT_e24_lhtight_iloose_L1EM20VH", false},
        {"HLT_e24_lhtight_iloose", false},
        {"HLT_e60_lhmedium", false},
        {"HLT_e120_lhloose", false},
        {"HLT_mu20_iloose_L1MU15", false},
        {"HLT_mu26_imedium", false},
        {"HLT_mu40", false},
        {"HLT_mu50", false},
        {"HLT_g140_loose", false},
        {"HLT_j100", false},
        {"HLT_j110", false},
        {"HLT_j150", false},
        {"HLT_j175", false},
        {"HLT_j200", false},
        {"HLT_j260", false},
        {"HLT_j300", false},
        {"HLT_j320", false},
        {"HLT_j360", false},
        {"HLT_j380", false},
        {"HLT_j400", false},
        {"HLT_j420", false},
        {"HLT_j440", false},
        {"HLT_j460", false}

    };
    this->ClearBranch();
    this->initializeTools();
}

OutputTreeMan::~OutputTreeMan(){
    delete iso_trackonly_;
}

int OutputTreeMan::initializeTools()
{
    // Isolation Selection Tools
    iso_trackonly_ = new CP::IsolationSelectionTool("iso_trackonly");
    CHECK(iso_trackonly_->setProperty("ElectronWP", "LooseTrackOnly"));
    CHECK(iso_trackonly_->setProperty("MuonWP", "LooseTrackOnly"));
    CHECK(iso_trackonly_->initialize());

    return 1;
}

void OutputTreeMan::ClearBranch(){
    mc_channel_number_ = -9999;
    mc_weight_ = -9999;

    for (auto& keys : trigger_map_){
        keys.second = false;
    }

    n_good_jet = 0;
    n_base_jet = 0;
    n_jet_btagged = 0;
    jet_p4_.clear();
    jet_dphi_MET.clear();
    jet_jvf0_.clear();
    jet_jvf1_.clear();
    jet_jvt_.clear();

    mt_munu = m_mumu = -999;
    mt_elnu = m_elel = -999;
    px_zee_ = py_zee_ = -999;
    MET_et_noEE = -999;

    n_base_el = 0;
    n_good_el = 0;
    n_good_iso_el = 0;
    el_is_good_.clear();
    el_medium_.clear();
    el_iso_trackonly_.clear();
    el_d0_.clear();
    el_z0_.clear();
    el_zp_.clear();
    el_charge_.clear();
    el_good_p4_.clear();
    el_sf_.clear();
    el_sf_up_.clear();
    el_sf_down_.clear();

    n_base_mu = 0;
    n_good_mu = 0;
    mu_is_good_.clear();
    mu_iso_trackonly_.clear();
    mu_d0_.clear();
    mu_z0_.clear();
    mu_zp_.clear();
    mu_charge_.clear();
    mu_good_p4_.clear();
    mu_sf_.clear();
    mu_stat_up_.clear();
    mu_stat_down_.clear();
    mu_sys_up_.clear();
    mu_sys_down_.clear();
    mu_trigger_up_.clear();
    mu_trigger_down_.clear();

    n_good_trk = 0;

    min_dphi_jetMET = -999;
    min_dphi_jetMET_zee = -999;
}

void OutputTreeMan::AddBranches2Tree(TTree& MyTree)
{
    // Event Info
    MyTree.Branch("PUWeight", &pu_weight_, "PUWeight/F");
    MyTree.Branch("corrected_mu", &corrected_mu_, "corrected_mu/F");
    MyTree.Branch("data_weight", &data_weight_, "data_weight/F");
    MyTree.Branch("actualIPC", &actualIPC_, "actualIPC/F");
    MyTree.Branch("averageIPC", &averageIPC_, "averageIPC/F");
    MyTree.Branch("bcid", &bcid_, "bcid/i");
    MyTree.Branch("lumiblock", &lumiBlock_, "lumiblock/i");
    MyTree.Branch("n_good_trk", &n_good_trk, "n_good_trk/I");

    // Trigger Info
    for(auto& kv : trigger_map_){
        TString key(kv.first);
        TString br_name(kv.first); 
        br_name.ReplaceAll("HLT", "trig");
        MyTree.Branch(br_name.Data(), &trigger_map_[key.Data()], 
                Form("%s/O",br_name.Data()));
    }

    // Jets
    MyTree.Branch("jet_p4", &jet_p4_);
    MyTree.Branch("jet_dphi_MET", &jet_dphi_MET);
    MyTree.Branch("jet_jvf0", &jet_jvf0_);
    MyTree.Branch("jet_jvf1", &jet_jvf1_);
    MyTree.Branch("jet_jvt", &jet_jvt_);
    MyTree.Branch("n_base_jet", &n_base_jet, "n_base_jet/I");
    MyTree.Branch("n_good_jet", &n_good_jet, "n_good_jet/I");
    MyTree.Branch("n_jet_btagged", &n_jet_btagged, "n_jet_btagged/I");
    MyTree.Branch("Ht", &ht_scalar_, "Ht/F");
    MyTree.Branch("HtV", &ht_vector_, "HtV/F");

    // Electrons
    MyTree.Branch("n_base_el",  &n_base_el,  "n_base_el/I");
    MyTree.Branch("n_good_el",  &n_good_el,  "n_good_el/I");
    MyTree.Branch("n_good_iso_el",  &n_good_iso_el,  "n_good_iso_el/I");
    MyTree.Branch("mt_elnu", &mt_elnu, "mt_elnu/D");
    MyTree.Branch("m_elel", &m_elel, "m_elel/D");
    MyTree.Branch("px_zee", &px_zee_, "px_zee/D");
    MyTree.Branch("py_zee", &py_zee_, "py_zee/D");
    MyTree.Branch("el_p4", &el_good_p4_);
    MyTree.Branch("el_is_good", &el_is_good_);
    MyTree.Branch("el_medium_LLH", &el_medium_);
    MyTree.Branch("el_iso_trackonly", &el_iso_trackonly_);
    MyTree.Branch("el_d0", &el_d0_);
    MyTree.Branch("el_z0", &el_z0_);
    MyTree.Branch("el_zp", &el_zp_);
    MyTree.Branch("el_charge", &el_charge_);
    MyTree.Branch("el_sf", &el_sf_);
    if(with_sys_) {
        MyTree.Branch("el_sf_up", &el_sf_up_);
        MyTree.Branch("el_sf_down", &el_sf_down_);
    }

    // Muons
    MyTree.Branch("n_base_mu",  &n_base_mu,  "n_base_mu/I");
    MyTree.Branch("n_good_mu",  &n_good_mu,  "n_good_mu/I");
    MyTree.Branch("mt_munu", &mt_munu,"mt_munu/D");
    MyTree.Branch("m_mumu", &m_mumu,"m_mumu/D");
    MyTree.Branch("mu_p4", &mu_good_p4_);
    MyTree.Branch("mu_is_good",   &mu_is_good_);
    MyTree.Branch("mu_iso_trackonly", &mu_iso_trackonly_);
    MyTree.Branch("mu_d0", &mu_d0_);
    MyTree.Branch("mu_z0", &mu_z0_);
    MyTree.Branch("mu_zp", &mu_zp_);
    MyTree.Branch("mu_charge", &mu_charge_);
    MyTree.Branch("mu_sf", &mu_sf_);
    if (with_sys_) {
        MyTree.Branch("mu_stat_up", &mu_stat_up_);
        MyTree.Branch("mu_stat_down", &mu_stat_down_);
        MyTree.Branch("mu_sys_up", &mu_sys_up_);
        MyTree.Branch("mu_sys_down", &mu_sys_down_);
        MyTree.Branch("mu_trigger_up", &mu_trigger_up_);
        MyTree.Branch("mu_trigger_down", &mu_trigger_down_);
    }

    // MET
    MyTree.Branch("MET_etx", &MET_etx, "MET_etx/D");
    MyTree.Branch("MET_ety", &MET_ety, "MET_ety/D");
    MyTree.Branch("MET_phi", &MET_phi, "MET_phi/D");
    MyTree.Branch("MET_et", &MET_et, "MET_et/D");
    MyTree.Branch("MET_ecorr", &MET_et_noEE, "MET_ecorr/D");
    MyTree.Branch("MET_sumet", &MET_sumet_, "MET_sumet/D");
    MyTree.Branch("MET_sumet_noMuon", &MET_sumet_noMuon_, "MET_sumet_noMuon/D");
    MyTree.Branch("MET_et_soft", &MET_et_soft_, "MET_et_soft/D");

    MyTree.Branch("MET_mu_etx", &MET_mu_etx_,     "MET_mu_etx/D");
    MyTree.Branch("MET_mu_ety", &MET_mu_ety_,     "MET_mu_ety/D");
    MyTree.Branch("MET_mu_sumet", &MET_mu_sumet_, "MET_mu_sumet/D");

    MyTree.Branch("MET_ph_etx", &MET_ph_etx_,     "MET_ph_etx/D");
    MyTree.Branch("MET_ph_ety", &MET_ph_ety_,     "MET_ph_ety/D");
    MyTree.Branch("MET_ph_sumet", &MET_ph_sumet_, "MET_ph_sumet/D");

    MyTree.Branch("MET_trk_etx", &MET_trk_etx_,     "MET_trk_etx/D");
    MyTree.Branch("MET_trk_ety", &MET_trk_ety_,     "MET_trk_ety/D");
    MyTree.Branch("MET_trk_phi", &MET_trk_phi_,     "MET_trk_phi/D");
    MyTree.Branch("MET_trk_sumet", &MET_trk_sumet_, "MET_trk_sumet/D");

    MyTree.Branch("MET_noEM_et", &MET_noEM_et_, "MET_noEM_et/D");
    MyTree.Branch("MET_noEM_phi", &MET_noEM_phi_,     "MET_noEM_phi/D");
    MyTree.Branch("MET_noEM_sumet", &MET_noEM_sumet_, "MET_noEM_sumet/D");

    MyTree.Branch("min_dphi_jetMET", &min_dphi_jetMET, "min_dphi_jetMET/F");
    MyTree.Branch("min_dphi_jetMET_zee", &min_dphi_jetMET_zee, "min_dphi_jetMET_zee/F");

    MyTree.Branch("max_dphi_jetjet", &max_dphi_jetjet_, "max_dphi_jetjet/F");
    MyTree.Branch("average_dphi_jetjet", &average_dphi_jetjet_, "average_dphi_jetjet/F");
    MyTree.Branch("average_dphi_jetjet_now", &average_dphi_jetjet_now_, "average_dphi_jetjet_now/F");
    MyTree.Branch("max_dr_jetjet", &max_dr_jetjet_, "max_dr_jetjet/F");
    MyTree.Branch("average_dr_jetjet", &average_dr_jetjet_, "average_dr_jetjet/F");
    MyTree.Branch("average_dr_jetjet_now", &average_dr_jetjet_now_, "average_dr_jetjet_now/F");

    // Flags of event selection
    MyTree.Branch("flag_dphi_jetMET", &flag_dphi_jetMET, "flag_dphi_jetMET/O");
    MyTree.Branch("flag_el_veto", &flag_el_veto, "flag_el_veto/O");
    MyTree.Branch("flag_mu_veto", &flag_mu_veto, "flag_mu_veto/O");
    MyTree.Branch("flag_trk_veto", &flag_trk_veto, "flag_trk_veto/O");
    MyTree.Branch("flag_4thjet_veto", &flag_4thjet_veto, "flag_4thjet_veto/O");
    MyTree.Branch("has_badMuon", &has_bad_muon_, "has_badMuon/O");
    MyTree.Branch("has_cosmicMuon", &has_cosmic_muon_, "has_cosmicMuon/O");
}

void OutputTreeMan::AddEventInfo2Tree(TTree& tree)
{
    tree.Branch("RunNumber", &run_number_, "RunNumber/I");
    tree.Branch("EventNumber", &event_number_, "EventNumber/I");
    tree.Branch("mc_channel_number", &mc_channel_number_, "mc_channel_number/I");
    tree.Branch("MCWeight", &mc_weight_, "MCWeight/F");
}

void OutputTreeMan::FillJet(const xAOD::Jet& jet)
{
    jet_p4_.push_back(jet.p4());
    jet_dphi_MET.push_back( (float) jet.auxdata< double >("dphi_MET") );
    vector<float> jvf;
    jvf.clear();
    jet.getAttribute(xAOD::JetAttribute::JVF, jvf);
    if (jvf.size() > 0){
        jet_jvf0_.push_back(jvf.at(0));
        jet_jvf1_.push_back(jvf.at(1));
    }
    float jvt = jet.auxdecor<float>("Jvt");
    // jet.getAttribute(xAOD::JetAttribute::JVT, jvt);
    // calculate the JVT
    // float jvt = pjvtag_->updateJvt(jet);
    jet_jvt_.push_back(jvt);
}

void OutputTreeMan::FillEventInfo(const xAOD::EventInfo& ei)
{
    run_number_ = ei.runNumber();
    event_number_= ei.eventNumber();

    if(ei.eventType(xAOD::EventInfo::IS_SIMULATION)) {
        mc_channel_number_ = ei.mcChannelNumber();
        const vector<float>& weights = ei.mcEventWeights();
        if(weights.size() > 0) mc_weight_ = weights[0];
    }
    actualIPC_ = ei.actualInteractionsPerCrossing();
    averageIPC_ = ei.averageInteractionsPerCrossing();
    bcid_ = ei.bcid();
    lumiBlock_ = ei.lumiBlock();
}

void OutputTreeMan::FillElectron(const xAOD::Electron& electron, 
        const xAOD::EventInfo& evtInfo,
        const xAOD::VertexContainer& vertices,
        bool is_signal, float sf)
{
    n_base_el ++;
    if(is_signal) n_good_el ++;
    el_is_good_.push_back(is_signal);
    bool pass_iso = (bool) iso_trackonly_->accept(electron);
    el_iso_trackonly_.push_back(pass_iso);
    if(is_signal && pass_iso) n_good_iso_el ++;
    el_good_p4_.push_back(electron.p4());
    el_sf_.push_back(sf);
    float d0 = -999, z0 = -999, zp = -999;
    CPToolsHelper::GetTrackQuality(electron, evtInfo, vertices, d0, z0, zp);
    el_d0_.push_back(d0);
    el_z0_.push_back(z0);
    el_zp_.push_back(zp);
    el_charge_.push_back(electron.charge());
}

void OutputTreeMan::FillElectronScaleSys(const string& sys_name, float sf_sys){
    if(sys_name.find("up") != string::npos){
        el_sf_up_.push_back(sf_sys);
    } else {
        el_sf_down_.push_back(sf_sys);
    }
}

void OutputTreeMan::FillMuon(const xAOD::Muon& muon, 
        const xAOD::EventInfo& evtInfo,
        const xAOD::VertexContainer& vertices,
        bool is_signal, float sf)
{
    n_base_mu ++;
    if(!is_signal) return ;
    n_good_mu ++;
    mu_is_good_.push_back(is_signal);
    mu_iso_trackonly_.push_back((bool) iso_trackonly_->accept(muon) ); 
    mu_good_p4_.push_back(muon.p4());
    mu_sf_.push_back(sf);
    float d0 = -999, z0 = -999, zp = -999;
    CPToolsHelper::GetTrackQuality(muon, evtInfo, vertices, d0, z0, zp);
    mu_d0_.push_back(d0);
    mu_z0_.push_back(z0);
    mu_zp_.push_back(zp);
    mu_charge_.push_back(muon.charge());
}

void OutputTreeMan::FillMuonScaleSys(const string& sys_name, float sf_sys)
{
    if(sys_name.find("SYS") != string::npos){
        if(sys_name.find("up") != string::npos){
            mu_sys_up_.push_back(sf_sys);
        }else{
            mu_sys_down_.push_back(sf_sys);
        }
    } else if (sys_name.find("STAT") != string::npos){
        if(sys_name.find("up") != string::npos){
            mu_stat_up_.push_back(sf_sys);
        }else{
            mu_stat_down_.push_back(sf_sys);
        }
    } else if (sys_name.find("Trig") != string::npos){
        if(sys_name.find("up") != string::npos){
            mu_trigger_up_.push_back(sf_sys);
        }else{
            mu_trigger_down_.push_back(sf_sys);
        }
    } else {
        // cout << "ERROR: I do not know: " << sys_name.c_str() << endl;
        log_err("I do not know %s", sys_name.c_str());
    }
}

void OutputTreeMan::FillWZ(const vector<TLorentzVector>& good_p4_, double& wlnu, double& zll, bool is_electron)
{
    if (good_p4_.size() == 1){
        wlnu = MonoJet::Helper::get_mt(good_p4_.at(0), MET_mu_etx_, MET_mu_ety_);
    }

    if(good_p4_.size() == 2){
        const auto& zee = good_p4_.at(0) + good_p4_.at(1);
        zll = zee.M();

        if (is_electron)
        {
            px_zee_ = zee.Px(); 
            py_zee_ = zee.Py();
            met_ecorr_px_ = MET_mu_etx_ + px_zee_;
            met_ecorr_py_ = MET_mu_ety_ + py_zee_;
            MET_et_noEE = sqrt(met_ecorr_px_*met_ecorr_px_ + 
                    met_ecorr_py_*met_ecorr_py_);
        }
    }
}

void OutputTreeMan::FillMonoJet()
{
    bool is_electron = true;
    this->FillWZ(el_good_p4_, mt_elnu, m_elel, is_electron);
    is_electron = false;
    this->FillWZ(mu_good_p4_, mt_munu, m_mumu, is_electron);

    // minimum dphi
    if (jet_dphi_MET.size() > 0){
        min_dphi_jetMET = *min_element(jet_dphi_MET.begin(), jet_dphi_MET.end());
    }

    flag_dphi_jetMET = min_dphi_jetMET > MyCuts::dphi_jetMET;
    flag_4thjet_veto = n_good_jet <= MyCuts::max_good_jets;
    flag_el_veto = n_base_el < 1;
    flag_mu_veto = n_base_mu < 1;
    flag_trk_veto = n_good_trk < 1;

    // sum of pt of jets (scalar sum and vector sum)
    // averaged weight of dphi(other-jet, leading-jet)
    // averaged delta_R (other-jet, leading-jet)
    max_dphi_jetjet_ = -99.;
    average_dphi_jetjet_ = -99.;
    average_dphi_jetjet_now_ = -99;
    max_dr_jetjet_ = -99.;
    average_dr_jetjet_ = -99.;
    average_dr_jetjet_now_ = -99;
    if (jet_p4_.size() > 0)
    {
        average_dphi_jetjet_ = 0;
        average_dr_jetjet_now_ = 0;

        ht_scalar_ = 0;
        double px_sum = 0, py_sum = 0;
        TVector2 lead_pt_vec( jet_p4_.at(0).Px(), 
                jet_p4_.at(0).Py());
        TLorentzVector& lead_pt_tlv = jet_p4_.at(0);

        double weighted_dr_jetjet = 0;
        double dr_jetjet_now = 0;
        double jet_mass_all = 0;
        double weighted_dphi_jetjet = 0;
        double dphi_jetjet_now = 0;
        for(size_t i = 0; i < jet_p4_.size(); i ++){
            TLorentzVector& lorentz = jet_p4_.at(i);
            ht_scalar_ += (float) lorentz.Pt();
            px_sum += lorentz.Px();
            py_sum += lorentz.Py();
            if ((int)i != 0){
                TVector2 other_jet_vec(lorentz.Px(), lorentz.Py());
                // take absolute values
                double dphi_jetjet = fabs(other_jet_vec.DeltaPhi(lead_pt_vec));
                dphi_jetjet_now += dphi_jetjet;
                weighted_dphi_jetjet += dphi_jetjet*lorentz.Pt();
                if(dphi_jetjet > max_dphi_jetjet_) max_dphi_jetjet_ = (float) dphi_jetjet;
                double delta_r_jetjet = lorentz.DeltaR(lead_pt_tlv);
                if(delta_r_jetjet > max_dr_jetjet_) max_dr_jetjet_ = delta_r_jetjet;
                weighted_dr_jetjet += lorentz.M()*delta_r_jetjet;
                jet_mass_all += lorentz.M();
                dr_jetjet_now += delta_r_jetjet;
            }
        }
        if(jet_p4_.size() > 1) {
            average_dphi_jetjet_now_ = dphi_jetjet_now/(jet_p4_.size() - 1);
            average_dr_jetjet_now_ = dr_jetjet_now/(jet_p4_.size() -1);
        }
        average_dphi_jetjet_ = (float) weighted_dphi_jetjet/(ht_scalar_ - lead_pt_vec.Mod());
        average_dr_jetjet_ = (float) weighted_dr_jetjet/jet_mass_all;
        ht_vector_ = (float) sqrt(px_sum*px_sum + py_sum*py_sum);
    }

    // correct the dphi for Zee samples
    if (MET_noEM_et_ != -999){
        min_dphi_jetMET_zee = 999;
        for(const auto& jet_p4 : jet_p4_){
            double dphi = fabs(TVector2::Phi_mpi_pi(MET_noEM_phi_ - jet_p4.Phi()));
            if(dphi < min_dphi_jetMET_zee) {
                min_dphi_jetMET_zee = (float) dphi;
            }
        }
    }
}
