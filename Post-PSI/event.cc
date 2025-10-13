#include "event.hh"

EventAction::EventAction(RunAction *) { fEdepCdTe = 0.; }

EventAction::~EventAction() {}

void EventAction::BeginOfEventAction(const G4Event *) { fEdepCdTe = 0.; }

void EventAction::AddEdepCdTe(G4double edep) { fEdepCdTe += edep; }

void EventAction::EndOfEventAction(const G4Event *) {
  G4AnalysisManager *man = G4AnalysisManager::Instance();

  if (fEdepCdTe > 1e-7) {
    // Ntuple 1: CdTe
    man->FillNtupleDColumn(0, 0, fEdepCdTe / MeV);
    man->AddNtupleRow(0);
  }
}
