#ifndef CONSTRUCTION_HH
#define CONSTRUCTION_HH

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"
#include "G4SubtractionSolid.hh"
#include "G4SystemOfUnits.hh"
#include "G4Tubs.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4VisAttributes.hh"
#include "cmath"
#include "detector.hh"

class DetectorConstruction : public G4VUserDetectorConstruction {
public:
  DetectorConstruction();
  ~DetectorConstruction();
  G4LogicalVolume *GetScoringVolumeGe() const { return fScoringVolumeGe; };
  G4LogicalVolume *GetScoringVolumeCdTe() const { return fScoringVolumeCdTe; };
  G4LogicalVolume *GetScoringVolumeNaI() const { return fScoringVolumeNaI; };

  virtual G4VPhysicalVolume *Construct();

private:
  G4LogicalVolume *fScoringVolumeGe;
  G4LogicalVolume *fScoringVolumeCdTe;
  G4LogicalVolume *fScoringVolumeNaI;
  virtual void ConstructSDandField();
};

#endif
