#include "MonoJet/PrescaleFactorTool.h"
#include "MonoJet/dbg.h"

#include <TString.h>

#include <stdexcept>
#include <iostream>
#include <fstream>
#include <sstream>

float PrescaleFactorTool::getWeight(int run, const string& trigger, UInt_t lbn)
{
    if(run != run_) {
        run_ = run;
        if(!load()) return 0.0;
    }
    if(trigger == "") return 0.0;
    float results = 0.0;
    int lbn_i = (int) lbn;
    try {
        for(const auto item : ps_dic_.at(trigger)){
            if(lbn_i < item.first){
                break;
            } else {
                results = item.second;
            }
        }
    } catch(const out_of_range& oor) {
        cout << trigger << " does not exist in PS dic" << endl;
    }
    // cout<<"getWeight: " << trigger << " " << lbn << " " << results << endl;
    return results;
}

string PrescaleFactorTool::getTrigger( const map<string, bool>& input, float pt)
{
    try{
        if(input.at("HLT_j400") && pt > 430){
            return "HLT_j400";
        } else if( input.at("HLT_j360") && pt > 390 && pt <= 430) {
            return "HLT_j360";
        } else if( input.at("HLT_j320") && pt > 350 && pt <= 490) {
            return "HLT_j320";
        } else if( input.at("HLT_j260") && pt > 280 && pt <= 350) {
            return "HLT_j260";
        } else if( input.at("HLT_j200") && pt > 220 && pt <= 280) {
            return "HLT_j200";
        } else if( input.at("HLT_j150") && pt > 170 && pt <= 220) {
            return "HLT_j150";
        } else if( input.at("HLT_j110") && pt > 130 && pt <= 170) {
            return "HLT_j110";
        } else if( input.at("HLT_j60") && pt > 80 && pt <= 130) {
            return "HLT_j60";
        } else {
            // cout <<"cannot find proper trigger"<<endl;
            return "";
        }
    } catch(const out_of_range& oor) {
        log_err("Check input for getTrigger");
        throw out_of_range("Check input");
        // return "";
    }
}

bool PrescaleFactorTool::load()
{
    ps_dic_.clear();
    const string& file_name(Form("%s/%d_ps.txt",file_path_.c_str(), run_));
    ifstream fin(file_name.c_str(), ifstream::in);
    if(fin.fail() || fin.bad()) {
        log_err("Cannot open %s", file_name.c_str());
        return false;
    }
    string line;
    string trigger;
    int lbn;
    float ps;
    map<int, float> lb_dic;
    while ( getline(fin, line) ) {
        if(line.empty()) continue;
        if(line[0] == '[') {
            if(!lb_dic.empty()) {
                ps_dic_[trigger] = lb_dic;
                lb_dic.clear();
            }
            trigger = line.substr(1, line.length()-2);
        } else {
            size_t pos_coma = line.find(',');
            line.replace(pos_coma, 1, 1, ' ');
            istringstream iss(line);
            string dummy;
            iss >> dummy >> lbn >> dummy >> ps;
            lb_dic[lbn] = ps;
        }
    }
    return true;
}
void PrescaleFactorTool::test()
{
    if(ps_dic_.empty()){
        return;
    }
    for(const auto item : ps_dic_) {
        cout <<"trigger: "<< item.first << endl;
        for(const auto lbInfo : item.second) {
            cout<<"LB " << lbInfo.first << " PS: " << lbInfo.second << endl;
        }
    }
    return;
}
