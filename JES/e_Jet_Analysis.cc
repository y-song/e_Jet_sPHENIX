#include <TFile.h>
#include <TTree.h>
#include <TLorentzVector.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TPaveStats.h>
#include "Math/LorentzVector.h"
#include "Math/Vector4D.h"
#include <TLegend.h>
#include <TROOT.h>
#include <TApplication.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TH2D.h>
#include <TF1.h>
#include <TLatex.h>
#include <TColor.h>
#include <THStack.h>
#include <TProfile.h>
#include <iostream>
#include <fstream>
#include <TMath.h>
#include <vector>
#include <math.h>

#define NTRACK_MAX (1U << 14)

const int MAX_INPUT_LENGTH = 200;
int counter = 0;

float calc_Q_square(float inE, TLorentzVector v)
{
  float Qs = 2.*inE*v.E()*(1-TMath::Abs(v.CosTheta()));
  return Qs;
}

int main(int argc, char *argv[])
{
  if (argc < 2) {
    std::cout<<"Syntax: [Command] [File]"<<std::endl;
    exit(EXIT_FAILURE);
  }
  int iarg = 1;
  TString root_file = (TString)argv[iarg];
  
  std::cout << "Opening: " << (TString)argv[iarg] << std::endl;
  
  TFile *file = TFile::Open(root_file);
  
  if (file == NULL) {
    std::cout << " File Fail" << std::endl;
    exit(EXIT_FAILURE); 
  } 
  
  file->Print();
  
  TTree *_tree_event = dynamic_cast<TTree *>(file->Get("T"));

  if (_tree_event == NULL) {
    std::cout << " Tree Fail " << std::endl;
    exit(EXIT_FAILURE);
  }

  //Declare Leaf Types
  Int_t           m_event;
  enum {MaxNumJets = 10};
  std::array <Int_t,MaxNumJets>           id;
  std::array <Int_t,MaxNumJets>           nComponent;
  std::array <Float_t,MaxNumJets>         eta;
  std::array <Float_t,MaxNumJets>         phi;
  std::array <Float_t,MaxNumJets>         e;
  std::array <Float_t,MaxNumJets>         pt;
  std::array <Int_t,MaxNumJets>           truthID;
  std::array <Int_t,MaxNumJets>           truthNComponent;
  std::array <Float_t,MaxNumJets>         truthEta;
  std::array <Float_t,MaxNumJets>         truthPhi;
  std::array <Float_t,MaxNumJets>         truthE;
  std::array <Float_t,MaxNumJets>         truthPt;

  //Electron Truth Variables
  Float_t         etruthEta;
  Float_t         etruthPhi;
  Float_t         etruthE;
  Float_t         etruthPt;
  Float_t         etruthpX;
  Float_t         etruthpY;
  Float_t         etruthpZ;
  Int_t           etruthPID;
  Int_t           etruthParentID;

  //Declare Branches
  TBranch        *b_event;
  TBranch        *b_id;
  TBranch        *b_nComponent;
  TBranch        *b_eta;
  TBranch        *b_phi;
  TBranch        *b_e;
  TBranch        *b_pt;
  TBranch        *b_truthID;
  TBranch        *b_truthNComponent;
  TBranch        *b_truthEta;
  TBranch        *b_truthPhi;
  TBranch        *b_truthE;
  TBranch        *b_truthPt;
  TBranch        *b_etruthEta;
  TBranch        *b_etruthPhi;
  TBranch        *b_etruthE;
  TBranch        *b_etruthPt;
  TBranch        *b_etruthpX;
  TBranch        *b_etruthpY;
  TBranch        *b_etruthpZ;
  TBranch        *b_etruthPID;
  TBranch        *b_etruthParentID;

  
  _tree_event->SetBranchAddress("m_event", &m_event, &b_event);
  _tree_event->SetBranchAddress("id", &id, &b_id);
  _tree_event->SetBranchAddress("nComponent", &nComponent, &b_nComponent);
  _tree_event->SetBranchAddress("eta", &eta, &b_eta);
  _tree_event->SetBranchAddress("phi", &phi, &b_phi);
  _tree_event->SetBranchAddress("e", &e, &b_e);
  _tree_event->SetBranchAddress("pt", &pt, &b_pt);
  _tree_event->SetBranchAddress("truthID", &truthID, &b_truthID);
  _tree_event->SetBranchAddress("truthNComponent", &truthNComponent, &b_truthNComponent);
  _tree_event->SetBranchAddress("truthEta", &truthEta, &b_truthEta);
  _tree_event->SetBranchAddress("truthPhi", &truthPhi, &b_truthPhi);
  _tree_event->SetBranchAddress("truthE", &truthE, &b_truthE);
  _tree_event->SetBranchAddress("truthPt", &truthPt, &b_truthPt);
 
  _tree_event->SetBranchAddress("etruthEta", &etruthEta, &b_etruthEta);
  _tree_event->SetBranchAddress("etruthPhi", &etruthPhi, &b_etruthPhi);
  _tree_event->SetBranchAddress("etruthE", &etruthE, &b_etruthE);
  _tree_event->SetBranchAddress("etruthPt", &etruthPt, &b_etruthPt);
  _tree_event->SetBranchAddress("etruthpX", &etruthpX, &b_etruthpX);
  _tree_event->SetBranchAddress("etruthpY", &etruthpY, &b_etruthpY);
  _tree_event->SetBranchAddress("etruthpZ", &etruthpZ, &b_etruthpZ);
  _tree_event->SetBranchAddress("etruthPt", &etruthPt, &b_etruthPt);
  _tree_event->SetBranchAddress("etruthPID", &etruthPID, &b_etruthPID);
  
  gStyle->SetOptStat("emr");
  gStyle->SetStatY(0.85);
  gStyle->SetStatX(0.87);
  gStyle->SetStatW(0.15);
  gStyle->SetMarkerStyle(20);
  gStyle->SetMarkerSize(1.2);
  gStyle->SetHistLineWidth(2.);
  gStyle->SetLineStyleString(2,"[12 12]"); // postscript dashes  
  TH1::SetDefaultSumw2(true);
  TH2::SetDefaultSumw2(true);
  
  //2D Histos
  TH2F * Tjve = new TH2F("ETrueJet_vs_Eelectron", "E^{True}_{Jet} (|#eta^{Jet}|<0.7) vs. E_{e}^{True}",100,0,25,100,0,25);
  TH2F * Rjve = new TH2F("ERecoJet_vs_Eelectron", "E^{Reco}_{Jet} (|#eta^{Jet}|<0.7) vs. E_{e}^{True}",100,0,25,100,0,25);
  //TH2F * eff = new TH2F("eff_vs_ETrueJet", "Efficiency vs E_{jet}^{tru}", 30, 0, 30, 100, 0, 1);

  //1D Histos
  //Ratio Histos
  TH1F * eoTj = new TH1F("ETrueJet_over_Eelectron", "E_{Reco}^{True}/E^{True}_{e} (|#eta^{Jet}|<0.7)",80,0,2);
  TH1F * eoRj = new TH1F("Eelectron_over_ERecoJet_over_Eelectron", "E_{Jet}^{Reco}/E^{True}_{e} (|#eta^{Jet}|<0.7)",80,0,2);
  TH1F * RjoTj = new TH1F("ERecoJet_over_ETrueJet", "E_{Jet}^{True}/E^{Reco}_{Jet} (|#eta^{Jet}|<0.7)",80,0,2);
  //Difference Histos
  TH1F * emTj = new TH1F("Eelectron_minus_ETrueJet", "E_{e}^{True} - E^{True}_{Jet} (|#eta^{Jet}|<0.7)",100,-20,30);
  TH1F * emRj = new TH1F("Eelectron_minus_ERecoJet", "E_{e}^{True} - E^{Reco}_{Jet} (|#eta^{Jet}|<0.7)",100,-20,30);
  //Detector Coordinate Histos
  TH1F * dPhiTj = new TH1F("dPhi_e_TrueJet", "|#Delta#varphi| (#varphi_{e} - #varphi^{True}_{Jet}) ", 32,0,M_PI);
  TH1F * dPhiRj = new TH1F("dPhi_e_RecoJet", "|#Delta#varphi| #varphi_{e} - #varphi(Jet^{Reco}_{Jet}) ", 32,0,M_PI);
  TH1F * dEtaTj = new TH1F("dEta_e_TrueJet", "|#Delta#eta| (#eta_{e} - #eta^{True}_{Jet})", 80,-10,10);
  TH1F * dEtaRj = new TH1F("dEta_e_RecoJet", "|#Delta#eta| (#eta_{e} - #eta^{Reco}_{Jet})", 80,-10,10);
  //Q^2
  TH1F * Q2 = new TH1F("Q2","Q^{2}",500,0,500);

  int count [30] = {0};
  int count_rec [30] = {0};

  Long64_t nentries = _tree_event->GetEntries();
  for(Long64_t ie = 0; ie < nentries ; ie++){
    _tree_event->GetEntry(ie); //each entry is a 5GeV Electron
    fprintf(stderr, "\r%s:%d: %llu / %llu", __FILE__, __LINE__, ie, nentries);

    //Efficiency
    for (int i = 0; i < 30; i++){
	if (truthE > i && truthE < i+1){
		count[i] += 1;
		if (TMath::Abs(eta-truthEta) < 0.5){
			count_rec[i] += 1;
		}
	}
    }

  } //entry loop
    
  //Write to new file
  FILE *f;
  char output[] = "eff.txt";
  f = fopen(output, "w");
  for (int j = 0; j < 30; j++){
  	fprintf(f, "%d\t%d\n", count[j], count_rec[j]);
  }
  fclose(f);
}
