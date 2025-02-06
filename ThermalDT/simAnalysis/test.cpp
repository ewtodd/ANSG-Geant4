#include "PlotFunctions.cpp"
#include <TBranch.h>
#include <TCanvas.h>
#include <TFile.h>
#include <TGaxis.h>
#include <TGraph.h>
#include <TH1D.h>
#include <TLegend.h>
#include <TPaveText.h>
#include <TTree.h>
#include <iostream>
#include <sstream>
#include <vector>

void test() {
  gROOT->SetBatch(kTRUE);

  // Define thermal energy range in MeV (1 eV = 1e-6 MeV) for thermal counting
  Double_t thermalEnergyLower = 0.015 * 1e-6; // 0.01 eV in MeV
  Double_t thermalEnergyUpper = 0.030 * 1e-6; // 0.05 eV in MeV

  // Arrays to store the thicknesses and corresponding normalized thermal counts
  std::vector<double> thicknesses;
  std::vector<double> waterThermalCounts;
  std::vector<double> polyThermalCounts;

  // Variables for TTree branches
  Double_t postEnergy;
  Double_t postPosX, postPosY, postPosZ;
  Double_t postMomX, postMomY, postMomZ;

  // Function to process ROOT files and extract thermal counts and store
  // momentum and position
  auto processFiles = [&](const std::string &material,
                          std::vector<double> &thermalCounts) {
    for (int thickness = 5; thickness <= 20; thickness += 1) {
      // Create the filename
      std::ostringstream filename;
      filename << "../" << material << thickness << "cm.root";

      // Open the ROOT file
      TFile *file = TFile::Open(filename.str().c_str());

      if (!file || file->IsZombie()) {
        std::cerr << "Error opening file " << filename.str() << "!"
                  << std::endl;
        continue;
      }

      // Get the ntuple from the file
      TTree *tree = (TTree *)file->Get("PostEnergy");

      if (!tree) {
        std::cerr << "Error: PostEnergy tree not found in " << filename.str()
                  << "!" << std::endl;
        file->Close();
        delete file; // ensure file is deleted
        continue;
      }

      // Set branch addresses
      tree->SetBranchAddress("fPostEnergy", &postEnergy);

      Int_t nEntries = tree->GetEntries();
      Int_t thermalCount = 0;

      // Loop over tree entries to count thermal neutrons
      for (Int_t i = 0; i < nEntries; ++i) {
        tree->GetEntry(i);
        if (postEnergy >= thermalEnergyLower &&
            postEnergy <= thermalEnergyUpper) {
          ++thermalCount;
        }
      }

      // Normalize the thermal count
      double normalizedThermalCount = static_cast<double>(thermalCount) * 100;

      // Store the results
      if (material == "Water") {
        thicknesses.push_back(thickness);
      }
      thermalCounts.push_back(normalizedThermalCount);

      // Output results for current file
      std::cout << material << " File: " << filename.str() << std::endl;
      std::cout << "  Total entries: " << nEntries << std::endl;
      std::cout << "  Number of thermal neutrons: " << thermalCount
                << std::endl;
      std::cout << "  Normalized number of thermal neutrons: "
                << normalizedThermalCount << std::endl;

      // Clean up
      file->Close();
      delete file;
    }
  };

  // Process water files
  processFiles("Water", waterThermalCounts);

  // Process poly files
  processFiles("Poly", polyThermalCounts);

  // Create a canvas to plot the thermal neutrons vs. thickness
  TCanvas *c3 =
      new TCanvas("c3", "Thermal Neutrons vs. Moderator Thickness", 2000, 1600);
  // Create the TGraph for poly files
  TGraph *graphWater =
      new TGraph(thicknesses.size(), &thicknesses[0], &waterThermalCounts[0]);
  graphWater->SetTitle(
      ";Moderator Thickness (cm);Estimated Thermal Neutron Flux (n/s)");
  graphWater->SetMarkerStyle(20);
  graphWater->SetMarkerSize(2);
  graphWater->SetMarkerColor(kBlue);

  TGraph *graphPoly =
      new TGraph(thicknesses.size(), &thicknesses[0], &polyThermalCounts[0]);
  graphPoly->SetMarkerStyle(21);
  graphPoly->SetMarkerSize(2);
  graphPoly->SetMarkerColor(kRed);

  // Draw the graphs
  graphWater->Draw("AP");
  graphPoly->Draw("P SAME");

  // Find the min and max y-values for both graphs
  double minY = std::min(
      *std::min_element(waterThermalCounts.begin(), waterThermalCounts.end()),
      *std::min_element(polyThermalCounts.begin(), polyThermalCounts.end()));
  double maxY = std::max(
      *std::max_element(waterThermalCounts.begin(), waterThermalCounts.end()),
      *std::max_element(polyThermalCounts.begin(), polyThermalCounts.end()));

  if (minY <= 0) {
    minY = 1e-3; // Set minimum to a small positive value if zero/negative
  }

  graphWater->GetHistogram()->SetMinimum(minY * 0.9);
  graphWater->GetHistogram()->SetMaximum(maxY * 1.1);

  TPaveText *title = new TPaveText(0.1, 0.93, 0.9, 0.98, "NDC");
  title->AddText("Thermal Neutron Flux from Moderated DT Generator");
  title->SetFillColor(0);
  title->SetTextAlign(22);
  title->SetTextFont(42);
  title->Draw();

  TLegend *legend = new TLegend(0.72, 0.19, 0.83, 0.3);
  legend->AddEntry(graphWater, "Water", "p");
  legend->AddEntry(graphPoly, "HDP", "p");
  legend->SetTextSize(0.03);
  legend->Draw();
  SetCanvasMargins(c3);
  graphWater->GetYaxis()->SetTitleOffset(2);
  c3->SetTickx();
  c3->SetTicky();
  TAxis *axis = graphWater->GetYaxis();
  gPad->SetLogy();
  axis->SetMoreLogLabels();
  c3->SetGridx();
  c3->SetGridy();

  c3->SaveAs("ThermalNeutronsVsThickness.png");

  // Additional: Generate full and thermal energy spectrum plots for 11 cm Poly
  // and 15 cm Water
  std::vector<std::pair<std::string, int>> specialFiles = {{"Poly", 11},
                                                           {"Water", 15}};
  TH1D *fullEnergyHistPoly =
      new TH1D("FullEnergyHistPoly", "Full Energy Spectrum;Energy (MeV);Counts",
               250, 0, 15);
  TH1D *thermalEnergyHistPoly =
      new TH1D("ThermalEnergyHistPoly", ";Energy (eV);Normalized Counts", 75, 0,
               0.1); // Adjusted bin width to 0.1 eV
  TH1D *fullEnergyHistWater =
      new TH1D("FullEnergyHistWater",
               "Full Energy Spectrum;Energy (MeV);Counts", 250, 0, 15);
  TH1D *thermalEnergyHistWater =
      new TH1D("ThermalEnergyHistWater", ";Energy (eV);Normalized Counts", 75,
               0, 0.1); // Adjusted bin width to 0.1 eV

  TFile *outputFileWater15cm = TFile::Open("Water15cm1E7.root", "RECREATE");
  TTree *outTree = new TTree("PostEnergy", "PostEnergy");
  outTree->Branch("fPostEnergy", &postEnergy, "fPostEnergy/D");
  outTree->Branch("fPostPosX", &postPosX, "fPostPosX/D");
  outTree->Branch("fPostPosY", &postPosY, "fPostPosY/D");
  outTree->Branch("fPostPosZ", &postPosZ, "fPostPosZ/D");
  outTree->Branch("fPostMomX", &postMomX, "fPostMomX/D");
  outTree->Branch("fPostMomY", &postMomY, "fPostMomY/D");
  outTree->Branch("fPostMomZ", &postMomZ, "fPostMomZ/D");

  for (const auto &file : specialFiles) {
    std::ostringstream filename;
    filename << "../" << file.first << file.second << "cm.root";
    TFile *specFile = TFile::Open(filename.str().c_str());

    if (!specFile || specFile->IsZombie()) {
      std::cerr << "Error opening file " << filename.str() << "!" << std::endl;
      if (specFile)
        delete specFile;
      continue;
    }

    TTree *tree = (TTree *)specFile->Get("PostEnergy");

    if (!tree) {
      std::cerr << "Error: PostEnergy tree not found in " << filename.str()
                << "!" << std::endl;
      specFile->Close();
      delete specFile;
      continue;
    }

    tree->SetBranchAddress("fPostEnergy", &postEnergy);
    tree->SetBranchAddress("fPostPosX", &postPosX);
    tree->SetBranchAddress("fPostPosY", &postPosY);
    tree->SetBranchAddress("fPostPosZ", &postPosZ);
    tree->SetBranchAddress("fPostMomX", &postMomX);
    tree->SetBranchAddress("fPostMomY", &postMomY);
    tree->SetBranchAddress("fPostMomZ", &postMomZ);
    Int_t nEntries = tree->GetEntries();

    for (Int_t i = 0; i < nEntries; ++i) {
      tree->GetEntry(i);
      if (postEnergy >= 0 && postEnergy <= 0.1 * 1e-6) {
        thermalEnergyHistPoly->Fill(postEnergy * 1e6);
        if (file.first == "Water" && file.second == 15) {
          outTree->Fill();
        }
      }
      if (postEnergy >= 0 && postEnergy <= 15) {
        fullEnergyHistPoly->Fill(postEnergy);
      }
    }

    specFile->Close();
    delete specFile;
  }

  if (thermalEnergyHistPoly->Integral() > 0) {
    thermalEnergyHistPoly->Scale(1.0 / thermalEnergyHistPoly->Integral());
  }

  outputFileWater15cm->cd();
  outTree->Write();
  outputFileWater15cm->Close();
  delete outputFileWater15cm;

  TCanvas *fullEnergyCanvas =
      new TCanvas("FullEnergyCanvas", "Full Energy Spectrum", 1600, 1200);
  fullEnergyHistPoly->SetLineColor(kRed);
  fullEnergyHistPoly->SetStats(0);
  fullEnergyHistWater->SetLineColor(kBlue);
  fullEnergyHistWater->SetStats(0);
  fullEnergyHistWater->Draw();
  fullEnergyHistPoly->Draw("SAME");
  TLegend *fullEnergyLegend = new TLegend(0.7, 0.7, 0.9, 0.9);
  fullEnergyLegend->AddEntry(fullEnergyHistWater, "Water 15 cm", "l");
  fullEnergyLegend->AddEntry(fullEnergyHistPoly, "Poly 11 cm", "l");
  fullEnergyLegend->Draw();
  fullEnergyCanvas->SaveAs("FullEnergySpectrum.png");

  TCanvas *thermalEnergyCanvas =
      new TCanvas("ThermalEnergyCanvas", "Thermal Energy Spectrum", 1600, 1200);
  thermalEnergyHistPoly->SetLineColor(kRed);
  thermalEnergyHistPoly->SetStats(0);
  thermalEnergyHistWater->SetLineColor(kBlue);
  thermalEnergyHistWater->SetStats(0);
  thermalEnergyHistWater->Draw("HIST");
  thermalEnergyHistPoly->Draw("HIST SAME");
  TLegend *thermalEnergyLegend = new TLegend(0.7, 0.7, 0.9, 0.9);
  thermalEnergyLegend->AddEntry(thermalEnergyHistWater, "Water, 15 cm", "l");
  thermalEnergyLegend->AddEntry(thermalEnergyHistPoly, "HDP, 11 cm", "l");
  thermalEnergyLegend->Draw();
  TPaveText *titleThermal = new TPaveText(0.1, 0.93, 0.9, 0.98, "NDC");
  titleThermal->AddText("Moderated DT Generator Thermal Neutron Spectrum");
  titleThermal->SetFillColor(0);
  titleThermal->SetTextAlign(22);
  titleThermal->SetTextFont(42);
  titleThermal->Draw();
  thermalEnergyCanvas->SetTickx();
  thermalEnergyCanvas->SetTicky();
  thermalEnergyCanvas->SetGridx();
  thermalEnergyCanvas->SetGridy();
  SetCanvasMargins(thermalEnergyCanvas);
  thermalEnergyCanvas->SaveAs("ThermalEnergySpectrum.png");

  delete fullEnergyHistPoly;
  delete thermalEnergyHistPoly;
  delete fullEnergyHistWater;
  delete thermalEnergyHistWater;
  delete fullEnergyLegend;
  delete thermalEnergyLegend;
  delete fullEnergyCanvas;
  delete thermalEnergyCanvas;
  delete graphWater;
  delete graphPoly;
  delete legend;
}
