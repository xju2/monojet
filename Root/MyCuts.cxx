#include <MonoJet/MyCuts.h>

MyCuts::MyCuts(){}
MyCuts::~MyCuts(){}

const float MyCuts::badjet_pt = 20e3;
const float MyCuts::jet_pt = 30e3;
const float MyCuts::jet_eta = 2.8;

const float MyCuts::ele_pt = 20e3;
const float MyCuts::ele_eta = 2.47;
const float MyCuts::muon_pt = 10e3;
const float MyCuts::muon_eta = 2.5;


const float MyCuts::leading_jet_pt = 250e3;
const float MyCuts::leading_jet_eta = 2.8;

const float MyCuts::met = 250e3;
const float MyCuts::dphi_jetMET = 0.4;

const int MyCuts::max_good_jets = 4;
