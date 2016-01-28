// System include(s):
#include <memory>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <typeinfo>
#include <math.h>

// ROOT include(s):
#include <TFile.h>
#include <TError.h>
#include <TString.h>
#include <TChain.h>
#include <TH1F.h>

// Infrastructure include(s):
#ifdef ROOTCORE
#   include "xAODRootAccess/Init.h"
#   include "xAODRootAccess/TEvent.h"
#endif // ROOTCORE

#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"

// EDM include(s):
#include "xAODEventInfo/EventInfo.h"

#include "xAODMuon/MuonContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/ElectronAuxContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"
#include "xAODMissingET/MissingETContainer.h"
#include "xAODMissingET/MissingETAuxContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackParticleContainer.h"

#include "xAODCore/ShallowCopy.h"

// Local include(s):
#include "CPAnalysisExamples/errorcheck.h"
#include "SUSYTools/SUSYObjDef_xAOD.h"

#include "JetSmearing/JetMCSmearingTool.h"
#include "JetSmearing/SmearData.h"

// Other includes
// #include "PATInterfaces/SystematicList.h"
#include "PATInterfaces/SystematicVariation.h"
#include "PATInterfaces/SystematicRegistry.h"
#include "PATInterfaces/SystematicCode.h"
#include "boost/unordered_map.hpp"


#include "MonoJet/MyCuts.h"
#include "MonoJet/OutputTreeMan.h"
#include "MonoJet/Helper.h"
#include "MonoJet/SmearedInfo.h"

#include "MyXAODTools/CPToolsHelper.h"
#include "MyXAODTools/TrackBranch.h"
#include "MyXAODTools/PhotonBranch.h"
#include "MyXAODTools/JetBranch.h"

#include <TError.h>
using namespace std;

static SG::AuxElement::Decorator<double> dec_dphi_MET("dphi_MET");
static SG::AuxElement::Decorator<char> dec_baseline("baseline");
static SG::AuxElement::Decorator<char> dec_signal("signal");
static SG::AuxElement::Decorator<char> dec_passOR("passOR");
static SG::AuxElement::Decorator<char> dec_bad("bad");
static SG::AuxElement::Decorator<char> dec_bjet("bjet");
static SG::AuxElement::Decorator<char> dec_cosmic("cosmic");
static SG::AuxElement::Decorator<double> dec_effscalefact("effscalefact");
static SG::AuxElement::Decorator<char> dec_isol("isol");
static SG::AuxElement::Decorator<char> dec_tightBad("tightBad");

bool descend_on_pt(xAOD::IParticle* p1, xAOD::IParticle* p2){
    return p1->pt() > p2->pt();
}

bool get_smeared_info(ST::SUSYObjDef_xAOD& objTool, xAOD::JetContainer* jets,
        xAOD::MuonContainer* muons, xAOD::ElectronContainer* electrons,
        SmearedInfo& smeared_info);
int main( int argc, char* argv[] ) 
{
    // gErrorIgnoreLevel = kError;

    if (argc > 1 && string(argv[1]) == "help")
    {
        cout << argv[0] << " toberun.txt number_evts isData=0 isAtlfast=0 noSys=1 doSmear=0 doPhoton=0 debug=0" << endl;
        exit(0);
    }
    // The application's name:
    const char* APP_NAME = argv[ 0 ];
    string inputFileName = "toberun.txt";
    if( argc > 1 ){
        inputFileName = string( argv[1] ); 
    }

    // Initialise the application:
    CHECK( xAOD::Init( APP_NAME ) );

    ifstream input_file( inputFileName.c_str() );
    Info( APP_NAME, "Input file name: %s", inputFileName.c_str() );
    TString name_file;
    TChain* fc = new TChain("CollectionTree");
    uint64_t total_evts_pro = 0;
    double sum_of_evt_w = 0;
    double sum_of_evt_w_sq = 0;
    while ( input_file>>name_file ){ 
        uint64_t n_events_process = 0;
        double sum_of_evt_weights = 0;
        double sum_of_evt_weight_sqd = 0;
        CHECK(CPToolsHelper::GetProcessEventsInfo(name_file.Data(), 
                    n_events_process,
                    sum_of_evt_weights,
                    sum_of_evt_weight_sqd));
        total_evts_pro += n_events_process;
        sum_of_evt_w += sum_of_evt_weights;
        sum_of_evt_w_sq += sum_of_evt_weight_sqd;

        fc->Add(name_file);
    }
    /** 
    Info(APP_NAME, "Total events: %lu", total_evts_pro);
    Info(APP_NAME, "Sum of evt weights: %f", sum_of_evt_w);
    Info(APP_NAME, "Sum of evt weights sq: %f", sum_of_evt_w_sq);
    **/

    // Create a TEvent object:
    xAOD::TEvent event( xAOD::TEvent::kClassAccess );
    CHECK( event.readFrom( fc ) );
    Info( APP_NAME, "Number of events in the chain: %i",
            static_cast< int >( event.getEntries() ) );

    // StatusCode::enableFailure();
    // CP::SystematicCode::enableFailure();
    // CP::CorrectionCode::enableFailure();
    xAOD::TStore store;

    // Decide how many events to run over:
    Long64_t entries = event.getEntries();
    if( argc > 2 ) {
        const Long64_t e = atoll( argv[ 2 ] );
        if( e>0 && e < entries ) {
            entries = e;
        }
    }

    int isData = 0;
    int isAtlfast = 0; 
    bool no_sys = true;
    bool do_smear = false;
    bool do_photon = false;
    bool do_debug = false;

    for (int i= 3 ; i<argc ; i++) {
        const char* key = strtok(argv[i],"=") ;
        const char* val = strtok(0," ") ;
        Info( APP_NAME,  "processing key %s  with value %s", key, val );
        if (strcmp(key,"isData")==0) isData = atoi(val);
        if (strcmp(key,"isAtlfast")==0) isAtlfast = atoi(val);
        if (strcmp(key,"noSys")==0) no_sys = (bool)atoi(val);
        if (strcmp(key,"doSmear")==0) do_smear = (bool)atoi(val);
        if (strcmp(key,"doPhoton")==0) do_photon = (bool)atoi(val);
        if (strcmp(key,"debug")==0) do_debug = (bool)atoi(val);
    }

    Info( APP_NAME, "Number of events to process: %i", static_cast<int>( entries ) );
    if(!no_sys){
        cout << "you are running systematics! go out and have a coffee" << endl;
    }
    if (isData) {
        cout <<"You are running data, congratuations" << endl;
    }
    if(do_smear){
        cout <<"running smearing" << endl;
    }
    // GRL, JVT and other tools are moved to MyXAODTools
    CPToolsHelper* cp_tools = new CPToolsHelper();
    TrackBranch* track_br = new TrackBranch();
    PhotonBranch* photon_br = new PhotonBranch();
    JetBranch* jet_br = new JetBranch();


    // Create SUSYTools
    ST::SUSYObjDef_xAOD objTool("SUSYObjDef_xAOD");
    std::cout << " ABOUT TO INITIALIZE SUSYTOOLS " << std::endl;
    if(do_debug) objTool.msg().setLevel(MSG::VERBOSE);
    else objTool.msg().setLevel(MSG::ERROR);

    // Configure the SUSYObjDef instance
    ST::SettingDataSource data_source = isData ? ST::Data : (isAtlfast ? ST::AtlfastII : ST::FullSim);

    /* config the Pileup reweighting tools */
    string maindir(getenv("ROOTCOREBIN"));
    vector<string> prw_conf;
    prw_conf.push_back(maindir+"/data/MyXAODTools/merged_prw_mc15b_monojet_bkg.root");
    // prw_conf.push_back(maindir+"/data/MyXAODTools/merged_prw_monojet_signals.root");
    CHECK( objTool.setProperty("DataSource", data_source) );
    CHECK( objTool.setProperty("PRWConfigFiles", prw_conf) );

    vector<string> prw_lumicalc;
    prw_lumicalc.push_back(maindir+"/data/MyXAODTools/ilumicalc_histograms_None_276262-284484.root");
    CHECK( objTool.setProperty("PRWLumiCalcFiles", prw_lumicalc) );
    CHECK( objTool.setProperty("ConfigFile", maindir+"/data/MonoJet/monojet.conf") );

    if( objTool.initialize() != StatusCode::SUCCESS){
        Error( APP_NAME, "Cannot intialize SUSYObjDef_xAOD..." );
        Error( APP_NAME, "Exiting... " );
        exit(-1);
    }else{
        Info( APP_NAME, "SUSYObjDef_xAOD initialized... " );
    }
    std::cout << " INITIALIZED SUSYTOOLS " << std::endl;
    OutputTreeMan output = OutputTreeMan( !no_sys );
        
    /* Setup Jet Smearing Tools */
    SUSY::JetMCSmearingTool* m_mySmearingTool = NULL;
    TH2F* lightJetResponse = NULL;
    TH2F* bJetResponse = NULL;
    const std::string smearJet = "smearjet";
    if (do_smear) {
        unsigned int test = 2000;

        m_mySmearingTool = new SUSY::JetMCSmearingTool("MySmearingTool");
        m_mySmearingTool->setProperty("NumberOfSmearedEvents",test);
        m_mySmearingTool->initialize();
        
        bool do_p2411 = false;
        std::string input_light_jet(maindir+"/data/JetSmearing/MC15/R_map2015_bveto_OP77_EJES_p2411.root");
        if(!do_p2411){
            input_light_jet = string(maindir+"/data/JetSmearing/MC15/R_map2015_bveto_OP77_WEJES_p1886.root");
        }
        TFile* lightJetFile = TFile::Open(input_light_jet.c_str(), "read");
        if(do_p2411) lightJetResponse = (TH2F*)lightJetFile->Get("responseEJES_p2411");
        else lightJetResponse = (TH2F*)lightJetFile->Get("responseWEJES_p1886");
        lightJetResponse->SetDirectory(0);
        lightJetFile->Close();

        std::string input_bjet(maindir+"/data/JetSmearing/MC15/R_map2015_btag_OP77_EJES_p2411.root");
        if(!do_p2411){
            input_bjet = string(maindir+"/data/JetSmearing/MC15/R_map2015_btag_OP77_WEJES_p1886.root");
        }
        TFile* bJetFile = TFile::Open(input_bjet.c_str(), "read");
        if(do_p2411) bJetResponse = (TH2F*)bJetFile->Get("responseEJES_p2411");
        else bJetResponse = (TH2F*)bJetFile->Get("responseWEJES_p1886");
        bJetResponse->SetDirectory(0);
        bJetFile->Close();

        m_mySmearingTool->SetResponseMaps(lightJetResponse, bJetResponse);
    }
    

    TFile *fOutputFile = new TFile( "reduced_ntup.root", "recreate" );
    vector<ST::SystInfo> systInfoList;
    vector<ST::SystInfo> weightSys;
    if (no_sys || isData) {
        ST::SystInfo infodef;
        infodef.affectsKinematics = false;
        infodef.affectsWeights = false;
        infodef.affectsType = ST::Unknown;
        systInfoList.push_back(infodef);
        if(!isData) {
            for(const auto& sys_info: objTool.getSystInfoList()){
                if (!MonoJet::Helper::IsSysNeeded(sys_info)) continue;
                if (sys_info.affectsWeights){
                    weightSys.push_back(sys_info);
                }
            }
        }
    } else {
        systInfoList = objTool.getSystInfoList();
    }
    /*record the number of processed events*/
    auto* tree = new TTree("associate","associate");
    output.AddEventInfo2Tree(*tree);
    tree->Branch("nEventsProcessed", &total_evts_pro, "nEventsProcessed/l");
    tree->Branch("nSumEventWeights", &sum_of_evt_w, "nSumEventWeights/D");
    tree->Branch("nSumEventWeightsSquared", &sum_of_evt_w_sq, 
            "nSumEventWeightsSquared/D");
    for (Long64_t entry = 0; entry < 1; ++entry) {
        event.getEntry( entry );
        const xAOD::EventInfo* ei = 0;
        CHECK( event.retrieve( ei, "EventInfo" ) );
        output.FillEventInfo(*ei);
        tree ->Fill();
    }
    fOutputFile->cd();
    tree ->Write();
    
    for (const auto& sysInfo : systInfoList) {
        const CP::SystematicSet& sys = sysInfo.systset;
        string tree_name = "physics";

        if (!no_sys && !isData)
        {
            if (!sysInfo.affectsKinematics) continue;
            if (!MonoJet::Helper::IsSysNeeded(sysInfo)) continue;
            //Only MET stuff
            if(sys.name().find("MET_SoftTrk") == string::npos) continue;
            tree_name = MonoJet::Helper::parse_sys_name(sys.name().c_str());
            if (tree_name == "none") {
                cout << sys.name() << " does not fit the pattern" << endl;
                continue;
            }
        }

        TTree MyTree(tree_name.c_str(), tree_name.c_str());
        output.AddEventInfo2Tree(MyTree);
        output.AddBranches2Tree(MyTree);
        track_br->AttachBranchToTree(MyTree);
        photon_br->AttachBranchToTree(MyTree);
        jet_br->AttachBranchToTree(MyTree);
        auto* smeared_data = new vector<SmearedInfo>() ;
        MyTree.Branch("pseudoData", smeared_data);

        for( Long64_t entry = 0; entry < entries; ++entry ) {
            output.ClearBranch();
            track_br->ClearBranch();
            photon_br->ClearBranch();
            jet_br->ClearBranch();
            smeared_data->clear();


            // Tell the object which entry to look at:
            event.getEntry( entry );

            const xAOD::EventInfo* ei = 0;
            CHECK( event.retrieve( ei, "EventInfo" ) );

            output.FillEventInfo(*ei);
            if (!isData){
                output.pu_weight_ = objTool.GetPileupWeight();
                output.corrected_mu_ = objTool.GetCorrectedAverageInteractionsPerCrossing();
            } else {
                output.pu_weight_ = 1.0;
                output.corrected_mu_ = -99;
            }
            output.data_weight_ = 1.0;

           
            if(entry == 0){
                cout << "Dumping trigger info" << endl;
                auto chainGroup = objTool.GetTrigChainGroup("HLT_.*");
                for (auto& trig : chainGroup->getListOfTriggers())
                    cout << " " << trig << endl;
            } 
            
            
            if(! cp_tools->PassGRL(*ei)) continue;
            if(! cp_tools->PassEventCleaning(*ei)) continue;
            // Info(APP_NAME, "In event: %d", (int)ei->eventNumber());

            // if(ei->eventNumber() != 620) continue;
            // cout<< "reading 620!"<<endl;

            for(auto& kv : output.trigger_map_)
            {
                if(objTool.IsTrigPassed(kv.first.c_str())) kv.second = true;
            }

            //////////////////
            //primary vertex
            //////////////////
            const xAOD::VertexContainer* vertice = 0;
            CHECK( event.retrieve(vertice, "PrimaryVertices") );
            if(! cp_tools->HasPrimaryVertex(*vertice, 1)) continue;
            // Fill vertex info
            int n_track_rec = 0;
            for(const auto& vxp : *vertice){
                if(n_track_rec++ > 5) break;
                track_br->Fill(*vxp);
            }
            // Fill Track info
            const xAOD::TrackParticleContainer* tracks = 0;
            CHECK( event.retrieve(tracks, "InDetTrackParticles") );
            for(const auto& track : *tracks){
                if(track->pt() < 20e3) continue;
                track_br->FillTrack(*track, *vertice);
            }

            if (objTool.applySystematicVariation(sys) != CP::SystematicCode::Ok){
                Error(APP_NAME, "Cannot configure SUSYTools for systematic var. %s", (sys.name()).c_str() );
            }

            /*get physics objects*/
            xAOD::JetContainer* jets_copy;
            xAOD::ShallowAuxContainer* jets_copyaux;
            CHECK( objTool.GetJets(jets_copy,jets_copyaux, true) );

            xAOD::MuonContainer* muons_copy;
            xAOD::ShallowAuxContainer* muons_copyaux;
            CHECK( objTool.GetMuons(muons_copy, muons_copyaux, true) );


            xAOD::ElectronContainer* electrons_copy;
            xAOD::ShallowAuxContainer* electrons_copyaux;
            CHECK( objTool.GetElectrons(electrons_copy, electrons_copyaux, true) );

            ///////////////////////
            // do overlap removal before object selection
            // turn off the harmonization
            ///////////////////////
            // bool doHarmonization = false;
            CHECK( objTool.OverlapRemoval(electrons_copy, muons_copy, 
                        jets_copy) );

            //discard the event if any jets is labelled as 'bad'
            bool passJetCleaning = true;
            output.n_base_jet = 0;
            for(const auto& jet : *jets_copy){
                objTool.IsBadJet(*jet, -999.);
                objTool.IsBJet(*jet) ;
                if ( jet->pt() > 20e3 )
                {
                    if( dec_bad(*jet) && dec_passOR(*jet)){
                        passJetCleaning = false;
                        break;
                    }
                    output.n_base_jet ++;
                }
            }
            if ( !passJetCleaning ){ 
                store.clear();
                continue;
            }

            // xAOD::TauJetContainer* taus_copy = nullptr;
            // xAOD::ShallowAuxContainer* tausAux_copy = nullptr;
            // CHECK(objTool.GetTaus(taus_copy, tausAux_copy));
            
            /* ********
             * MET with muon invisible. 
             * ********/
            auto* met = new xAOD::MissingETContainer;
            auto* metAux = new xAOD::MissingETAuxContainer;
            met->setStore(metAux);
            CHECK( store.record( met, "MET_MyRefFinal" ) );
            CHECK( store.record( metAux, "MET_MyRefFinalAux." ) );
            CHECK( objTool.GetMET(*met,
                        jets_copy,
                        electrons_copy,
                        muons_copy, // muon term
                        nullptr, // photon
                        nullptr,//taus_copy,
                        true,  // TST
                        true, // JVT
                        muons_copy// 0 
                        ) );
            xAOD::MissingETContainer::const_iterator met_it = met->find("Final");

            if (met_it == met->end() )
            {
                Error( APP_NAME, "No RefFinal inside MET container" );
            }
            output.MET_etx = (*met_it)->mpx();
            output.MET_ety = (*met_it)->mpy();
            output.MET_phi = (*met_it)->phi();
            output.MET_et  = (*met_it)->met();
            output.MET_sumet_ = (*met_it)->sumet();
            xAOD::MissingETContainer::const_iterator met_muon_it = met->find("Muons");
            output.MET_sumet_noMuon_ = output.MET_sumet_ - (*met_muon_it)->sumet();
            output.MET_et_soft_ = (*(met->find("PVSoftTrk")))->met();

            /* ********
             * normal MET 
             * */
            auto* metNorm = new xAOD::MissingETContainer;
            auto* metNormAux = new xAOD::MissingETAuxContainer;
            metNorm->setStore(metNormAux);
            CHECK( store.record( metNorm, "MET_Norm" ) );
            CHECK( store.record( metNormAux, "MET_NormAux." ) );
            CHECK( objTool.GetMET(*metNorm,
                        jets_copy,
                        electrons_copy,
                        muons_copy, // muon term
                        nullptr, // photon
                        nullptr,//taus_copy,
                        true,  // TST
                        true, // JVT
                        0 // 0 
                        ) );
            xAOD::MissingETContainer::const_iterator metNorm_it = metNorm->find("Final");
            output.MET_mu_etx_ = (*metNorm_it)->mpx();
            output.MET_mu_ety_ = (*metNorm_it)->mpy();
            output.MET_mu_sumet_ = (*metNorm_it)->sumet();

            ////////////
            // MET, TST, with Photon, for gamma+jets studies
            /////////////
            if(do_photon)
            {
                xAOD::PhotonContainer* ph_copy = nullptr;
                xAOD::ShallowAuxContainer* phAux_copy = nullptr;
                CHECK(objTool.GetPhotons(ph_copy, phAux_copy, true));
                for(const auto& ph : *ph_copy){
                    if(! dec_baseline(*ph)) continue;
                    photon_br->Fill(*ph);
                }

                xAOD::MissingETContainer* met_ph = nullptr;
                xAOD::MissingETAuxContainer* met_ph_aux = nullptr;
                if(ph_copy == nullptr){
                    Error( APP_NAME, "No Photon container" );
                } else {
                    met_ph = new xAOD::MissingETContainer;
                    met_ph_aux = new xAOD::MissingETAuxContainer;
                    met_ph->setStore(met_ph_aux);
                    CHECK( store.record( met_ph, "MET_RefFinal_ph" ) );
                    CHECK( store.record( met_ph_aux, "MET_RefFinal_phAux." ) );
                    CHECK( objTool.GetMET(*met_ph,
                                jets_copy,
                                electrons_copy,
                                muons_copy, // muon term
                                ph_copy, // photon
                                nullptr,//taus_copy,
                                true,  // TST
                                true, // JVT
                                0 // invis
                                ) );
                    if (met_ph->find("Final") == met_ph->end())
                    {
                        Error( APP_NAME, "No RefFinal inside MET container" );
                    } else {
                        output.MET_ph_etx_ = (*met_ph)["Final"]->mpx();
                        output.MET_ph_ety_ = (*met_ph)["Final"]->mpy();
                        output.MET_ph_sumet_ = (*met_ph)["Final"]->sumet();
                    }
                }
            }

            /* Track Missing Et */
            auto* met_track = new xAOD::MissingETContainer;
            auto* metAux_track = new xAOD::MissingETAuxContainer;
            met_track->setStore(metAux_track);
            CHECK( store.record( met_track, "MET_TRACK_MyRefFinal" ) );
            CHECK( store.record( metAux_track, "MET_TRACK_MyRefFinalAux." ) );
            CHECK( objTool.GetTrackMET(*met_track,
                        jets_copy,
                        electrons_copy,
                        muons_copy
                        ) );
            xAOD::MissingETContainer::const_iterator met_track_it = met_track->find("Track");
            output.MET_trk_etx_ = (*met_track_it)->mpx();
            output.MET_trk_ety_ = (*met_track_it)->mpy();
            output.MET_trk_phi_ = (*met_track_it)->phi();
            output.MET_trk_sumet_ = (*met_track_it)->sumet();

            //////////////////////
            // Electron's selection
            ///////////////////////
            for(auto el = electrons_copy->begin(); el != electrons_copy->end(); el++)
            {
                if( !(bool) dec_baseline(**el) 
                     || !(bool) dec_passOR(**el)
                  ){ 
                    continue;
                }
                bool is_signal = dec_signal(**el); 
                float el_sf = objTool.GetSignalElecSF(**el);

                output.FillElectron(**el, *ei, *vertice, is_signal, el_sf);
                output.el_medium_.push_back(cp_tools->PassEleMediumLLH(**el));

                if(!no_sys && !isData) {  // don't do systematics
                    for(const auto& sys_info : weightSys){
                        if(sys_info.affectsType == ST::Electron){
                            const CP::SystematicSet& sys_weight = sys_info.systset;
                            if (objTool.applySystematicVariation(sys_weight) 
                                    != CP::SystematicCode::Ok){
                                Error(APP_NAME, "Cannot configure SUSYTools for systematic var. %s", 
                                        (sys_weight.name()).c_str() );
                            }
                            output.FillElectronScaleSys(sys_weight.name(), 
                                    (float)objTool.GetSignalElecSF(**el));
                        }
                    }
                    CHECK(objTool.resetSystematics());
                }
            }

            /* Get MET with electrons and muons treated as invisible.
             * */
            xAOD::IParticleContainer tmpGoodLeptons(SG::VIEW_ELEMENTS);
            for(auto mu : *muons_copy){ 
                if ( (bool) dec_baseline(*mu) && 
                     (bool) dec_passOR(*mu)
                   ){
                    tmpGoodLeptons.push_back(mu);
                }
            }
            for(auto el : *electrons_copy){ 
                if((bool) dec_signal(*el) && 
                   (bool) dec_passOR(*el) && 
                   (bool) dec_baseline(*el)
                   ){  
                    tmpGoodLeptons.push_back(el);
                }
            }
            
            auto* metInviEle = new xAOD::MissingETContainer;
            auto* metInviEleAux = new xAOD::MissingETAuxContainer;
            metInviEle->setStore(metInviEleAux);
            CHECK( store.record( metInviEle, "MET_InviEle" ) );
            CHECK( store.record( metInviEleAux, "MET_InviEleAux." ) );
            CHECK( objTool.GetMET(*metInviEle,
                        jets_copy,
                        electrons_copy,
                        muons_copy, // muon term
                        nullptr, // photon
                        nullptr,//taus_copy,
                        true,  // TST
                        true, // JVT
                        &tmpGoodLeptons// 0 
                        ) );
            xAOD::MissingETContainer::const_iterator met_it_inviEle = metInviEle->find("Final");
            output.MET_noEM_et_ = (*met_it_inviEle)->met();
            output.MET_noEM_phi_ = (*met_it_inviEle)->phi();
            output.MET_noEM_sumet_ = (*met_it_inviEle)->sumet();

            sort(jets_copy->begin(), jets_copy->end(), descend_on_pt);
            output.n_good_jet = 0;
            output.n_jet_btagged = 0;
            for(auto jet_itr = jets_copy->begin(); 
                    jet_itr != jets_copy->end(); jet_itr++)
            {
                xAOD::Jet* thisJet = (*jet_itr); 
                if (dec_baseline(**jet_itr) && dec_passOR(**jet_itr)){
                    thisJet->auxdata<char>(smearJet) = true;
                }

                bool is_signal = dec_signal(**jet_itr);
                if(!is_signal || !dec_passOR(**jet_itr) ) continue;

                double dphi = fabs(TVector2::Phi_mpi_pi(output.MET_phi - (*jet_itr)->phi()));
                dec_dphi_MET(**jet_itr) = dphi;
                if( dec_bjet(**jet_itr)) output.n_jet_btagged += 1;

                jet_br->Fill(**jet_itr);
                output.FillJet(**jet_itr);
                dec_tightBad(**jet_itr) = jet_br->jet_isBadTight_->at(output.n_good_jet);
                output.n_good_jet ++;
            }
            if (output.n_good_jet < 1 || 
                    output.jet_p4_.at(0).Pt()/1e3 < 100 ||
                    jet_br->jet_isBadTight_->at(0) != 1 ||
                    fabs(output.jet_p4_.at(0).Eta()) >= 2.4
                    )
            {
                store.clear();
                continue;
            }
            if(do_smear){
                std::vector<std::unique_ptr<SmearData > > smrMc;
                m_mySmearingTool->DoSmearing(smrMc,*jets_copy);
                for (auto& SmearedEvent : smrMc){
                    xAOD::JetContainer* theJetContainer =  SmearedEvent->jetContainer;
                    SmearedInfo smeared;
                    if (get_smeared_info(objTool, theJetContainer, 
                            muons_copy, electrons_copy, smeared)){
                        smeared_data->push_back(smeared);
                    }
                }
            }

            /////////////////// 
            // Muon's Selection
            /////////////////// 
            output.has_cosmic_muon_ = false;
            output.has_bad_muon_ = false;
            for(auto mu_itr = muons_copy->begin(); 
                    mu_itr != muons_copy->end(); ++mu_itr)
            {
                if( !dec_baseline(**mu_itr) ) continue;
                if(dec_bad(**mu_itr)) output.has_bad_muon_ = true;
                if(!dec_passOR(**mu_itr)) continue;
                // !dec_isol(**mu_itr)
                if(dec_cosmic(**mu_itr)) output.has_cosmic_muon_ = true;
                bool signal = dec_signal(**mu_itr);
                float muon_sf = objTool.GetSignalMuonSF(**mu_itr);
                output.FillMuon(**mu_itr, *ei, *vertice, signal, muon_sf);

                if(!no_sys && !isData){ //
                    for(const auto& sys_info : weightSys){
                        if(sys_info.affectsType == ST::Muon){
                            const CP::SystematicSet& sys_weight = sys_info.systset;
                            if (objTool.applySystematicVariation(sys_weight) 
                                    != CP::SystematicCode::Ok){
                                Error(APP_NAME, "Cannot configure SUSYTools for systematic var. %s", 
                                        (sys_weight.name()).c_str() );
                                continue;
                            }
                            output.FillMuonScaleSys(sys_weight.name(), (float)objTool.GetSignalMuonSF(**mu_itr));
                        }
                    }
                    CHECK(objTool.resetSystematics());
                }
            }


            output.FillMonoJet();

            // The containers created by the shallow copy are owned by you. 
            // Remember to delete them
            store.clear();
            MyTree.Fill();
        }

        if(smeared_data) delete smeared_data;
        fOutputFile->cd();
        MyTree.Write();
        CHECK(objTool.resetSystematics());
    }

    fOutputFile->cd();
    fOutputFile->Close();
    Info( APP_NAME, "finished analysis; Cleaning..." );

    delete cp_tools;
    delete track_br;
    delete photon_br;
    delete jet_br;
    if(m_mySmearingTool) delete m_mySmearingTool;
    if(lightJetResponse) delete lightJetResponse;
    if(bJetResponse) delete bJetResponse;

    Info( APP_NAME, "Successfully finished analysis; Exitting..." );
    return 1;
}
bool get_smeared_info(ST::SUSYObjDef_xAOD& objTool, xAOD::JetContainer* jets,
        xAOD::MuonContainer* muons, xAOD::ElectronContainer* electrons,
        SmearedInfo& smeared_info)
{
    sort(jets->begin(), jets->end(), descend_on_pt);
    smeared_info.leading_jet_pt_ = (float)jets->at(0)->p4().Pt();
    smeared_info.leading_jet_eta_ = (float)jets->at(0)->p4().Eta();
    if(smeared_info.leading_jet_pt_ < 100E3 || 
            fabs(smeared_info.leading_jet_eta_) >= 2.4 ||
            dec_tightBad(*(jets->at(0))) != 1 ||
            jets->at(0)->auxdata< char >("smearjet") == false
    ) return false;
    auto* met = new xAOD::MissingETContainer;
    auto* metAux = new xAOD::MissingETAuxContainer;
    met->setStore(metAux);
    objTool.GetMET(*met,
                jets,
                electrons,
                muons, // muon term
                nullptr, // photon
                nullptr,//taus_copy,
                true,  // TST
                true, // JVT
                muons // 0 
                );
    xAOD::MissingETContainer::const_iterator met_it = met->find("Final");

    if (jets->size() > 1) {
        smeared_info.sub_leading_jet_pt_ = (float)jets->at(1)->p4().Pt();
        smeared_info.sub_leading_jet_eta_ = (float)jets->at(1)->p4().Eta();
    }
    smeared_info.met_ =(float) (*met_it)->met();
    smeared_info.sum_et_ =(float) (*met_it)->sumet();
    float min_dphi_jetMET  = 9999;
    int n_good_jets = 0;
    for(auto jet: *jets) {
        if ( jet->auxdata< char >("smearjet") == false ) continue;
        if(jet->pt() <= 30E3 || fabs(jet->eta()) >= 2.8)  continue;
        float dphi = (float) fabs(TVector2::Phi_mpi_pi((*met_it)->phi() - jet->phi()));
        n_good_jets ++;
        if(dphi < min_dphi_jetMET) min_dphi_jetMET = dphi;
        // bool is_signal = objTool.IsSignalJet((*jet), 30e3, 2.8, 0.64);
    }
    smeared_info.min_jets_met_ = min_dphi_jetMET;
    smeared_info.n_good_jets_ = n_good_jets;

    /* Track Missing Et */
    auto* met_track = new xAOD::MissingETContainer;
    auto* metAux_track = new xAOD::MissingETAuxContainer;
    met_track->setStore(metAux_track);
    objTool.GetTrackMET(*met_track,
                jets,
                electrons,
                muons
                );
    xAOD::MissingETContainer::const_iterator met_track_it = met_track->find("Track");
    smeared_info.dphi_EP_ = fabs(TVector2::Phi_mpi_pi((*met_track_it)->phi() - (*met_it)->phi()));

    delete met;
    delete metAux;
    delete met_track;
    delete metAux_track;
    return true;
}
