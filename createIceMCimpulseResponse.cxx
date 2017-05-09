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
#include <stdio.h>
#include <stdlib.h>

using namespace std;


int main(){

  const char *anitaInstallDir= getenv("ANITA_UTIL_INSTALL_DIR");

  cout << anitaInstallDir << endl;

  char fileName[400];
  
  string spol[2] ={"V", "H"};
  string sring[3]={"T", "M", "B"};

  TFile *output = new TFile("Anita3_ImpulseResponseDigitizer.root", "recreate");

  for (int ipol=0; ipol<2; ipol++){
    for (int iring=0; iring<3; iring++){
      for (int iphi=0; iphi<16; iphi++){

	sprintf(fileName, "%s/share/UCorrelator/responses/IndividualBRotter/%02d%s%s.imp", anitaInstallDir, iphi+1, sring[iring].c_str(), spol[ipol].c_str());
	
	cout << fileName << endl;
	TGraph *g = new TGraph (fileName);
	g->SetTitle(Form("Impulse Response: %02d%s%s; Time [ns]; Amplitude [V]", iphi+1, sring[iring].c_str(), spol[ipol].c_str()));

	g->Write(Form("g%02d%s%s", iphi+1, sring[iring].c_str(), spol[ipol].c_str() ));

	delete g;
      }
    }
  }


  sprintf(fileName, "%s/share/UCorrelator/responses/SingleBRotter/all.imp", anitaInstallDir);
  
  cout << fileName << endl;
  TGraph *g = new TGraph (fileName);
  g->SetTitle(Form("Impulse Response: All; Time [ns]; Amplitude [V]"));
  g->Write(Form("gAll"));
  
  output->Write();
  output->Close();
  



  return 0;
}
