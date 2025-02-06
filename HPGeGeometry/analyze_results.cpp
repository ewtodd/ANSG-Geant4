#include <TFile.h>
#include <TTree.h>
#include <iostream>

// analyze_results.C
void analyze_results(const char *filename) {
  TFile *f = new TFile(filename);
  TTree *counts = (TTree *)f->Get("Counts");

  Int_t produced, emitted;
  counts->SetBranchAddress("Produced", &produced);
  counts->SetBranchAddress("Emitted", &emitted);

  Long64_t entries = counts->GetEntries();
  Int_t total_produced = 0;
  Int_t total_emitted = 0;

  // Sum up all the 10s (each represents one gamma)
  for (Long64_t i = 0; i < entries; i++) {
    counts->GetEntry(i);
    if (produced == 10)
      total_produced++;
    if (emitted == 10)
      total_emitted++;
  }

  Double_t emission_efficiency = (Double_t)total_emitted / total_produced;

  std::cout << "Results for " << filename << std::endl;
  std::cout << "Total 68.75 keV gammas produced: " << total_produced
            << std::endl;
  std::cout << "Total 68.75 keV gammas emitted: " << total_emitted << std::endl;
  std::cout << "Emission efficiency: " << emission_efficiency << std::endl;

  f->Close();
}
