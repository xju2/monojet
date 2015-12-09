#include <stdlib.h>
#include <vector>
#include <TString.h>
#include <iostream>

#include "MonoJet/CutFlow.h"
#include "MonoJet/Regions.h"

using namespace std;
CutFlow::CutFlow(const char* _filename, bool _is_mc, const char* outname, double _lumi):
    is_mc(_is_mc),
    lumi(_lumi)
{
    myalg = new EventCounter(false, is_mc);
    myalg->GetTotalEventsDic(_filename, "associate");
    myalg ->printTotalEvents();

    TChain* ch = EventCounter::getChain(_filename, "physics");
    _p = new physics(ch);
    outfile = TFile::Open(outname, "recreate");
    region_ = new Regions(_p);
    if(!is_mc){
        cout << "Running Data, Congrats" << endl;
        region_->SetDataFlag();
    }
}

CutFlow::~CutFlow()
{
    delete _p;
    delete myalg;
    if(outfile->IsOpen()) outfile->Close();
    delete region_;
}

void CutFlow::getCutFlows(int channel)
{
    Long64_t nentries = _p->fChain->GetEntries();
    vector<string>* region_types = new vector<string>();
    region_types->push_back("SR");
    region_types->push_back("CRWmunu");
    region_types->push_back("CRWenu");
    region_types->push_back("CRZmumu");
    region_types->push_back("CRZee");
    int total_regions = (int)region_types->size();
    outfile->cd();
    TH1F* h_cutflow_tmp = new TH1F( "h_cutflow_tmp","h_cutflow",100,0.5,100.5 );
    std::vector<TH1F*>* hists_vec = new std::vector<TH1F*>();
    for(int i = 0; i < total_regions; i++){
        TH1F* h1 = dynamic_cast<TH1F*>(h_cutflow_tmp->Clone(Form("cutflow_%s",region_types->at(i).c_str())));
        setCutFlowLabel(h1, region_types ->at(i));
        hists_vec ->push_back(h1);
        h1 ->Fill(1, myalg->getTotalEventsNoWeight(channel));

        if(is_mc){
            TH1F* h2 = dynamic_cast<TH1F*>(h_cutflow_tmp->Clone(Form("cutflow_%s_weight",region_types->at(i).c_str())));
            h2 ->Fill(1, lumi*myalg->getCrossSection(channel));
            setCutFlowLabel(h2, region_types ->at(i));
            hists_vec ->push_back(h2);

            TH1F* h3 = dynamic_cast<TH1F*>(h_cutflow_tmp->Clone(Form("cutflow_%s_nomcweight",region_types->at(i).c_str())));
            h3 ->Fill(1, lumi*myalg->getCrossSection(channel));
            setCutFlowLabel(h3, region_types ->at(i));
            hists_vec ->push_back(h3);
        }
    }

    // Regions* region = new Regions(_p);
    for(Long64_t ientry = 0; ientry < nentries; ientry ++)
    {
		_p->fChain->LoadTree(ientry );
		_p->fChain->GetEntry(ientry );
        
        int mc_id = -9999;
        if(is_mc) { 
            mc_id = _p->mc_channel_number;
            if(mc_id != channel) continue;
        }

        region_->clear();
        for(int i = 0; i < total_regions; i++){
            if(is_mc){ 
                fillCutFlow(hists_vec->at(i*3), _p, region_types->at(i), 1.0);
                double weight = lumi*myalg->getCrossSection(mc_id)/myalg->getTotalEvents(mc_id)*_p->MCWeight;
                fillCutFlow(hists_vec->at(i*3+1), _p, region_types->at(i), weight);
                double weight2 = lumi*myalg->getCrossSection(mc_id)/myalg->getTotalEventsNoWeight(mc_id);
                fillCutFlow(hists_vec->at(i*3+2), _p, region_types->at(i), weight2);
            }else{
                fillCutFlow(hists_vec->at(i), _p, region_types->at(i), 1.0);
            }
        }
    }
    // delete region;
    outfile->Write();
    outfile->Close();
    delete region_types;
}

void CutFlow::setCutFlowLabel(TH1F* h_cutflow, std::string& region)
{
    h_cutflow->GetXaxis()->SetBinLabel(  1, "Initial            " );
    h_cutflow->GetXaxis()->SetBinLabel(  2, "Jet cleaning       " );
    h_cutflow->GetXaxis()->SetBinLabel(  3, "Leading jet kine   " );
    h_cutflow->GetXaxis()->SetBinLabel(  4, "MET                " );
    h_cutflow->GetXaxis()->SetBinLabel(  5, "Leading jet quality" );
    int flow_index = 8;
    if (region.compare("SR") == 0){
        h_cutflow->GetXaxis()->SetBinLabel(  6, "Ele veto" );
        h_cutflow->GetXaxis()->SetBinLabel(  7, "Muo veto" );
    }else if(region.compare("CRWmunu") == 0){
        h_cutflow->GetXaxis()->SetBinLabel(  6, "Ele veto" );
        h_cutflow->GetXaxis()->SetBinLabel(  7, "Muo = 1 " );
        h_cutflow->GetXaxis()->SetBinLabel(  8, "30 < mT < 100" );
        flow_index = 9;
    }else if(region.compare("CRWenu") == 0){
        h_cutflow->GetXaxis()->SetBinLabel(  6, "Ele = 1" );
        h_cutflow->GetXaxis()->SetBinLabel(  7, "Muo veto" );
    }else if(region.compare("CRZmumu") == 0){
        h_cutflow->GetXaxis()->SetBinLabel(  6, "Ele = 0" );
        h_cutflow->GetXaxis()->SetBinLabel(  7, "Muo = 2" );
        h_cutflow->GetXaxis()->SetBinLabel(  8, "66 < m_mumu < 116" );
        flow_index = 9;
    }else if(region.compare("CRZee") == 0){
        h_cutflow->GetXaxis()->SetBinLabel(  6, "Ele = 2" );
        h_cutflow->GetXaxis()->SetBinLabel(  7, "Muo = 0" );
        h_cutflow->GetXaxis()->SetBinLabel(  8, "MET > 150" );
        h_cutflow->GetXaxis()->SetBinLabel(  9, "66 < m_ee < 116" );
        flow_index = 10;
    }
    h_cutflow->GetXaxis()->SetBinLabel(  flow_index++, "4th jet veto       " );
    h_cutflow->GetXaxis()->SetBinLabel(  flow_index++, "DPhi MET jet       " );
    h_cutflow->GetXaxis()->SetBinLabel(  flow_index++, "MET > 400" );
    h_cutflow->GetXaxis()->SetBinLabel(  flow_index++, "p^{t}_{j1} > 400" );
    h_cutflow->GetXaxis()->SetBinLabel(  flow_index++, "Trigger" );
    h_cutflow->GetXaxis()->SetBinLabel(  flow_index++, "GRL" );
}

void CutFlow::fillCutFlow(TH1F* h1, physics* _p, 
        std::string& region, double weight)
{
    if(region.compare("CRZee") == 0){
        if(! region_->PassElTrigger()) return;
    } else {
        if(! region_->PassMETTrigger()) return;
    }
    h1 ->Fill(2, weight);
    double met = region.compare("CRZee") == 0 ? _p->MET_noEM_et: _p->MET_et;
    _p->flag_dphi_jetMET = region.compare("CRZee") == 0 ? _p->min_dphi_jetMET_zee > 0.4 : _p->flag_dphi_jetMET;
    
    double jet_leading_pt = _p->n_good_jet >= 1?(_p->jet_p4->at(0)).Pt():-1;
    if(region_->PassBadTight() && jet_leading_pt > 150e3) // Jet kinematic cut
    {
        h1 ->Fill(3, weight);
        if(met > 150e3 || region.compare("CRZee") == 0)
        {  // MET cut
            h1 ->Fill(4, weight);
            // if(_p ->flag_jet_qual)
            if(true)
            { // Jet Quality cut
                h1 ->Fill(5, weight);
                int flow_index = 8;
                bool has_passed = false;
                bool pass_trigger = false;
                if (region.compare("SR") == 0){
                    // In signal region
                    if(_p ->flag_el_veto){
                        h1 ->Fill(6, weight);
                        if(_p ->flag_mu_veto){
                            h1 ->Fill(7, weight);
                            has_passed = true;
                        }
                    }
                    pass_trigger = region_->PassMETTrigger();
                }else if(region.compare("CRWmunu") == 0){
                    // In control region of W->munu
                    if(_p ->flag_el_veto){
                        h1 ->Fill(6, weight);
                        if(_p ->n_good_mu == 1 && _p->n_base_mu == 1){
                            h1 ->Fill(7, weight);
                            double mT_mu = _p ->mt_munu/1e3;
                            // has_passed = true;
                            // flow_index ++;
                            if(mT_mu > 30 && mT_mu < 100){
                                h1 ->Fill(flow_index++, weight);
                                has_passed = true;
                            }
                        }
                    }
                    pass_trigger = region_->PassMETTrigger();//  || region_->PassMuTrigger();
                }else if(region.compare("CRWenu") == 0){
                    // In CR of W->enu
                    if(_p ->n_good_iso_el == 1 && _p->n_base_el == 1){
                        h1 ->Fill(6, weight);
                        if(_p ->flag_mu_veto){
                            h1 ->Fill(7, weight);
                            has_passed = true;
                        }
                    }
                    pass_trigger = region_->PassMETTrigger() || region_->PassElTrigger();
                }else if(region.compare("CRZmumu") == 0){
                    if(_p ->flag_el_veto){
                        h1 ->Fill(6, weight);
                        if(_p ->n_good_mu == 2 && _p->n_base_mu == 2){
                            h1 ->Fill(7, weight);
                            double m_mumu = _p->m_mumu/1e3;
                            if(m_mumu > 66 && m_mumu < 116){
                                h1 ->Fill(flow_index++, weight);
                                has_passed = true;
                            }
                        }
                    }
                    pass_trigger = region_->PassMuTrigger();
                }else if(region.compare("CRZee") == 0){
                    if(_p ->n_good_el == 2 && _p->n_base_el == 2){
                        h1 ->Fill(6, weight);
                        if(_p ->flag_mu_veto){
                            h1 ->Fill(7, weight);
                            if(met > 150e3){
                                h1 ->Fill(flow_index++, weight);
                                double m_elel = _p->m_elel/1e3;
                                if(m_elel > 66 && m_elel < 116){
                                // if(true){
                                    h1 ->Fill(flow_index++, weight);
                                    has_passed = true;
                                }
                            }
                        }
                    }
                    pass_trigger = region_->PassElTrigger();
                }

                if(_p ->flag_4thjet_veto && has_passed){
                    h1 ->Fill(flow_index++, weight);
                    if(_p ->flag_dphi_jetMET){
                        h1 ->Fill(flow_index++, weight);
                        double ljet_pt = (_p->jet_p4->at(0)).Pt() /1e3;
                        int trigger_index = flow_index+2;
                        if(met > 400e3){
                            h1 ->Fill(flow_index++, weight);
                            if(ljet_pt > 400){
                                h1 ->Fill(flow_index++, weight);
                            }
                        }
                        // add trigger
                        if(pass_trigger) {
                            h1->Fill(trigger_index++, weight);
                        }
                        if(region_->PassGRL()){
                            h1 ->Fill(trigger_index, weight);
                        }
                    }
                }
            }
        }
    }
}

