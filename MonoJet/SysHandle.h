#ifndef _MONOJET_SYSHANDLE_H_
#define _MONOJET_SYSHANDLE_H_

#include <string>
#include <vector>
#include <TH1F.h>

#include "MyXAODTools/EventCounter.h"
#include "MonoJet/Regions.h"

using namespace std;

class SysHandle { 
private:
    bool debug_;
    string sample_name_;
    float met_cut_; 
    float jet_pt_cut_;
    EventCounter* myalg;
    Regions* region;
    vector<float>* exp_events_;
    vector<string>* sys_names_;
    string append_name_;

public:
    explicit SysHandle(const char* sample_name);
    ~SysHandle();
    
    void SetDebug();
    void SetCuts(float met_cut, float jet_pt_cut, int njet_cut = 4);
    void GetYields(const string& norm_dir, const string& sys_dir);
    void GetHist(const char* output_name);
    void PrintYields();
    void SetSampleName(const string& sample_name){
        sample_name_ = sample_name;
    }

private:
    bool ProcessTree(const string& tree_name, const string& dir);
    TH1F* CreateSysHist(int i, int j);
    TH1F* CreateHist(float yield, const string& name);
};
#endif
