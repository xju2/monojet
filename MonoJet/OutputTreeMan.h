#ifndef _MONOJET_OUTPUTTREEMAN_
#define _MONOJET_OUTPUTTREEMAN_

#include <vector>
#include <map>
#include <TTree.h>
#include "xAODJet/JetContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODEgamma/Photon.h"

#include "IsolationSelection/IsolationSelectionTool.h"
#include "JetMomentTools/JetVertexTaggerTool.h"

#include "MonoJet/MyCuts.h"

using namespace std;
class OutputTreeMan {

public:
    OutputTreeMan(bool with_sys = false);
    virtual ~OutputTreeMan();
    void AddBranches2Tree(TTree& tree);
    void AddEventInfo2Tree(TTree& tree);
    void ClearBranch();
    void FillJet(const xAOD::Jet& jet);
    void FillEventInfo(const xAOD::EventInfo& ei);

    void FillElectron(const xAOD::Electron& electron, 
            const xAOD::EventInfo& evtInfo,
            const xAOD::VertexContainer& vertices,
            bool is_signal, float sf);
    void FillElectronScaleSys(const string& sys_name, float sf_sys);

    void FillMuon(const xAOD::Muon& mu, 
            const xAOD::EventInfo& evtInfo,
            const xAOD::VertexContainer& vertices,
            bool is_signal, float sf);
    void FillMuonScaleSys(const string& sys_name, float sf_sys);

    void FillPhoton(const xAOD::Photon& ph, bool is_signal, float sf);

    void FillMonoJet();
    void FillWZ(const vector<TLorentzVector>& good_p4, double& wlnu, double& zll, bool is_electron);

public:
    const char* APP_NAME ;
    // branches for the output
    int run_number_;
    int event_number_;
    int mc_channel_number_;
    float mc_weight_;
    float pu_weight_;
    float corrected_mu_;
    float data_weight_;
    int n_pv_;
    float actualIPC_; // actual interaction per crossing for the current BCID -- for in-time pile-up
    float averageIPC_;// average interaction per corssing for all BCIDs -- for out-of-time pile-up
    uint32_t bcid_;
    uint32_t lumiBlock_;

    //////////////////
    // Trigger Info
    //////////////////
    map<string, bool> trigger_map_;

    //////////////////
    // Jets Info
    //////////////////
    vector<TLorentzVector> jet_p4_;
    std::vector<float> jet_dphi_MET;
    std::vector<float> jet_jvf0_;
    std::vector<float> jet_jvf1_;
    vector<float> jet_jvt_;

    int n_jet_btagged;
    int n_base_jet;
    int n_good_jet;
    float ht_scalar_; // scalar sum of Pt of jets
    float ht_vector_; // vector sum of Pt of jets

    int n_good_el;
    int n_good_iso_el;
    int n_base_el;
    double mt_elnu;
    double m_elel;
    double px_zee_, py_zee_;
    vector<bool> el_is_good_;
    vector<bool> el_medium_;
    vector<bool> el_iso_trackonly_;
    vector<float> el_d0_;
    vector<float> el_z0_;
    vector<float> el_zp_;
    vector<float> el_charge_;
    vector<TLorentzVector> el_good_p4_;
    vector<float> el_sf_;
    vector<float> el_sf_up_;
    vector<float> el_sf_down_;


    int n_base_mu;
    int n_good_mu;
    double mt_munu;
    double m_mumu;
    vector<TLorentzVector> mu_good_p4_;
    vector<bool> mu_is_good_;
    vector<bool> mu_iso_trackonly_;
    vector<float> mu_d0_;
    vector<float> mu_z0_;
    vector<float> mu_zp_;
    vector<float> mu_charge_;
    vector<float> mu_sf_;
    vector<float> mu_stat_up_;
    vector<float> mu_stat_down_;
    vector<float> mu_sys_up_;
    vector<float> mu_sys_down_;
    vector<float> mu_trigger_up_;
    vector<float> mu_trigger_down_;

    int n_good_trk;

    double MET_et_noEE;
    double met_ecorr_px_, met_ecorr_py_;
    // double met_ecorr_phi_;
    float min_dphi_jetMET;
    float min_dphi_jetMET_zee;
    float average_dphi_jetjet_;
    float average_dphi_jetjet_now_;
    float max_dphi_jetjet_;
    float average_dr_jetjet_;
    float average_dr_jetjet_now_;
    float max_dr_jetjet_;
    float dphi_leading_jetMET;

    double MET_etx;
    double MET_ety;
    double MET_phi;
    double MET_et;
    double MET_sumet_;

    double MET_mu_etx_;
    double MET_mu_ety_;
    double MET_mu_sumet_;

    double MET_ph_etx_;
    double MET_ph_ety_;
    double MET_ph_sumet_;

    double MET_trk_etx_;
    double MET_trk_ety_;
    double MET_trk_phi_;
    double MET_trk_sumet_;

    double MET_noEM_et_;
    double MET_noEM_phi_;
    double MET_noEM_sumet_;

    bool flag_dphi_jetMET;
    bool flag_el_veto;
    bool flag_mu_veto;
    bool flag_trk_veto;
    bool flag_4thjet_veto;
    bool has_bad_muon_;
    bool has_cosmic_muon_;

private:
    CP::IsolationSelectionTool* iso_trackonly_;
    bool with_sys_ ; 
    int initializeTools();
};
#endif
