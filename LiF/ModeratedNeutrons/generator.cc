#include "generator.hh"
#include "G4Positron.hh"

PrimaryGenerator::PrimaryGenerator() {
  G4int numParticles = 1;
  fParticleGun = new G4ParticleGun(numParticles);
  fParticleGun->SetParticleDefinition(G4Positron::Definition());
}

PrimaryGenerator::~PrimaryGenerator() { delete fParticleGun; }

G4double PrimaryGenerator::SampleCf252Spectrum() {
  // Watt spectrum parameters for Cf-252
  const G4double a = 1.025; // MeV
  const G4double b = 2.926; // MeV^-1
  const G4double maxP = 1.04;

  G4double E, P;
  do {
    E = 15.0 * MeV * G4UniformRand(); // Reduced upper limit
    P = std::exp(-E / a) * std::sinh(std::sqrt(b * E));
  } while (G4UniformRand() > P / maxP); // Better efficiency

  return E;
}

void PrimaryGenerator::GeneratePrimaries(G4Event *anEvent) {
  G4double lifZPos = 12.63 * cm; // LiF's Z-position from your geometry
  // fParticleGun->SetParticlePosition(G4ThreeVector(0, 0, lifZPos));
  // fParticleGun->SetParticleMomentumDirection(G4RandomDirection());
  //  Sample energy from Cf-252 spectrum
  // G4double energy = SampleCf252Spectrum();
  // fParticleGun->SetParticleEnergy(20 * eV);

  // fParticleGun->GeneratePrimaryVertex(anEvent);

  // 1) Gamma #1
  fParticleGun->SetParticleDefinition(G4Gamma::Definition());
  fParticleGun->SetParticleEnergy(511 * keV);
  fParticleGun->SetParticlePosition(G4ThreeVector(0., 0., lifZPos));
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0., 1., 0.)); // +x
  fParticleGun->GeneratePrimaryVertex(anEvent);

  // 2) Gamma #2
  fParticleGun->SetParticleEnergy(511 * keV);
  // same position or very close:
  fParticleGun->SetParticlePosition(G4ThreeVector(0., 0., lifZPos));
  // opposite direction:
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0., -1., 0.)); // –x
  fParticleGun->GeneratePrimaryVertex(anEvent);
}
