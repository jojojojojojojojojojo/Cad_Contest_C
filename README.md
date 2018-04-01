# 2017 CAD Contest Problem B - Multi-Deck Standard Cell Legalizatioin
Second Place
Shih-Wei Hsieh, Shao-Chun Hung–National Taiwan University
Advisor: Prof. Yao-Wen Chang

Usage:

tech_lef : specifies the tech.lef input file
cell_lef : specifies the cell.lef input file
input_def: specifies the input DEF file (that is a global placement solution)
output_def: specifies the output DEF file (that is a legal solution)
placement_constraints : specifies the placement.constraints file

An example for running our binary: 
cada041 -tech_lef tech.lef -cell_lef cell.lef -input_def placed.def -placement_constraints placement.constraints -output_def lg.def
