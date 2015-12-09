#ifndef __MONOJET_CUTFLOW_H_
#define __MONOJET_CUTFLOW_H_

#include "MonoJet/physics.h"
#include "MonoJet/Regions.h"
#include "MyXAODTools/EventCounter.h"
#include <TH1F.h>
#include <string>

using namespace std;
class CutFlow
{
    private:
        bool is_mc;
        double lumi;
        EventCounter* myalg;
        physics* _p; 
        TFile* outfile;
        Regions* region_;

        void setCutFlowLabel(TH1F*, std::string& region);
        void fillCutFlow(TH1F*, physics* _p, std::string& region, double);
    public:
        explicit CutFlow(const char* filename, bool, const char* outname, double lumi=5000.);
        ~CutFlow();
        void getCutFlows(int channel);
};

#endif
