#!/bin/bash

# Geometry scan parameters
MIN_THICKNESS=1.0  # mm
MAX_THICKNESS=20.0 # mm
STEP_THICKNESS=1.0

MIN_LENGTH=5.0   # mm
MAX_LENGTH=50.0  # mm
STEP_LENGTH=5.0

# Create results directory
mkdir -p results

# Create summary file
echo "Thickness(mm) Length(mm) Produced Emitted Efficiency" > results/summary.txt

# Loop over geometries
for thickness in $(seq $MIN_THICKNESS $STEP_THICKNESS $MAX_THICKNESS); do
    for length in $(seq $MIN_LENGTH $STEP_LENGTH $MAX_LENGTH); do
        # Update geometry in construction file
        sed -i "s/G4double thick = .*/G4double thick = ${thickness}\/2.0 * mm;/" construction.cc
        sed -i "s/G4double len_wid = .*/G4double len_wid = ${length}\/2.0 * mm;/" construction.cc
        
        # Compile and run simulation
        cd build
        make
        ./sim run.mac
        
 
        cd ..
        python combineoutputs.py output0
        # Analyze results
        root -l -q "analyze_results.C(\"output0.root\")" > temp.txt
        
        # Extract results and append to summary
        produced=$(grep "produced:" temp.txt | awk '{print $NF}')
        emitted=$(grep "emitted:" temp.txt | awk '{print $NF}')
        efficiency=$(grep "efficiency:" temp.txt | awk '{print $NF}')
        
        echo "$thickness $length $produced $emitted $efficiency" >> results/summary.txt
        
        # Move output file to results directory with meaningful name
        mv output0.root "results/thickness${thickness}_length${length}.root"
    done
done

# Create visualization plot
root -l <<EOF
{
    TGraph2D *g = new TGraph2D("results/summary.txt");
    g->SetTitle("Emission Efficiency;Thickness (mm);Length (mm);Efficiency");
    g->Draw("colz");
    c1->SaveAs("results/efficiency_map.pdf");
}
EOF
