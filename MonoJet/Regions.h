#ifndef __MONOJET_REGIONS_H_
#define __MONOJET_REGIONS_H_

#include "MonoJet/physics.h"
#include "MyXAODTools/CPToolsHelper.h"

typedef enum {
    SR=0, WENU, WMUNU, ZMM, ZEE, NUM_REGIONS 
} RegionType;

class Regions{
    public:
        Regions();
        Regions(physics* p);
        Regions(physics* p, float jet_pt_cut, 
                float met_cut, int njet_cut);
        ~Regions();
        void SetCuts(float jet_pt_cut, float met_cut, int njet_cut);
        void SetNtuple(physics* p);
        void clear();
        RegionType GetRegionType();
        static const char* GetRegionName(RegionType type);
        void CorrectMET();

        void Verbose() { verbose_ = true;}
        void SetNoDphiCut() { nodphi_ = true;}
        void SetDataFlag() { is_data_ = true; }
        void SetNoTrigger() { trigger_ = false; }

        bool PassCommonCuts(double met_value, double dphi_jet_met);

        bool IsSR();
        bool IsCRWen();
        bool IsCRWmn();
        bool IsCRZmm();
        bool IsCRZee();

        void AddIsolation2Lep();
        void ApplyIso();
        void ApplyTrack();
        void ApplyIsoTrack();

        bool PassElTrigger();
        bool PassMuTrigger();
        bool PassMETTrigger();
        bool PassGRL();
        bool PassBadTight();

    private:
        physics* p_;
        bool verbose_;
        bool nodphi_;
        bool is_data_;
        bool trigger_;

        float jet_pt_cut_;
        float met_cut_;
        int njet_cut_;
        int pass_common_;

        float new_met_et_;
        float new_dphi_;
        float new_mt_el_;
        float new_mt_mu_;
        CPToolsHelper* cp_helper_;

        int n_good_mu_iso ;
        int n_good_mu_d0z0;
        int n_good_mu_all;
        int n_good_el_iso;
        int n_good_el_d0z0;
        int n_good_el_all;
};
#endif

