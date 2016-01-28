#ifndef __MONOJET_SMEAREDINFO_H_
#define __MONOJET_SMEAREDINFO_H_

typedef struct SmearedInfo 
{
    float leading_jet_pt_;
    float leading_jet_eta_;
    float sub_leading_jet_pt_;
    float sub_leading_jet_eta_;
    float met_;
    float sum_et_;
    float min_jets_met_;
    float dphi_EP_;
    int n_good_jets_;
} SmearedInfo;

#endif
