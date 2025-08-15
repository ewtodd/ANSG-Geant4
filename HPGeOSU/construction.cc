#include "construction.hh"

DetectorConstruction::DetectorConstruction() {}
DetectorConstruction::~DetectorConstruction() {}

G4VPhysicalVolume *DetectorConstruction::Construct() {
  G4NistManager *nist = G4NistManager::Instance();
  G4Material *worldMat = nist->FindOrBuildMaterial("G4_AIR");
  G4Material *Ge = nist->FindOrBuildMaterial("G4_Ge");

  // HPGe dimensions
  G4double radius = 8 * mm;
  G4double halfHeight = 5 * mm;

  // Position of Ge center along z
  G4double zPositionGe = radius + 3.0 * cm;

  // Calculate minimal world half dimensions with safety margins
  // After rotation, cylinder axis is along X, so max X extent is halfHeight
  G4double worldHalfX = halfHeight + 1 * cm;               // 1 cm margin
  G4double worldHalfY = radius + 1 * cm;                   // 1 cm margin
  G4double worldHalfZ = zPositionGe + halfHeight + 1 * cm; // 1 cm margin

  // Create minimal world volume
  G4Box *solidWorld =
      new G4Box("solidWorld", worldHalfX, worldHalfY, worldHalfZ);

  // Create Ge cylinder (default axis along Z)
  G4Tubs *solidGe =
      new G4Tubs("solidGe", 0., radius, halfHeight, 0 * deg, 360 * deg);

  // Get volume for output
  G4cout << "Volume of the Germanium target: "
         << solidGe->GetCubicVolume() / cm3 << " cm3." << G4endl;

// Get volume for output
  G4cout << "World volume: "
         << solidWorld->GetCubicVolume() / cm3 << " cm3." << G4endl;


  G4LogicalVolume *logicWorld =
      new G4LogicalVolume(solidWorld, worldMat, "logicWorld");
  G4VPhysicalVolume *physWorld = new G4PVPlacement(
      0, G4ThreeVector(0., 0., 0.), logicWorld, "physWorld", 0, false, 0, true);

  logicGe = new G4LogicalVolume(solidGe, Ge, "logicGe");
  G4VisAttributes *GeVisAttr = new G4VisAttributes(G4Colour(0.0, 0.0, 1.0));
  logicGe->SetVisAttributes(GeVisAttr);

  // Rotate cylinder so its side (curved surface) faces the beam (Z-axis)
  G4RotationMatrix *rotGe = new G4RotationMatrix();
  rotGe->rotateZ(90 * deg); // Rotates cylinder axis from Z to X
  rotGe->rotateY(90 * deg);
  // Place Ge with rotation and position
  G4VPhysicalVolume *physGe =
      new G4PVPlacement(rotGe, G4ThreeVector(0., 0., zPositionGe), logicGe,
                        "physGe", logicWorld, false, 0, true);

  // Germanium lattice information
  G4LatticeManager *LM = G4LatticeManager::GetLatticeManager();
  G4LatticeLogical *GeLogical = LM->LoadLattice(Ge, "Ge");
  G4LatticePhysical *GePhysical =
      new G4LatticePhysical(GeLogical, physGe->GetFrameRotation());
  LM->RegisterLattice(physGe, GePhysical);

  return physWorld;
}

void DetectorConstruction::ConstructSDandField() {
  SensitiveDetector *sensDet = new SensitiveDetector("Germanium");
  logicGe->SetSensitiveDetector(sensDet);
}
