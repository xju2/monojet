#include <TH1F.h>
#include <TString.h>

#include "/afs/cern.ch/user/x/xju/tool/AtlasUtils.C"
#include "/afs/cern.ch/user/x/xju/tool/AtlasStyle.C"
#include "/afs/cern.ch/user/x/xju/tool/loader.c"

void plot_met(){
    SetAtlasStyle();

    TCanvas* canvas = new TCanvas("canvas", "canvas", 600, 600);
    // const char* input_name = "$GROUPEOSDIR/monojet/minitrees/mc15_13TeV_v1/test_met_gamma_jet_all.root";
    // const char* input_name = "$GROUPEOSDIR/monojet/minitrees/mc15_13TeV_v1/test_met_Wtaunu_361357_all.root";
    // const char* input_name = "/afs/cern.ch/user/x/xju/work/monoJet/process/get_minitree/testarea/reduced_ntup_withmuon_subtracted.root";
    // TChain* physics_nomuon = new TChain("physics", "physics");
    // physics_nomuon->Add(input_name);
    //
    string base_dir = "/afs/cern.ch/user/x/xju/work/monoJet/process/do_minitree/eps_2015/";
    const char* input_name = Form("%sWmunu.list",base_dir.c_str());
    TCut base_cut("flag_el_veto && flag_mu_veto && n_good_jet <= 4");
    double met_xbins[] = {50, 100, 125, 150, 175, 200, 225, 250, 275, 300, 350, 400, 450, 500, 600, 800, 1000};
    int met_nbins = 16;
    TH1F* h1_nophtau = new TH1F("h1_nophtau","h1_nophtau", met_nbins, met_xbins);
    
    TChain* physics_nomuon = loader(input_name, "physics");
    physics_nomuon->Draw("sqrt(MET_noph_tau_etx**2+MET_noph_tau_ety**2)/1e3>>h1_nophtau", base_cut);

    TH1F* h1_nomuon = (TH1F*) h1_nophtau->Clone("h1_nomuon");
    physics_nomuon->Draw("sqrt(MET_nomuon_etx**2 + MET_nomuon_ety**2)/1e3>>h1_nomuon", base_cut);
    //
    TH1F* h1_withmuon = (TH1F*) h1_nophtau->Clone("h1_withmuon");
    physics_nomuon->Draw("MET_et/1e3>>h1_withmuon", base_cut);

    TH1F* h1_cst = (TH1F*) h1_nophtau->Clone("h1_cst");
    physics_nomuon->Draw("sqrt(MET_cst_etx**2+MET_cst_ety**2)/1e3>>h1_cst", base_cut);



    h1_nophtau->SetLineColor(2);
    h1_nomuon->SetLineColor(4);
    h1_withmuon->SetLineColor(8);
    h1_cst->SetLineColor(95);

    h1_nophtau->SetXTitle("MET [GeV]");
    double max_y = h1_nophtau->GetMaximum() > h1_nomuon->GetMaximum()?h1_nophtau->GetMaximum():h1_nomuon->GetMaximum();
    if(h1_withmuon->GetMaximum() > max_y) max_y = h1_withmuon->GetMaximum();
    h1_nophtau->GetYaxis()->SetRangeUser(0., max_y*1.2);

    h1_nophtau->Draw();
    h1_nomuon->Draw("same");
    h1_withmuon->Draw("same");
    h1_cst->Draw("same");

    myLineText(0.25, 0.80, *h1_nophtau, "no ph tau");
    myLineText(0.25, 0.75, *h1_nomuon, "no muon");
    myLineText(0.25, 0.65, *h1_withmuon, "with muon");
    myLineText(0.25, 0.60, *h1_cst, "CST");

    canvas->SaveAs("met_SR_wmunu.eps");
    canvas->SaveAs("met_SR_wmunu.png");
}

void plot_met2(){
    SetAtlasStyle();

    TCanvas* canvas = new TCanvas("canvas", "canvas", 600, 600);
    const char* input_name = "/afs/cern.ch/user/x/xju/work/monoJet/process/get_minitree/testarea/systematic/wmunu/all.root";
    TH1F* h1_down = new TH1F("h1_down","h1_down", 60, 0, 600);
    // TChain* ch_down = new TChain("MET_SoftTrk_ScaleDown-down", "MET_SoftTrk_ScaleDown-down");
    TChain* ch_down = new TChain("MET_SoftTrk_ResoPara-down", "MET_SoftTrk_ResoPara-down");
    ch_down->Add(input_name);
    ch_down->Draw("sqrt((MET_etx-MET_muon_etx)**2 + (MET_ety-MET_muon_ety)**2)/1e3>>h1_down","");

    TH1F* h1_up = (TH1F*) h1_down->Clone("h1_up");
    // TChain* ch_up = new TChain("MET_SoftTrk_ScaleUp-up", "MET_SoftTrk_ScaleUp-up");
    TChain* ch_up = new TChain("MET_SoftTrk_ResoPerp-up", "MET_SoftTrk_ResoPerp-up");
    ch_up->Add(input_name);
    ch_up->Draw("sqrt((MET_etx-MET_muon_etx)**2 + (MET_ety-MET_muon_ety)**2)/1e3>>h1_up","");
    //physics_nomuon->Draw("sqrt(MET_nomuon_etx**2 + MET_nomuon_ety**2)/1e3>>h1_muon","");
    //

    h1_down->SetLineColor(2);
    h1_up->SetLineColor(4);
    h1_down->SetXTitle("MET [GeV]");
    double max_y = h1_up->GetMaximum() > h1_down->GetMaximum()?h1_up->GetMaximum():h1_down->GetMaximum();
    h1_down->GetYaxis()->SetRangeUser(1e-3, max_y*1.05);
    // canvas->SetLogy();
    h1_down->Draw();
    // h1_nomuon->Draw("same");
    h1_up->Draw("same");

    myLineText(0.25, 0.80, *h1_down, "resolution down");
    // myLineText(0.25, 0.75, *h1_nomuon, "no muon");
    myLineText(0.25, 0.70, *h1_up, "resolution up");

    canvas->SaveAs("met_soft_term_reso_sys.eps");
    canvas->SaveAs("met_soft_term_reso_sys.png");
}

void compare_met(){
    SetAtlasStyle();
    TCanvas* canvas = new TCanvas("canvas", "canvas", 600, 600);

    const char* base_name = "/afs/cern.ch/user/x/xju/work/monoJet/process/get_minitree/testarea/";
    const char* file_name1 = Form("%smini_wtaunu_susy1.root", base_name);
    const char* file_name2 = Form("%smini_wtaunu_exot5.root", base_name);
    TChain* ch1 = new TChain("physics", "physics");
    ch1->Add(file_name1);
    TChain* ch2 = new TChain("physics", "physics");
    ch2->Add(file_name2);
    //double met_xbins[] = {50, 100, 125, 150, 175, 200, 225, 250, 275, 300, 350, 400, 450, 500, 600, 800, 1000};
    //int met_nbins = 16;
    TH1F* MET_template = new TH1F("MET_template","MET_template", 100, 0, 600);
    TH1F* hist1 = (TH1F*)MET_template->Clone("hist1");
    TH1F* hist2 = (TH1F*)MET_template->Clone("hist2");
    
    TCut cut("jet_index_1st >=0 && jet_p4[jet_index_1st].Pt() > 150e3 && flag_el_veto && flag_mu_veto");
    // TCut cut("");
    ch1->Draw("MET_et/1e3>>hist1", cut);
    ch2->Draw("MET_et/1e3>>hist2", cut);
    
    hist1->SetXTitle("MET [GeV]");
    double max_y = hist1->GetMaximum() > hist2->GetMaximum()?hist1->GetMaximum():hist2->GetMaximum();
    hist1->GetYaxis()->SetRangeUser(0, max_y*1.1);
    
    hist1->SetLineColor(2);
    hist2->SetLineColor(4);

    // cout << hist1->Integral() << " " << hist2->Integral() << endl;
    hist1->Scale(1./hist1->Integral());
    hist2->Scale(1./hist2->Integral());

    hist1->Draw();
    hist2->Draw("same");
    myLineText(0.65, 0.80, *hist1, "SUSY1");
    myLineText(0.65, 0.75, *hist2, "Exot5");
    canvas->SaveAs("met_comp_daod_no_lep.eps");
    canvas->SaveAs("met_comp_daod_no_lep.png");
}

void compare_dphi_jetjet()
{
    SetAtlasStyle();
    TCanvas* canvas = new TCanvas("canvas", "canvas", 600, 600);

    const char* base_name = "/afs/cern.ch/user/x/xju/work/monoJet/process/get_minitree/testarea/";
    const char* file_name1 = Form("%smini_zvv_jetjet.root", base_name);
    const char* file_name2 = Form("%smini_wtaunu_jetjet.root", base_name);
    const char* file_name3 = Form("%smini_dmv_jetjet.root", base_name);
    TChain* ch1 = new TChain("physics", "physics");
    ch1->Add(file_name1);
    TChain* ch2 = new TChain("physics", "physics");
    ch2->Add(file_name2);
    TChain* ch3 = new TChain("physics", "physics");
    ch3->Add(file_name3);
    //double met_xbins[] = {50, 100, 125, 150, 175, 200, 225, 250, 275, 300, 350, 400, 450, 500, 600, 800, 1000};
    //int met_nbins = 16;
    TH1F* MET_template = new TH1F("MET_template","MET_template", 50, 0, 3.5);
    TH1F* hist1 = (TH1F*)MET_template->Clone("hist1");
    TH1F* hist2 = (TH1F*)MET_template->Clone("hist2");
    TH1F* hist3 = (TH1F*)MET_template->Clone("hist3");
    
    TCut cut("n_good_jet >= 2 && jet_p4[jet_index_1st].Pt() > 150e3 && flag_el_veto && flag_mu_veto && MET_et > 250e3 && min_dphi_jetMET > 0.4");
    ch1->Draw("abs(average_dphi_jetjet)>>hist1", cut);
    ch2->Draw("abs(average_dphi_jetjet)>>hist2", cut);
    ch3->Draw("abs(average_dphi_jetjet)>>hist3", cut);
    // ch1->Draw("min_dphi_jetMET>>hist1", cut);
    // ch2->Draw("min_dphi_jetMET>>hist2", cut);
    // ch3->Draw("min_dphi_jetMET>>hist3", cut);
    
    // hist1->SetXTitle("average #Delta#phi(jet,leading)");
    hist1->SetXTitle("#Delta#phi(jets, MET)");
    // cout << hist1->Integral() << " " << hist2->Integral() << endl;
    hist1->Scale(1./hist1->Integral());
    hist2->Scale(1./hist2->Integral());
    hist3->Scale(1./hist3->Integral());

    double max_y = hist1->GetMaximum() > hist2->GetMaximum()?hist1->GetMaximum():hist2->GetMaximum();
    hist1->GetYaxis()->SetRangeUser(0, max_y*1.1);
    
    hist1->SetLineColor(2);
    hist2->SetLineColor(4);
    hist3->SetLineColor(209);
    hist3->SetLineStyle(2);

    hist1->Draw();
    hist2->Draw("same");
    hist3->Draw("same");
    myLineText(0.65, 0.80, *hist1, "Z#rightarrow#nu#nu");
    myLineText(0.65, 0.75, *hist2, "W#rightarrow#tau#nu");
    myLineText(0.65, 0.70, *hist3, "DM Vector 50 300");
    canvas->SaveAs("dphi_jj_with_dphi_cut.eps");
    canvas->SaveAs("dphi_jj_with_dphi_cut.png");
}
