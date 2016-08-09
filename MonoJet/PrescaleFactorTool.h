#ifndef __MONOJET_PrescaleFactorTool_H__
#define __MONOJET_PrescaleFactorTool_H__

#include "TSystem.h"

#include <string>
#include <map>
#include <vector>
#include <utility>

using namespace std;
class PrescaleFactorTool{
public:
    PrescaleFactorTool(const string& file_path=gSystem->ExpandPathName("$ROOTCOREBIN/data/MonoJet/ps_per_lb")){
        file_path_ = file_path;
        run_ = -999;
    }

    ~PrescaleFactorTool() {}
    float getWeight(int run, const string& trigger, UInt_t lbn);
    static string getTrigger(const map<string, bool>& input, float leading_jet_pt);
    void test();
private:
    string file_path_;
    // key: trigger, lumi, prescale_factor
    map<string, map<int, float> > ps_dic_;
    int run_;

    //methods
    bool load();
};
#endif
