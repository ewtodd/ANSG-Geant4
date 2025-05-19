#include "construction.hh"
#include "G4UnitsTable.hh"

DetectorConstruction::DetectorConstruction() {}

DetectorConstruction::~DetectorConstruction() {}

G4VPhysicalVolume *DetectorConstruction::Construct() {
  G4NistManager *nist = G4NistManager::Instance();
  G4Material *worldMat = nist->FindOrBuildMaterial("G4_AIR");

  // Create world volume
  G4Box *solidWorld = new G4Box("solidWorld", 0.3 * m, 0.3 * m, 0.3 * m);
  G4LogicalVolume *logicWorld =
      new G4LogicalVolume(solidWorld, worldMat, "logicWorld");
  G4VPhysicalVolume *physWorld = new G4PVPlacement(
      0, G4ThreeVector(0., 0., 0.), logicWorld, "physWorld", 0, false, 0, true);

  // Define moderator material
  G4Material *moderatorMaterial = nist->FindOrBuildMaterial("G4_POLYETHYLENE");
  G4double modBoxHalfX = 15 * cm;
  G4double modBoxHalfY = 15 * cm;
  G4double modBoxHalfZ = (8.0 / 2) * cm;

  G4double offset = 1 * cm;

  // Create the moderator box
  G4Box *modBox = new G4Box("ModBox", modBoxHalfX, modBoxHalfY, modBoxHalfZ);
  G4LogicalVolume *logicModBox =
      new G4LogicalVolume(modBox, moderatorMaterial, "ModBox");
  // Create and set visual attributes for the moderator box
  G4VisAttributes *moderatorVisAttr =
      new G4VisAttributes(G4Colour(0.0, 1.0, 0.0)); // Green
  logicModBox->SetVisAttributes(moderatorVisAttr);
  // Place the moderator box in the world volume
  G4VPhysicalVolume *physModBox =
      new G4PVPlacement(0, G4ThreeVector(0., 0., modBoxHalfZ + offset),
                        logicModBox, "ModBox", logicWorld, false, 0, true);

  G4Material *LiF = nist->FindOrBuildMaterial("G4_Li");

  G4Box *solidLiF = new G4Box("LiF", 0.5 * cm, 0.5 * cm, 0.5 * cm);
  G4LogicalVolume *logicLiF = new G4LogicalVolume(solidLiF, LiF, "LiF");

  G4Element *La = nist->FindOrBuildElement("La");
  G4Element *Br = nist->FindOrBuildElement("Br");
  G4Material *LaBr3 = new G4Material("LaBr3", 5.29 * g / cm3, 2);
  LaBr3->AddElement(La, 1);
  LaBr3->AddElement(Br, 3);
  G4double inch = 2.54 * cm;
  G4Tubs *solidLaBr3 =
      new G4Tubs("LaBr3", 0, 1.5 * inch / 2, 1.5 * inch / 2, 0, 360 * deg);

  fScoringVolume = new G4LogicalVolume(solidLaBr3, LaBr3, "LaBr3");
  G4double halfcm = 0.5 * cm;
  G4VPhysicalVolume *physLiF = new G4PVPlacement(
      0,
      G4ThreeVector(0, 0,
                    modBoxHalfZ*2 + offset+ (1.5 * inch)/2 + 0.01*cm), // Relative to AirShell
      logicLiF, "LiF",
      logicWorld, // Mother volume
      false, 0, true);

  G4VPhysicalVolume *physLaBr3 = new G4PVPlacement(
      0,
      G4ThreeVector(0, (1.5 * inch) / 2 + halfcm + inch,
                    modBoxHalfZ*2 + offset + (1.5 * inch)/2 + 0.01*cm),
      fScoringVolume, "LaBr3", logicWorld, false, 0, true);
  // Visualization attributes
  G4VisAttributes *liFVis =
      new G4VisAttributes(G4Colour(0.8, 0.8, 0.0)); // Yellow
  logicLiF->SetVisAttributes(liFVis);

  return physWorld;
}

void DetectorConstruction::ConstructSDandField() {
  // LaBr3 sensitive detector
  SensitiveDetector *SD = new SensitiveDetector("LaBr3");
  fScoringVolume->SetSensitiveDetector(SD);
}
