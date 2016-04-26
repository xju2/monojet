#include <stdlib.h>

#include <TString.h>
#include <TChain.h>
#include <iostream>
#include <sstream>
#include <TH2F.h>
#include <TH1F.h>

#include "/afs/cern.ch/user/x/xju/tool/AtlasUtils.C"
#include "/afs/cern.ch/user/x/xju/tool/AtlasStyle.C"
#include "/afs/cern.ch/user/x/xju/tool/loader.c"

using namespace std;
const float weight = 3210;
const string base_dir = "combined/";

const float min_dphi_jet_met = 0.4;
const float dphi_et_pt_cut = 1.6;
int first_bin = 1;
int last_bin = 50;
bool do_ratio = true;


template<typename T>
class HistMgr{
public:
    HistMgr()
    {
        f_znunu = TFile::Open(Form("%sZnunu_combined.root",base_dir.c_str()), "READ");
        f_wlnu  = TFile::Open(Form("%sW_combined.root",base_dir.c_str()), "READ");
        f_zll  = TFile::Open(Form("%sZ_combined.root",base_dir.c_str()), "READ");
        f_top  = TFile::Open(Form("%stop_combined.root",base_dir.c_str()), "READ");
        f_vv = TFile::Open(Form("%sDiboson_combined.root",base_dir.c_str()), "READ");
        f_dijets = TFile::Open(Form("%sDijets_combined.root",base_dir.c_str()), "READ");
        // f_dijets = TFile::Open(Form("%sDiboson_combined.root",base_dir.c_str()), "READ");
        f_data = TFile::Open(Form("%sdata_combined.root",base_dir.c_str()), "READ");
        f_data2 = TFile::Open(Form("%sdata_combined.root",base_dir.c_str()), "READ");
        // f_data2 = TFile::Open(Form("%sdata_no_leading.root",base_dir.c_str()), "READ");
        //
        // f_signal = TFile::Open(Form("%ssignal_combined.root",base_dir.c_str()), "READ");
        f_signal = NULL;
        
    }
    
    void load_hist(const char* hist_name)
    {
        h_znunu = (T*) f_znunu->Get(hist_name);
        h_wlnu = (T*) f_wlnu->Get(hist_name);
        h_zll = (T*) f_zll->Get(hist_name);
        h_top = (T*) f_top->Get(hist_name);
        h_vv = (T*) f_vv->Get(hist_name);
        h_dijets = (T*) f_dijets->Get(hist_name);
        h_data = (T*) f_data->Get(hist_name);
        h_data2 = (T*) f_data2->Get(hist_name);
        if(f_signal) h_signal = (T*) f_signal->Get(hist_name);
        cout <<"got histograms"<<endl;

        // weight
        Option_t* option = "";
        float add_weight = 1.0;
        if(TString(hist_name).Contains("met") || 
           TString(hist_name).Contains("ljetPt")) option = "width";
        h_znunu->Scale(weight*add_weight, option);
        h_wlnu->Scale(weight*add_weight, option);
        h_zll->Scale(weight*add_weight, option);
        h_top->Scale(weight*add_weight, option);
        h_vv->Scale(weight*add_weight, option);
        // h_dijets->Scale(weight*7.848206e+00);
        // h_dijets->Scale(weight*add_weight*2.087575e-04*1.073483e+00*6.313839e-01, option);
        h_dijets->Scale(weight*add_weight, option);
        if(h_signal) h_signal->Scale(weight, option);
        h_data->Sumw2();
        h_data->Scale(1., option);

        h_bkg_no_dijets = (T*) h_znunu->Clone("h_bkg_no_dijets");
        h_bkg_no_dijets->Add(h_wlnu);
        h_bkg_no_dijets->Add(h_zll);
        h_bkg_no_dijets->Add(h_vv);
        h_bkg_no_dijets->Add(h_top);

        h_bkg= (T*) h_bkg_no_dijets->Clone("h_bkg");
        h_bkg->Add(h_dijets);
        cout << " histograms loaded" << endl;
    }
    
    void do_plot(const char* hist_name)
    {
        load_hist(hist_name);

        string var_name;
        string met_cut;
        string region_name;
        tokenize_histname(hist_name, var_name, met_cut, region_name);

        TCanvas* canvas = new TCanvas("canvas", "canvas", 600, 600);
        SetAtlasStyleCanvas(canvas);
        // decoration
        h_znunu->SetLineColor(206);
        h_wlnu ->SetLineColor(64);
        h_zll->SetLineColor(95);
        h_vv ->SetLineColor(28);
        h_dijets ->SetLineColor(29);
        h_top->SetLineColor(209);
        if(h_signal){ 
            h_signal->SetLineColor(2);
            h_signal->SetFillColor(2);
        }

        //****
        h_znunu->SetFillColor(206);
        h_wlnu ->SetFillColor(64);
        h_zll->SetFillColor(95);
        h_vv ->SetFillColor(28);
        h_dijets ->SetFillColor(29);
        h_top->SetFillColor(209);
        //****/
        h_data->SetMarkerStyle(20);
        h_data->SetMarkerSize(1.2);

        h_data2->SetMarkerStyle(24);
        h_data2->SetMarkerSize(1.2);

        h_znunu->SetLineWidth(2);
        h_wlnu->SetLineWidth(2);
        h_zll->SetLineWidth(2);
        h_vv->SetLineWidth(2);
        h_dijets ->SetLineWidth(2);
        h_top->SetLineWidth(2);
        h_data->SetLineWidth(2);

        h_data2->SetLineWidth(2);

        // print the information
        printf("data: %.f\n", h_data->Integral());
        if(h_signal) printf("signal: %.2f\n", h_signal->Integral());
        printf("Bkg: %.2f\n", h_bkg->Integral());
        printf("Znunu: %.2f\n", h_znunu->Integral());
        printf("Wjets: %.2f\n", h_wlnu->Integral());
        printf("Zjets: %.2f\n", h_zll->Integral());
        printf("VV: %.2f\n", h_vv->Integral());
        printf("Dijets: %.2f\n", h_dijets->Integral());
        printf("Top: %.2f\n", h_top->Integral());

        if(var_name.compare("dphiEP") == 0){
            int low_bin = 1;
            int hi_bin = h_data->FindBin(dphi_et_pt_cut);
            // int hi_bin = h_data->GetNbinsX();
            printf("------------------------------\n");
            printf("Low: %d, Hi: %d\n", low_bin, hi_bin);
            printf("Efficiency (dphi_met_mpt):\nn");
            printf("data: %.2f\n", h_data->Integral(low_bin, hi_bin)/h_data->Integral());
            if(h_signal) printf("signal: %.2f\n", h_signal->Integral(low_bin, hi_bin)/h_signal->Integral());
            printf("Znunu: %.2f\n", h_znunu->Integral(low_bin, hi_bin)/h_znunu->Integral());
            printf("Wjets: %.2f\n", h_wlnu->Integral(low_bin, hi_bin)/h_wlnu->Integral());
            printf("Zjets: %.2f\n", h_zll->Integral(low_bin, hi_bin)/h_zll->Integral());
            printf("VV: %.2f\n", h_vv->Integral(low_bin, hi_bin)/h_vv->Integral());
            printf("Dijets: %.2f\n", h_dijets->Integral(low_bin, hi_bin)/h_dijets->Integral());
            printf("Top: %.2f\n", h_top->Integral(low_bin, hi_bin)/h_top->Integral());
            printf("------------------------------\n");
        }

        printf("--------------------\n");
        printf("data: %.f\n", h_data->GetEntries());
        if(h_signal) printf("signal: %.2f\n", h_signal->GetEntries());
        printf("Znunu: %.2f\n", h_znunu->GetEntries());
        printf("Wjets: %.2f\n", h_wlnu->GetEntries());
        printf("Zjets: %.2f\n", h_zll->GetEntries());
        printf("VV: %.2f\n", h_vv->GetEntries());
        printf("Dijets: %.2f\n", h_dijets->GetEntries());
        printf("Top: %.2f\n", h_top->GetEntries());
        printf("--------------------\n");
        /****
          norm_hist(h_vv);
          norm_hist(h_zll);
          norm_hist(h_top);
          norm_hist(h_wlnu);
          norm_hist(h_znunu);
          norm_hist(h_dijets);
         ****/
        TList* objarray = new TList();
        add_hist(objarray, h_vv);
        add_hist(objarray, h_zll);
        add_hist(objarray, h_top);
        add_hist(objarray, h_wlnu);
        add_hist(objarray, h_znunu);
        add_hist(objarray, h_dijets);

        // add_hist(objarray, h_signal);
        
        THStack* hs = new THStack("hs","");
        for(int i=0; i < objarray->GetEntries(); i++){
            TH1F* h1 = (TH1F*)objarray->At(i);
            hs->Add(h1);
        }

        bool use_log = true;
        if(use_log) canvas->SetLogy();
        double y_max = hs->GetMaximum()>h_data->GetMaximum()?hs->GetMaximum():h_data->GetMaximum();
        double y_min = hs->GetMinimum()<h_data->GetMinimum()?hs->GetMinimum():h_data->GetMinimum();
        if(!use_log) {
            // y_max = hs->GetMaximum();
            h_data->GetYaxis()->SetRangeUser(1e-3, y_max*1.1);
        }else{
            h_data->GetYaxis()->SetRangeUser(4e-3, y_max*1e4);
        }
        string xtitle = "E_{T}^{miss} [GeV]";
        if(var_name.compare("dphiEP") == 0){
            xtitle = "#Delta#phi(E_{T}^{miss}, #vec{p}_{T}^{miss}) [rad]";
        } else if (var_name.compare("dphi") == 0) {
            xtitle = "#Delta#phi(jets, E_{T}^{miss}) [rad]";
        } else if (var_name.compare("ljetPt") == 0) {
            xtitle = "leading jet p_{T} [GeV]";
        } else if (var_name.compare("metCST") == 0) {
            xtitle = "E_{T}^{miss} [GeV]";
        } else if (var_name.compare("njets") == 0){
            xtitle = "N_{jets}";
        } else if (var_name.compare("ratioMETpt") == 0) {
            xtitle = "E_{T}^{miss}/leading jet p_{T}";
        }
        h_data->SetNdivisions(8, "X");
        h_data->SetXTitle(xtitle.c_str());
        h_data->SetYTitle("Events");

        if(do_ratio){
            h_data->GetXaxis()->SetLabelSize(0);
            TPad* pad1 = add_ratio_pad(h_data, h_bkg);
            if(use_log) pad1->SetLogy();
        }

        h_data->Draw("EP");
        hs->Draw("HISTsame");
        h_data->Draw("AXISSAME");
        h_data->Draw("EPsame");

        // h_data2->Draw("EPsame");

        float x_off = 0.68, y_off = 0.85, delta_y = 0.04;
        legend = myLegend(x_off, y_off-delta_y*7, x_off+0.2, y_off);
        legend->AddEntry(h_data, "Data", "lp");
        legend->AddEntry(h_znunu, "Z #rightarrow #nu#nu", "fl");
        legend->AddEntry(h_wlnu, "W #rightarrow #it{l}#nu", "fl");
        legend->AddEntry(h_zll, "Z #rightarrow #it{l}#it{l}", "fl");
        legend->AddEntry(h_vv, "Diboson", "fl");
        legend->AddEntry(h_top, "t#bar{t} + single top", "fl");
        legend->AddEntry(h_dijets, "Multi-jet", "fl");
        legend->Draw();
        float x_off_title = 0.22;
        // myText(x_off_title, 0.85, 1, "#bf{#it{ATLAS}} Internal");
        myText(x_off_title, 0.85, 1, "#bf{#it{ATLAS}} Work In Progress");
        myText(x_off_title-0.04, 0.80, 1, Form("#sqrt{s} = 13 TeV: #scale[0.55]{#int}Ldt = %.1f fb^{-1}", weight/1e3));
        // myText(x_off_title, 0.75, 1, Form("%s, E_{T}^{miss}> %s GeV", region_name.c_str(), met_cut.c_str()));
        myText(x_off_title, 0.75, 1, Form("QCD, E_{T}^{miss}> %s GeV", met_cut.c_str()));

        TString outeps_name(Form("eps/%s", hist_name));
        if(use_log) outeps_name += "_log";
        if(do_ratio) outeps_name += "_ratio";
        canvas->SaveAs(Form("%s.eps", outeps_name.Data()));

        // do ROC
        /*
        int cutbin = get_roc(h_signal, h_bkg);
        cout << "After cut of " << h_signal->GetBinLowEdge(cutbin) << endl;
        print_after_cut("signal", h_signal, cutbin);
        */
    }

    void qcd(const char* hist_name){
        load_hist(hist_name);

        string var_name;
        string met_cut;
        string region_name;
        tokenize_histname(hist_name, var_name, met_cut, region_name);
        /* 
         * obtain W and Z scaling factor from CR
         */ 
        const char* var_for_sf = "njets";
        float z_data = 0, z_mc = 0;
        float w_data = 0, w_mc = 0;
        float z_tmp_data, z_tmp_mc;
        float w_tmp_data, w_tmp_mc;
        float sf_ZEE = get_sf(Form("%s_ZEE_%s", var_for_sf, met_cut.c_str()), 
                z_tmp_data, z_tmp_mc);
        // z_data += z_tmp_data; 
        // z_mc += z_tmp_mc;
        float sf_Zmm = get_sf(Form("%s_ZMM_%s", var_for_sf, met_cut.c_str()),
                z_tmp_data, z_tmp_mc);
        z_data += z_tmp_data; 
        z_mc += z_tmp_mc;
        float sf_Wenu = get_sf(Form("%s_WENU_%s", var_for_sf, met_cut.c_str()),
                w_tmp_data, w_tmp_mc);
        w_data += w_tmp_data;
        w_mc += w_tmp_mc;
        float sf_Wmunu = get_sf(Form("%s_WMUNU_%s", var_for_sf, met_cut.c_str()),
                w_tmp_data, w_tmp_mc);
        w_data += w_tmp_data;
        w_mc += w_tmp_mc;

        float w_scale, w_error;
        float z_scale, z_error;
        get_ratio_and_error(w_data, w_mc, w_scale, w_error);
        get_ratio_and_error(z_data, z_mc, z_scale, z_error);

        printf("W scale: %.3f +/- %.3f\n", w_scale, w_error);
        printf("Z scale: %.3f +/- %.3f\n", z_scale, z_error);
        /* 
         * total background (no-dijets), 
         * weighted by scaling factor in CR 
         */
        T* h_mc_corrected = (T*) h_znunu->Clone("h_mc_corrected");
        h_mc_corrected->Add(h_wlnu, w_scale);
        h_mc_corrected->Add(h_zll, z_scale);
        h_mc_corrected->Add(h_vv);
        h_mc_corrected->Add(h_top);

        #ifdef _ADD_SIGNAL_
        h_mc_corrected->Add(h_signal);
        h_bkg_no_dijets->Add(h_signal);
        #endif 

        TCanvas* canvas = new TCanvas("canvas", "canvas", 600, 600);
        SetAtlasStyleCanvas(canvas);

        h_data->GetXaxis()->SetTitle("#Delta#phi(jets, E_{T}^{miss}) [rad]");
        h_data->GetYaxis()->SetTitle("#Delta#phi(E_{T}^{miss}, #vec{p}_{T}^{miss}) [rad]");
        h_data->SetMarkerSize(0.3);
        h_data->GetYaxis()->SetRangeUser(0, 8);
        h_data->Draw();
        float x_off = 0.70, y_off = 0.85, delta_y = 0.04;
        // myLineText(x_off, y_off, *h_data, "Data", delta_y);
        float x_off_title = 0.5;
        myText(x_off_title, 0.85, 1, "#bf{#it{ATLAS}} Internal");
        myText(x_off_title-0.04, 0.80, 1, Form("#sqrt{s} = 13 TeV: #scale[0.55]{#int}Ldt = %.1f pb^{-1}", weight));
        myText(x_off_title, 0.75, 1, Form("%s, E_{T}^{miss}> %s GeV", region_name.c_str(), met_cut.c_str()));
        TString outeps_name(Form("eps/%s", hist_name));
        canvas->SaveAs(Form("%s.eps", outeps_name.Data()));
       
        calculate_qcd(h_data, h_bkg_no_dijets);
        calculate_qcd(h_data, h_mc_corrected);
    }
    
    bool tokenize_histname(const char* hist_name, 
            string& var_name, string& met_cut, string& region_name) 
    {
        istringstream iss(hist_name);
        vector<string> tokens;
        string token;
        while (getline(iss, token, '_')){
            tokens.push_back(token);
        }
        size_t ntokens = tokens.size();
        var_name = tokens.at(0);
        met_cut = tokens.at(ntokens-1);
        region_name = tokens.at(ntokens-2);

        float met_cut_value = atof(met_cut.c_str());
        if(met_cut_value >= 10 && met_cut_value <= 50) {
            met_cut = "250";
        }
        return true;
    }

    float get_sf(const char* hist_name, float& exp_data, float& exp_mc)
    {
        TH1F* h_znunu_cr = (TH1F*) f_znunu->Get(hist_name);
        TH1F* h_wlnu_cr = (TH1F*) f_wlnu->Get(hist_name);
        TH1F* h_zll_cr = (TH1F*) f_zll->Get(hist_name);
        TH1F* h_top_cr = (TH1F*) f_top->Get(hist_name);
        TH1F* h_vv_cr = (TH1F*) f_vv->Get(hist_name);
        TH1F* h_dijets_cr = (TH1F*) f_dijets->Get(hist_name);

        h_znunu_cr->Scale(weight);
        h_wlnu_cr->Scale(weight);
        h_zll_cr->Scale(weight);
        h_top_cr->Scale(weight);
        h_vv_cr->Scale(weight);
        h_dijets_cr->Scale(weight);

        TH1F* h_data_cr = (TH1F*) f_data->Get(hist_name);

        TH1F* h_mc = (TH1F*) h_znunu_cr->Clone("h_mc");
        h_mc->Add(h_wlnu_cr);
        h_mc->Add(h_zll_cr);
        h_mc->Add(h_top_cr);
        h_mc->Add(h_vv_cr);
        h_mc->Add(h_dijets_cr);
        
        float ratio =  h_data_cr->Integral()/h_mc->Integral();
        exp_data = h_data_cr->Integral();
        exp_mc = h_mc->Integral();
/***
        printf("HistName: %s\n", hist_name);
        printf("data: %.f\n", exp_data);
        printf("bkg: %.3f\n", exp_mc);
        printf("ratio: %.3f\n", ratio);
        printf("Znunu: %.3f\n", h_znunu_cr->Integral());
        printf("Wjets: %.3f\n", h_wlnu_cr->Integral());
        printf("Zjets: %.3f\n", h_zll_cr->Integral());
        printf("VV: %.3f\n", h_vv_cr->Integral());
        printf("Dijets: %.3f\n", h_dijets_cr->Integral());
        printf("Top: %.3f\n", h_top_cr->Integral());
 ***/     
        delete h_znunu_cr;
        delete h_wlnu_cr;
        delete h_zll_cr;
        delete h_top_cr;
        delete h_vv_cr;
        delete h_dijets_cr;
        delete h_mc;

        return  ratio;
    }

    float calculate_qcd(T* h_data, T* h_bkg)
    {
        int bin_dphi_jets_met = h_data->GetXaxis()->FindBin(min_dphi_jet_met);
        int bin_dphi_met_mpt = h_data->GetYaxis()->FindBin(dphi_et_pt_cut);

        float regionA_data = h_data->Integral(first_bin, bin_dphi_jets_met, 
                first_bin, bin_dphi_met_mpt);
        float regionA_mc = h_bkg->Integral(first_bin, 
                bin_dphi_jets_met, first_bin, bin_dphi_met_mpt);


        float regionB_data = h_data->Integral(first_bin, bin_dphi_jets_met, 
                bin_dphi_met_mpt+1, last_bin);
        float regionB_mc = h_bkg->Integral(first_bin, 
                bin_dphi_jets_met, bin_dphi_met_mpt+1, last_bin);

        float regionC_data = h_data->Integral(bin_dphi_jets_met+1, last_bin,
                bin_dphi_met_mpt+1, last_bin);
        float regionC_mc = h_bkg->Integral(bin_dphi_jets_met+1, last_bin,
                bin_dphi_met_mpt+1, last_bin);

        float regionD_data = h_data->Integral(bin_dphi_jets_met+1, last_bin,
                first_bin, bin_dphi_met_mpt);
        float regionD_mc = h_bkg->Integral(bin_dphi_jets_met+1, last_bin,
                first_bin, bin_dphi_met_mpt);
/****
        float regionA_dijets = h_dijets->Integral(first_bin, 
                bin_dphi_jets_met, first_bin, bin_dphi_met_mpt);
        float regionB_dijets = h_dijets->Integral(first_bin, 
                bin_dphi_jets_met, bin_dphi_met_mpt+1, last_bin);
        float regionC_dijets = h_dijets->Integral(bin_dphi_jets_met+1, last_bin,
                bin_dphi_met_mpt+1, last_bin);
        float regionD_dijets = h_dijets->Integral(bin_dphi_jets_met+1, last_bin,
                first_bin, bin_dphi_met_mpt);
        printf("------------------------------\n");
        printf("Dijets in A, B, C, D\n");
        printf("%.2f, %.2f, %.2f, %.2f\n", regionA_dijets, regionB_dijets, regionC_dijets, regionD_dijets);
***/

        float deltaA = regionA_data - regionA_mc;
        float deltaB = regionB_data - regionB_mc;
        float deltaC = regionC_data - regionC_mc;
        float deltaD = regionD_data - regionD_mc;
        printf("------------------------------\n");
        printf("A, %.f, %.2f, %.2f\n", regionA_data, regionA_mc, deltaA);
        printf("B, %.f, %.2f, %.2f\n", regionB_data, regionB_mc, deltaB);
        printf("C, %.f, %.2f, %.2f\n", regionC_data, regionC_mc, deltaC);
        printf("D, %.f, %.2f, %.2f\n", regionD_data, regionD_mc, deltaD);
        printf("------------------------------\n");

        float regionD_dd = regionA_data/regionB_data * regionC_data;
        float regionD_dd_sub = deltaA / deltaB * deltaC;
        printf("SR: data_abcd_subtract\n");
        // printf("SR: %.2f, %.2f\n", regionD_dd, regionD_dd_sub);
        printf("SR: %.2f\n", regionD_dd_sub);
        printf("------------------------------\n");
        return regionD_dd_sub;
    }

    void correlation_study(T* hist, float dphi_cut, float dphi_ep_cut)
    {
        int bin_dphi_jets_met_low = hist->GetXaxis()->FindBin(0.2);
        int bin_dphi_met_mpt_low = hist->GetXaxis()->FindBin(2.0);
        int bin_dphi_jets_met = hist->GetXaxis()->FindBin(dphi_cut);
        int bin_dphi_met_mpt = hist->GetYaxis()->FindBin(dphi_ep_cut);

        float regionA = hist->Integral(first_bin, bin_dphi_jets_met_low, 
                first_bin, bin_dphi_met_mpt);
        float regionB = hist->Integral(first_bin, bin_dphi_jets_met_low, 
                bin_dphi_met_mpt_low+1, last_bin);
        float regionC = hist->Integral(bin_dphi_jets_met+1, last_bin,
                bin_dphi_met_mpt_low+1, last_bin);
        double regionD_error;
        float regionD = hist->IntegralAndError(bin_dphi_jets_met+1, last_bin,
                first_bin, bin_dphi_met_mpt, regionD_error);
        /***
        printf("A(%.2f), B(%.2f), C(%.2f)\n", regionA, regionB, regionC);
        printf("yields in region D\n");
        printf("MC, %.2f +/- %.3f\n", regionD, regionD_error);
        printf("ABCD, %.2f\n", regionA/regionB*regionC);
        */
        printf("(A/B)/(D/C): %.2f\n", (regionA/regionB)/(regionD/regionC));
    }

    void correlation_study()
    {
        SetAtlasStyle();
        TH1F* dphi_A = (TH1F*) f_dijets->Get("dphi_SR_11"); 
        TH1F* dphi_B = (TH1F*) f_dijets->Get("dphi_SR_12"); 
        TH1F* dphi_C = (TH1F*) f_dijets->Get("dphi_SR_13"); 
        TH1F* dphi_D = (TH1F*) f_dijets->Get("dphi_SR_14"); 

        TH1F* dphi_up = (TH1F*) dphi_B->Clone("dphi_up");
        // dphi_up->Sumw2();
        dphi_up->Add(dphi_C);
        TH1F* dphi_down = (TH1F*) dphi_A->Clone("dphi_down");
        // dphi_down->Sumw2();
        dphi_down->Add(dphi_D);

        dphi_up->SetLineColor(2);
        dphi_down->SetLineColor(4);

        TCanvas* canvas = new TCanvas("canvas1", "canvas", 600, 600);
        dphi_down->Scale(dphi_up->Integral()/dphi_down->Integral());
        double max_y = dphi_down->GetMaximum()>dphi_up->GetMaximum()?
            dphi_down->GetMaximum():dphi_up->GetMaximum();
        dphi_up->GetYaxis()->SetRangeUser(0, max_y*1.1);
        dphi_up->SetXTitle("#Delta#phi(jets, E_{T}^{miss})");
        // add_ratio_pad(dphi_up, dphi_down);

        dphi_up->Draw();
        dphi_down->Draw("same");
        legend = myLegend(0.5, 0.7, 0.7, 0.85);
        legend->AddEntry(dphi_up,"#Delta#phi(E_{T}^{miss}, #vec{p}_{T}^{miss}) > 2.0", "l");
        legend->AddEntry(dphi_down,"#Delta#phi(E_{T}^{miss}, #vec{p}_{T}^{miss}) < 2.0", "l");
        legend->Draw();
        canvas->SaveAs("eps/dphi_dphiEP_vary.eps");
    }
    void correlation_study2()
    {
        SetAtlasStyle();
        TH1F* dphi_A = (TH1F*) f_dijets->Get("dphiEP_SR_11"); 
        TH1F* dphi_B = (TH1F*) f_dijets->Get("dphiEP_SR_12"); 
        TH1F* dphi_C = (TH1F*) f_dijets->Get("dphiEP_SR_13"); 
        TH1F* dphi_D = (TH1F*) f_dijets->Get("dphiEP_SR_14"); 

        TH1F* dphi_up = (TH1F*) dphi_B->Clone("dphi_up");
        // dphi_up->Sumw2();
        dphi_up->Add(dphi_A);
        TH1F* dphi_down = (TH1F*) dphi_C->Clone("dphi_down");
        // dphi_down->Sumw2();
        dphi_down->Add(dphi_D);

        dphi_up->SetLineColor(2);
        dphi_down->SetLineColor(4);

        TCanvas* canvas = new TCanvas("canvas2", "canvas", 600, 600);
        dphi_down->Scale(dphi_up->Integral()/dphi_down->Integral());
        double max_y = dphi_down->GetMaximum()>dphi_up->GetMaximum()?
            dphi_down->GetMaximum():dphi_up->GetMaximum();
        dphi_up->GetYaxis()->SetRangeUser(0, max_y*1.1);
        dphi_up->SetXTitle("#Delta#phi(E_{T}^{miss}, #vec{p}_{T}^{miss})");
        // add_ratio_pad(dphi_up, dphi_down);

        dphi_up->Draw();
        dphi_down->Draw("same");
        legend = myLegend(0.5, 0.7, 0.7, 0.85);
        legend->AddEntry(dphi_up,"#Delta#phi(jets, E_{T}^{miss}) < 0.4", "l");
        legend->AddEntry(dphi_down,"#Delta#phi(jets, E_{T}^{miss}) > 0.4", "l");
        legend->Draw();
        canvas->SaveAs("eps/dphiEP_dphi_vary.eps");
    }
    void correlation_study3(){
        SetAtlasStyle();
        h2d = (TH2F*) f_dijets->Get("dphi2D_SR_250"); 
        int bin1 = h2d->GetXaxis()->FindBin(0.4);
        int bin2 = h2d->GetXaxis()->FindBin(0.8);
        int bin3 = h2d->GetXaxis()->FindBin(1.2);
        int bin4 = h2d->GetXaxis()->FindBin(2.0);
        int nbins = h2d->GetNbinsX();
        h1 = h2d->ProjectionX("h1", 1, bin1);
        h2 = h2d->ProjectionX("h2", bin1+1, bin2);
        h3 = h2d->ProjectionX("h3", bin2+1, bin3);
        h4 = h2d->ProjectionX("h4", bin3+1, bin4);
        h5 = h2d->ProjectionX("h5", bin4+1, nbins);
        h2->Scale(h1->Integral()/h2->Integral());
        h3->Scale(h1->Integral()/h3->Integral());
        h4->Scale(h1->Integral()/h4->Integral());
        int rebin = 22;
        h1->Rebin(rebin);
        h2->Rebin(rebin);
        h3->Rebin(rebin);
        h4->Rebin(rebin);
        h5->Rebin(rebin);
        h1->GetYaxis()->SetRangeUser(0, 5.5); 
        h1->SetLineColor(206);
        h2->SetLineColor(64);
        h3->SetLineColor(95);
        h4->SetLineColor(28);
        h5->SetLineColor(29);
        
        TCanvas* canvas = new TCanvas("canvas3", "canvas", 600, 600);
        h1->SetXTitle("#Delta#phi(E_{T}^{miss}, jets)");
        h1->GetYaxis()->SetRangeUser(0, 0.1);
        h1->Draw();
        h2->Draw("same");
        h3->Draw("same");
        h4->Draw("same");
        h5->Draw("same");
        hnull = (TH1F*) h1->Clone("hnull");
        hnull->SetLineColor(0);
        legend = myLegend(.50, 0.55, 0.70, 0.85);
        legend->AddEntry(hnull,"#Delta#phi(E_{T}^{miss},#vec{p}_{T}^{miss}) slice", "l");
        legend->AddEntry(h1,"[0, 0.4]", "l");
        legend->AddEntry(h2,"[0.4, 0.8]", "l");
        legend->AddEntry(h3,"[0.8, 1.2]", "l");
        legend->AddEntry(h4,"[1.2, 2.0]", "l");
        legend->AddEntry(h5,"[2.0, 3.2]", "l");
        legend->Draw();
        canvas->SaveAs("eps/dphiEP_dphi_vary2.eps");
    }


public:
    T* h_znunu;
    T* h_wlnu;
    T* h_zll;
    T* h_top;
    T* h_vv;
    T* h_dijets;
    T* h_data;
    T* h_data2;
    T* h_bkg_no_dijets;
    T* h_bkg;
    T* h_signal = NULL;
    TFile* f_znunu;
    TFile* f_wlnu;
    TFile* f_zll;
    TFile* f_top;
    TFile* f_vv;
    TFile* f_dijets;
    TFile* f_data;
    TFile* f_data2;
    TFile* f_signal = NULL;

};

void run(int option, const char* hist_name)
{
    SetAtlasStyle();
    if(option == 1){
        HistMgr<TH1F>* hist_mgr_1d = new HistMgr<TH1F>();
        hist_mgr_1d->do_plot(hist_name);
    } else {
        HistMgr<TH2F>* hist_mgr_2d = new HistMgr<TH2F>();
        hist_mgr_2d->qcd(hist_name);
    }
}

void go_plot(int cut_tag, const char* region)
{
    SetAtlasStyle();
    HistMgr<TH1F>* hist_mgr_1d = new HistMgr<TH1F>();
    
    hist_mgr_1d->do_plot(Form("met_%s_%d", region, cut_tag));
    hist_mgr_1d->do_plot(Form("ljetPt_%s_%d", region, cut_tag));
    hist_mgr_1d->do_plot(Form("dphi_%s_%d", region, cut_tag));
    hist_mgr_1d->do_plot(Form("njets_%s_%d", region, cut_tag));
    // hist_mgr_1d->do_plot(Form("dphiEP_%s_%d", region, cut_tag));
    // hist_mgr_1d->do_plot(Form("met_SR_%d", cut_tag));
    // hist_mgr_1d->do_plot(Form("met_ZEE_%d", cut_tag));
    // hist_mgr_1d->do_plot(Form("met_ZMM_%d", cut_tag));
    // hist_mgr_1d->do_plot(Form("met_WENU_%d", cut_tag));
    // hist_mgr_1d->do_plot(Form("met_WMUNU_%d", cut_tag));
}

void correlate(const char* hist_name){
    HistMgr<TH2F>* hist_mgr = new HistMgr<TH2F>();
    float dphi_cut = min_dphi_jet_met;
    float dphi_ep_cut = dphi_et_pt_cut;
    hist_mgr->load_hist(hist_name);
    hist_mgr->correlation_study(hist_mgr->h_dijets, dphi_cut, dphi_ep_cut);
    hist_mgr->correlation_study(hist_mgr->h_signal, dphi_cut, dphi_ep_cut);
    hist_mgr->correlation_study(hist_mgr->h_znunu, dphi_cut, dphi_ep_cut);
}

void correlate(){
    HistMgr<TH2F>* hist_mgr = new HistMgr<TH2F>();
    hist_mgr->correlation_study();
    hist_mgr->correlation_study2();
    hist_mgr->correlation_study3();
}

void get_qcd_met(){
    HistMgr<TH1F>* hist_A = new HistMgr<TH1F>();
    hist_A->load_hist("met_SR_11");
    HistMgr<TH1F>* hist_B = new HistMgr<TH1F>();
    hist_B->load_hist("met_SR_12");
    HistMgr<TH1F>* hist_C = new HistMgr<TH1F>();
    hist_C->load_hist("met_SR_13");

    hist_A->h_data->Add(hist_A->h_bkg_no_dijets, -1);
    hist_B->h_data->Add(hist_B->h_bkg_no_dijets, -1);
    hist_C->h_data->Add(hist_C->h_bkg_no_dijets, -1);
    TH1F* h_met = (TH1F*) hist_A->h_data->Clone("h_met");
    h_met->Divide(hist_B->h_data);
    h_met->Multiply(hist_C->h_data);
    SetAtlasStyle();

    TCanvas* canvas = new TCanvas("canvas", "canvas", 600, 600);
    h_met->UseCurrentStyle();
    h_met->SetMarkerSize(0);
    h_met->GetYaxis()->SetRangeUser(1e-3, 1e4);
    h_met->SetLineColor(2);

    TF1* fa = new TF1("fa", "[0]*exp([1]*x)", 100, 1000);
    fa->SetParameter(0, 1.32351e-03);
    fa->SetParameter(1, -7.64122e-02);
    TFitResultPtr fit_res = h_met->Fit("fa", "SR");
    h_met->SetXTitle("E_{T}^{miss} [GeV]");
    h_met->Draw("");
    canvas->SetLogy();
    canvas->SaveAs("eps/met_qcd.eps");
   
    int bin_100 = h_met->FindBin(100);
    int bin_250 = h_met->FindBin(250);
    int total_bins = h_met->GetNbinsX();
    double ratio_hist = h_met->Integral(bin_250, total_bins)/h_met->Integral(bin_100, total_bins);
    double ratio_tf = fa->Integral(250, 1000)/fa->Integral(100, 1000);
    cout << "Ratio: " << ratio_hist << " " << ratio_tf <<  endl;
    // TH1F* h1 = generate_th(h_met, "h1", 101);
    // h1->SetLineColor(4);
    // h1->Draw("sameEP");
}
