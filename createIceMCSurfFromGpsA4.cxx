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


  
  static const int npointThresh = 1640;
  Float_t threshScanThresh[2][48][npointThresh]; // adc thresholds from threshold scan
  Float_t threshScanScaler[2][48][npointThresh]; // scalers from threshold scan
  Float_t minadcthresh[2][48];
  Float_t maxadcthresh[2][48];


  // Reading in last threshold scan before Anita-4 flight
  // Run 11927
  TFile *fthresh = new TFile ("data/threshScan_anita4.root");
  TGraph *gtemp;double rateToThreshold(double rate, int band);
  
  double *x, *y;
  for (int ipol=0;ipol<2;ipol++){
    for (int iant=0;iant<48;iant++){
      gtemp = (TGraph*)fthresh->Get(Form("g_%i_%i", ipol, iant));
      x = gtemp->GetX();
      y = gtemp->GetY();
      for (int i=0;i<npointThresh;i++){
	threshScanThresh[ipol][iant][i] = (Int_t)x[i];
	threshScanScaler[ipol][iant][i] = (Int_t)y[i];
      }
      minadcthresh[ipol][iant]=TMath::MinElement(npointThresh, x);
      maxadcthresh[ipol][iant]=TMath::MaxElement(npointThresh, x);
    }
  }

  // // This channel was turned off during the threshold scan
  // // We are then using the scan for a different channel
  // // that had very similar thresholds during the flight
  // for (int i=0;i<npointThresh;i++){
  //   threshScanThresh[0][35][i] = threshScanThresh[0][20][i];
  //   threshScanScaler[0][35][i] = threshScanScaler[0][20][i];
  // }
  // minadcthresh[0][35] = minadcthresh[0][20];
  // maxadcthresh[0][35] = maxadcthresh[0][20];
  
  delete gtemp;
  delete fthresh;

  
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
    sprintf(surfName,"/unix/anita4/flight2016/root/run%d/surfHkFile%d.root",run,run);
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
  
  Double_t fakeThresholds[2][48];
  Double_t fakeThresholds2[2][48]; 
  Double_t fakeScalers[2][48];    

  Float_t threshadc;
  Double_t thisrate;
  
  double constant = 5.87953;
  double slope = -1.32115;
  double thispowerthresh=999.;

  UInt_t realTime;

  TTree* tree = new TTree();
  tree->Branch("realTime",         &realTime,        "realTime/i"                  );
  tree->Branch("thresholds",        thresholds,      "thresholds[2][48]/s"         );
  tree->Branch("scalers",           scalers,         "scalers[2][48]/s"            );
  tree->Branch("fakeThreshold",     fakeThresholds,  "fakeThresholds[2][48]/D"     );
  tree->Branch("fakeThreshold2",    fakeThresholds2, "fakeThresholds2[2][48]/D"    );
  tree->Branch("fakeScaler",        fakeScalers,     "fakeScalers[2][48]/D"        );

  // ProgressBar p(numGpsEntries);

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
	  int iant=iring*16+iphi;
	  thresholds[ipol-2][iant] = surf->getThreshold(iphi, (AnitaRing::AnitaRing_t)iring, (AnitaTrigPol::AnitaTrigPol_t)ipol);
	  //	  cout << ipol << " " << iring*16+iphi <<  " " << thresholds[ipol][iring*16+iphi] << endl;
	  scalers[ipol-2][iant] = surf->getScaler(iphi, (AnitaRing::AnitaRing_t)iring, (AnitaTrigPol::AnitaTrigPol_t)ipol);



	  threshadc = (Float_t)thresholds[ipol-2][iant];
	
	  index=TMath::BinarySearch(npointThresh, threshScanThresh[ipol-2][iant], threshadc);
	  
	  thisrate=(double)threshScanScaler[ipol-2][iant][index]; // these scalers are in kHz
	  
	  thisrate=thisrate/1.e3; // put it in MHz
	  
	  thispowerthresh=(log(thisrate)- constant)/slope;
	  	  
	  fakeScalers[ipol-2][iant]=thisrate*1.e3;
	  fakeThresholds[ipol-2][iant]=thispowerthresh;
	  fakeThresholds2[ipol-2][iant]=(log(scalers[ipol-2][iant]/1.e3)-constant)/slope;


	  
	}
      }
    }
    
    

    tree->Fill();
    
    // p++;
    
  }
  
  
  cout << " Writing output " << endl;
  TFile *output = new TFile (Form("partial/SampleSurf_run%d_anita4.root", firstRun), "recreate");
  tree->Write("surf_icemc");
  cout << "Done " << endl;
  output->Close();
  cout << "Closed " << endl;

  return 0;
}
