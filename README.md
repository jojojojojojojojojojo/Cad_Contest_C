# 2017 CAD Contest Problem C - Multi-Deck Standard Cell Legalizatioin

Second Place

Shih-Wei Hsieh, Shao-Chun Hung – National Taiwan University

Advisor: Prof. Yao-Wen Chang

Usage:

1. tech_lef : specifies the tech.lef input file

2. cell_lef : specifies the cell.lef input file

3. input_def: specifies the input DEF file (that is a global placement solution)

4. output_def: specifies the output DEF file (that is a legal solution)

5. placement_constraints : specifies the placement.constraints file

An example for running our binary: 

cada041 -tech_lef tech.lef -cell_lef cell.lef -input_def placed.def -placement_constraints placement.constraints -output_def lg.def
