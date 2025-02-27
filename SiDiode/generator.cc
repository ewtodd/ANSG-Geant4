#include "generator.hh"

PrimaryGenerator::PrimaryGenerator() {
  fParticleGun = new G4ParticleGun(1);
  G4ParticleTable *particleTable = G4ParticleTable::GetParticleTable();
  G4String particleName = "alpha";
  G4ParticleDefinition *particle = particleTable->FindParticle(particleName);
  fParticleGun->SetParticleDefinition(particle);
  fParticleGun->SetParticlePosition(G4ThreeVector(0, 0, 0));
}

PrimaryGenerator::~PrimaryGenerator() { delete fParticleGun; }

void PrimaryGenerator::GeneratePrimaries(G4Event *anEvent) {

  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0, 0, 1));

  G4double mean_energy = 5486 * keV;
  G4double resolution = 16.0 / 5486;
  G4double sigma = mean_energy * resolution;

  // Sample from Gaussian distribution
  G4double energy;
  do {
    energy = G4RandGauss::shoot(mean_energy, sigma);
  } while (energy <= 0); // Ensure positive energy

  fParticleGun->SetParticleEnergy(energy);

  fParticleGun->GeneratePrimaryVertex(anEvent);
}
