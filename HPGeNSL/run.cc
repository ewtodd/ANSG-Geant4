#include "run.hh"

RunAction::RunAction() {
  G4AnalysisManager *man = G4AnalysisManager::Instance();

  man->CreateNtuple("LaBr3", "LaBr3");
  man->CreateNtupleDColumn("fEDep");
  man->CreateNtupleDColumn("fTime");
  man->FinishNtuple(0);

  man->CreateNtuple("CeBr3", "CeBr3");
  man->CreateNtupleDColumn("fEDep");
  man->CreateNtupleDColumn("fTime");
  man->FinishNtuple(1);
}
RunAction::~RunAction() {}
void RunAction::BeginOfRunAction(const G4Run *run) {
  G4AnalysisManager *man = G4AnalysisManager::Instance();
  G4int runNumber = run->GetRunID();
  std::stringstream strRunID;
  strRunID << runNumber;
  man->OpenFile("output" + strRunID.str() + ".root");
}
void RunAction::EndOfRunAction(const G4Run *) {
  G4AnalysisManager *man = G4AnalysisManager::Instance();

  man->Write();
  man->CloseFile("output.root");
}
