/*
 * =====================================================================================
 *
 *       Filename:  constants.h
 *
 *    Description: declear the cuts used in the analysis
 *
 *
 * =====================================================================================
 */
#ifndef MONOJET_constants_H
#define MONOJET_constants_H
class MyCuts
{
    public:
        MyCuts();
        ~MyCuts ();
        static const float badjet_pt;

        //cuts for each object, sush as jet, electron, muon
        static const float jet_pt;
        static const float jet_eta;

        static const float ele_pt;
        static const float ele_eta;
        static const float muon_pt;
        static const float muon_eta;

        //specific cuts for mono-jet
        static const float leading_jet_pt;
        static const float leading_jet_eta;
        //in 13 TeV analysis, met cut is the same as leading jet pt cut
        static const float met;
        static const float dphi_jetMET;
        static const int max_good_jets;
};
#endif
