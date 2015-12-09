#include <stdlib.h>

#include <TChain.h>
#include <TFile.h>

// Infrastructure include(s):
#ifdef ROOTCORE
#   include "xAODRootAccess/Init.h"
#   include "xAODRootAccess/TEvent.h"
#endif // ROOTCORE

#include "CPAnalysisExamples/errorcheck.h"
#include "MonoJet/OutputTreeMan.h"
#include "MyXAODTools/CPToolsHelper.h"
int main( int argc, char* argv[] ) 
{
    if (argc > 1 && string(argv[1]) == "help")
    {
        cout << argv[0] << " toberun.txt" << endl;
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

    // Create a TEvent object:
    xAOD::TEvent event( xAOD::TEvent::kClassAccess );
    CHECK( event.readFrom( fc ) );
    Info( APP_NAME, "Number of events in the chain: %i",
            static_cast< int >( event.getEntries() ) );

    OutputTreeMan output = OutputTreeMan(false);

    TFile *fOutputFile = new TFile( "reduced_ntup.root", "recreate" );
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

    delete fc;
    fOutputFile->Close();
    return 0;
}
