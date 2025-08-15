#include "generator.hh"
#include <G4AutoLock.hh>
#include <G4Event.hh>
#include <G4Neutron.hh>
#include <G4SystemOfUnits.hh>
#include <TFile.h>
#include <TH1D.h>
#include <TRandom3.h>
#include <TTree.h>
#include <mutex>
#include <unistd.h>

PrimaryGenerator::PrimaryGenerator()
    : G4VUserPrimaryGeneratorAction(), fParticleGun(nullptr) {
  G4int numParticles = 1;
  fParticleGun = new G4ParticleGun(numParticles);
  fParticleGun->SetParticleDefinition(G4Neutron::Neutron());
}

PrimaryGenerator::~PrimaryGenerator() { delete fParticleGun; }

void PrimaryGenerator::GeneratePrimaries(G4Event *anEvent) {

  // Set the particle's position from the data
  fParticleGun->SetParticlePosition(G4ThreeVector(0, 0, 0));

  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0, 0, 1));

  // Sample energy from the histogram
  double energy = 0.025 * eV; // Default energy in case histogram not available

  fParticleGun->SetParticleEnergy(energy);

  fParticleGun->GeneratePrimaryVertex(anEvent);
}
