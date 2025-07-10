import ROOT
import os
import sys

def combine_root_files(output_file, input_files):
    # Create a TChain for each tree type
    LaBr3_chain = ROOT.TChain("LaBr3")
    CeBr3_chain = ROOT.TChain("CeBr3")

    # Add files to the chains
    for file_name in input_files:
        LaBr3_chain.Add(file_name)
        CeBr3_chain.Add(file_name)
    # Create the output file
    output = ROOT.TFile(output_file, 'RECREATE')

    # Merge the chains into the output file
    output.cd()
    LaBr3_tree = LaBr3_chain.CloneTree(-1, "fast")
    CeBr3_tree = CeBr3_chain.CloneTree(-1, "fast")
    # Write the trees to the output file
    LaBr3_tree.Write()
    CeBr3_tree.Write()
    # Close the output file
    output.Close()
    print(f"Combined {len(input_files)} ROOT files into {output_file}")

# List of input ROOT files (one for each thread)
input_files = [f'build/output0_t{i}.root' for i in range(32)]

# Output ROOT file
if (len(sys.argv) != 1):
    output_file = sys.argv[1] + '.root'
else: output_file = 'combined_output.root'

# Combine the files
combine_root_files(output_file, input_files)
