#include "AnitaConventions.h"
#include "UsefulAnitaEvent.h"
#include "RawAnitaEvent.h"
#include "SurfHk.h"
#include "TimedAnitaHeader.h"
#include "ProgressBar.h"
#include "PrettyAnitaHk.h"
#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TH2.h"
#include "TColor.h"
#include "TMath.h"
#include "TStyle.h"
#include "TSystem.h"
#include <iostream>
#include <fstream>

using namespace Acclaim;

using namespace std;

int main(int argc, char *argv[]){

  if(!(argc==3 || argc==2)){
    std::cerr << "Usage 1: " << argv[0] << " [run]" << std::endl;
    std::cerr << "Usage 2: " << argv[0] << " [firstRun] [lastRun]" << std::endl;
    return 1;
  }

  std::cout << argv[0] << "\t" << argv[1];
  if(argc==3){std::cout << "\t" << argv[2];}
  std::cout << std::endl;
  const Int_t firstRun = atoi(argv[1]);
  const Int_t lastRun = argc==3 ? atoi(argv[2]) : firstRun;


  FileStat_t staty;
  char surfName[1000];
  // char gpsName[FILENAME_MAX];

  AnitaVersion::set(4);

  TFile *finput = new TFile("anita4gps_pitchroll.root", "read");
  TTree *icemcGpsTree = (TTree*)finput->Get("adu5PatTree");
  icemcGpsTree->SetName("icemcGpsTree");
  
  Int_t icemcRealTime;
  icemcGpsTree->SetBranchAddress("realTime", &icemcRealTime);

  Int_t numGpsEntries = icemcGpsTree->GetEntries();

  SurfHk *surf=NULL;
  // Adu5Pat *pat = 0;
  TChain *surfChain = new TChain("surfHkTree");
  // TChain *gpsChain = new TChain("adu5PatTree");

  for (int run=firstRun;run<lastRun+1; run++){
    sprintf(surfName,"$unix/anita4/flight2016/root/run%d/surfHkFile%d.root",run,run);
    // sprintf(gpsName,"/unix/anita3/flight1415/root/run%d/gpsFile%d.root",run,run);
    if(gSystem->GetPathInfo(surfName,staty)) {
      continue;
    }
    surfChain->Add(surfName);
    // gpsChain->Add(gpsName);
  }

  surfChain->SetBranchAddress("surf",&surf);
  // gpsChain->SetBranchAddress("pat", &pat);

  surfChain->BuildIndex("surf->realTime");

  UShort_t thresholds[2][48];
  UShort_t scalers[2][48];
  UInt_t realTime;

  TTree* tree = new TTree();
  tree->Branch("realTime",          &realTime,    "realTime/i"           );
  tree->Branch("thresholds",        thresholds,   "thresholds[2][48]/s"  );
  tree->Branch("scalers",           scalers,      "scalers[2][48]/s"     );


  ProgressBar p(numGpsEntries);

  cout << "Total entries " << numGpsEntries << endl;

  for (UInt_t ientry=0; ientry<numGpsEntries; ientry++){
    icemcGpsTree->GetEntry(ientry);
    int index = surfChain->GetEntryNumberWithIndex(icemcRealTime);
    
    if (index<0){
      //      cout << " I couldn't find " << icemcRealTime << " . Getting entry with best index." << endl;
      index = surfChain->GetEntryNumberWithBestIndex(icemcRealTime);

    } 
    surfChain->GetEntry(index);
    realTime = surf->realTime;
    
    if (TMath::Abs((int)icemcRealTime-(int)realTime)>60) cout << icemcRealTime << " " << realTime << endl;

  
    for (int ipol=AnitaTrigPol::kHorizontal;ipol!=AnitaTrigPol::kNotATrigPol;ipol++){
      for(int iring=AnitaRing::kTopRing;iring!=AnitaRing::kNotARing;iring++){
	for (int iphi=0;iphi<16;iphi++){
	  thresholds[ipol-2][iring*16+iphi] = surf->getThreshold(iphi, (AnitaRing::AnitaRing_t)iring, (AnitaTrigPol::AnitaTrigPol_t)ipol);
	  //	  cout << ipol << " " << iring*16+iphi <<  " " << thresholds[ipol][iring*16+iphi] << endl;
	  scalers[ipol-2][iring*16+iphi] = surf->getScaler(iphi, (AnitaRing::AnitaRing_t)iring, (AnitaTrigPol::AnitaTrigPol_t)ipol);
	}
      }
    }
    
    

    tree->Fill();
    
    p++;
    
  }
  
  
  cout << " Writing output " << endl;
  TFile *output = new TFile (Form("partial/SampleSurf_run%d_anita4.root", firstRun), "recreate");
  tree->Write("surf_icemc");
  cout << "Done " << endl;
  output->Close();
  cout << "Closed " << endl;

  return 0;
}
