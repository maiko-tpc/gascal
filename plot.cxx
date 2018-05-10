#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <TCanvas.h>
#include <TROOT.h>
#include <TApplication.h>
#include <TGraph.h>
#include <TAxis.h>
#include <TH1S.h>

#include "MediumMagboltz.hh"
#include "FundamentalConstants.hh"

using namespace Garfield;
using namespace std;

int main(int argc, char * argv[]){
  
  double frac1, frac2, frac3;
  double press;
  char *gas1, *gas2, *gas3;
  char gasfile[100], gtitle[100];
  char *opt;

  if(argc == 9){
    gas1 =argv[1];
    frac1=atof(argv[2]);
    gas2 =argv[3];
    frac2=atof(argv[4]);
    gas3 =argv[5];
    frac3 =atof(argv[6]);
    press=atof(argv[7]);
    opt=argv[8];
  }


  else if(argc == 7){
    gas1 =argv[1];
    frac1=atof(argv[2]);
    gas2 =argv[3];
    frac2=atof(argv[4]);
    press=atof(argv[5]);
    opt=argv[6];

    if(fabs((frac1+frac2)-100) > 0.000001){
      cout << "" << endl;
      cout << "   error: frac1 + frac2 must be 100.0 !!" << endl;
      cout << "" << endl;
      return 0;
    }
    
  }

  else if(argc == 5){
    gas1 =argv[1];
    frac1=atof(argv[2]);
    press=atof(argv[3]);
    opt=argv[4];

    if(frac1 != 100.0){
      cout << "" << endl;
      cout << "    useage: ./plot [gas1] 100 [hPa] [opt]" << endl;
      cout << "" << endl;
      return 0;
    }
  }

  else if(argc == 3){
    opt=argv[2];
  }

  else {
    cout << "" << endl;
    cout << "    useage: ./plot [gas1] [frac1] [gas2] [frac2] [gas3] [frac3] [hPa] [opt]" << endl;
    cout << "    [opt] = dv     (for drift velocity)" << endl;
    cout << "            dt     (for transverse diffusion)" << endl;
    cout << "            dt     (for longitudinal diffusion)" << endl;
    cout << "            town   (for townsend coefficient)" << endl;
    cout << "            attach (for attachment coefficient)" << endl;
    cout << "" << endl;
    return 0;
  }

  
  // input files
  /* 3 gas mix */
  if(argc == 9){
  sprintf(gasfile, "gasfile/%s(%s)+%s(%s)+%s(%s)_%s.gas", 
	  argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], argv[7]);

  sprintf(gtitle, "%s(%s%) + %s(%s%) + %s(%s%) %s hPa", 
	  argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], argv[7]);
  }

  /* 2 gas mix */
  if(argc == 7){
  sprintf(gasfile, "gasfile/%s(%s)+%s(%s)_%s.gas", 
	  argv[1], argv[2], argv[3], argv[4], argv[5]);

  sprintf(gtitle, "%s(%s%) + %s(%s%) %s hPa", 
	  argv[1], argv[2], argv[3], argv[4], argv[5]);
  }

  /* 1 gas mix */
  if(argc == 5){
  sprintf(gasfile, "gasfile/%s(%s)_%s.gas", 
	  argv[1], argv[2], argv[3]);

  sprintf(gtitle, "%s(%s%)_%s hPa", 
	  argv[1], argv[2], argv[3]);
  }

  /* give the filename */
  if(argc == 3){
    sprintf(gasfile, "%s", argv[1]);
    sprintf(gtitle, "%s", argv[1]);
  }
  
  FILE *fp;
  if((fp = fopen(gasfile, "r")) == NULL){
    cout << "" << endl;
    cout << "******************************" << endl;
    cout << "Gasfile doesn't exist!!"        << endl;
    cout <<  "First, you have to run mag."   << endl;
    cout << "******************************" << endl;
    cout << "" << endl;    
    return 0;
  }

  else{
    TApplication app("app", &argc, argv);
  
    // load the gasfile
    MediumMagboltz* gas = new MediumMagboltz();
    gas -> LoadGasFile(gasfile);
    std::vector<double> efields, bfields, angles;
    
    gas -> GetFieldGrid(efields, bfields, angles);
    double ef[efields.size()];
    double driftv[efields.size()];
    double diff_t[efields.size()], diff_l[efields.size()];
    double town_a[efields.size()], attach[efields.size()];


    for(int i=0; i<efields.size(); i++){
      gas->GetElectronVelocityE(i, 0, 0, driftv[i]);
      gas -> GetElectronTransverseDiffusion(i, 0, 0, diff_t[i]);
      gas -> GetElectronLongitudinalDiffusion(i, 0, 0, diff_l[i]);
      gas -> GetElectronTownsend(i, 0, 0, town_a[i]);
      gas -> GetElectronAttachment(i, 0, 0, attach[i]);

      ef[i]= efields[i];
      driftv[i]=driftv[i]*1000;
      diff_t[i]=diff_t[i]*10000;
      diff_l[i]=diff_l[i]*10000;
      
      if(town_a[i]<0) town_a[i]=0;
      if(attach[i]<0) attach[i]=0;

    }

    /* plotting drift velocity */
    if(strcmp(opt, "dv") == 0){
      TCanvas* c1 = new TCanvas("c1", "drift velocity",21,28,500,527);    
      TGraph* g = new TGraph(efields.size(), ef, driftv);
      g -> GetYaxis()->SetTitle("drift velocity (cm/#mus)");
      g -> SetLineWidth(2);
      g -> SetMarkerStyle(21);
      g -> SetMarkerColor(kRed);
      g -> SetMarkerSize(1);
      g -> SetTitle("");
      g -> Draw("AP");
      c1 -> SetGrid();
      g -> SetTitle(gtitle);
      g -> GetHistogram() -> SetTitleOffset(0.5);
      g -> GetXaxis()->SetTitle("Electric field (V/cm)");
      g -> GetXaxis()->SetTitleOffset(1.3);
      g -> GetYaxis()->SetTitleOffset(1.3);
    }

    /* plotting transverse diffusion */
    else if(strcmp(opt, "dt") == 0){
      TCanvas* c1 = new TCanvas("c1", "transverse diffusion",21,28,500,527);    
      TGraph* g = new TGraph(efields.size(), ef, diff_t);
      g -> GetYaxis()->SetTitle("diffusion coefficient (#sqrt{cm})");
      g -> SetLineWidth(2);
      g -> SetMarkerStyle(21);
      g -> SetMarkerColor(kRed);
      g -> SetMarkerSize(1);
      g -> SetTitle("");
      g -> Draw("AP");
      c1 -> SetGrid();
      g -> SetTitle(gtitle);
      g -> GetHistogram() -> SetTitleOffset(0.5);
      g -> GetXaxis()->SetTitle("Electric field (V/cm)");
      g -> GetXaxis()->SetTitleOffset(1.3);
      g -> GetYaxis()->SetTitleOffset(1.4);
    }

    /* plotting longitudinal diffusion */
    else if(strcmp(opt, "dl") == 0){
      TCanvas* c1 = new TCanvas("c1", "longitudinal diffusion",21,28,500,527);    
      TGraph* g = new TGraph(efields.size(), ef, diff_l);
      g -> GetYaxis()->SetTitle("diffusion coefficient (#sqrt{cm})");
      g -> SetLineWidth(2);
      g -> SetMarkerStyle(21);
      g -> SetMarkerColor(kRed);
      g -> SetMarkerSize(1);
      g -> SetTitle("");
      g -> Draw("AP");
      c1 -> SetGrid();
      g -> SetTitle(gtitle);
      g -> GetHistogram() -> SetTitleOffset(0.5);
      g -> GetXaxis()->SetTitle("Electric field (V/cm)");
      g -> GetXaxis()->SetTitleOffset(1.3);
      g -> GetYaxis()->SetTitleOffset(1.4);
    }

    /* plotting townsent alpha */
    else if(strcmp(opt, "town") == 0){
      TCanvas* c1 = new TCanvas("c1", "Townsent coefficient",21,28,500,527);    
      TGraph* g = new TGraph(efields.size(), ef, town_a);
      g -> GetYaxis()->SetTitle("Townsent (1/cm)");
      g -> SetLineWidth(2);
      g -> SetMarkerStyle(21);
      g -> SetMarkerColor(kRed);
      g -> SetMarkerSize(1);
      g -> SetTitle("");
      g -> Draw("AP");
      c1 -> SetGrid();
      g -> SetTitle(gtitle);
      g -> GetHistogram() -> SetTitleOffset(0.5);
      g -> GetXaxis()->SetTitle("Electric field (V/cm)");
      g -> GetXaxis()->SetTitleOffset(1.3);
      g -> GetYaxis()->SetTitleOffset(1.4);
    }

    /* plotting attachment coeficient*/
    else if(strcmp(opt, "attach") == 0){
      TCanvas* c1 = new TCanvas("c1", "attachment coefficient",21,28,500,527);    
      TGraph* g = new TGraph(efields.size(), ef, attach);
      g -> GetYaxis()->SetTitle("attachment (1/cm)");
      g -> SetLineWidth(2);
      g -> SetMarkerStyle(21);
      g -> SetMarkerColor(kRed);
      g -> SetMarkerSize(1);
      g -> SetTitle("");
      g -> Draw("AP");
      c1 -> SetGrid();
      g -> SetTitle(gtitle);
      g -> GetHistogram() -> SetTitleOffset(0.5);
      g -> GetXaxis()->SetTitle("Electric field (V/cm)");
      g -> GetXaxis()->SetTitleOffset(1.3);
      g -> GetYaxis()->SetTitleOffset(1.4);
    }
    
    else{
      cout << "" << endl;
      cout << "   error: opt = dv | dt | dl | town | attach " << endl;
      cout << "" << endl;
      return 0;
    }

    app.Run();
    return 0;
  }
}

