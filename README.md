# flaggsim

# The program code is tentatively available to the public.
# We will organize it as soon as possible, hopefully within a month.

This directory contains a sample code for the paper:

Interactive Optimization for Cartographic Aggregation of Building Features
by Shigero Takahashi, Ryo Kokubun, Satoshi Nishimura, Kazuo Misue, and Masatoshi Arikawa.

https://doi.org/10.1111/cgf.15090
https://diglib.eg.org/handle/10.1111/cgf15090

@article{10.1111:cgf.15090,
journal = {Computer Graphics Forum},
title = {{Interactive Optimization for Cartographic Aggregation of Building Features}},
author = {Takahashi, Shigeo and Kokubun, Ryo and Nishimura, Satoshi and Misue, Kazuo and Arikawa, Masatoshi},
year = {2024},
publisher = {The Eurographics Association and John Wiley & Sons Ltd.},
ISSN = {1467-8659},
DOI = {10.1111/cgf.15090}
}


The program code is written in C++, with GLUT for map drawing, FLTK for user interface, and CGAL for geometric computation. We revised the source code for label cost optimization (gco-v3.0 library) provided by Delong et al. in this implementation. See https://vision.cs.uwaterloo.ca/code/

The source codes are readily complied at the MacOS platform.

Before compiling the program codes, install the necessary libraries using homebrew, as
> brew install cgal fltk opencv gl2ps flann

Then compile the program
> touch Make-depend (Just create an empty file)
> make depend
> make

and launch the program
> ./flaggsim

In the control panel, press "Load" and open "akehara_008.dat" for example.
Then press "Aggregate" button to obtain a set of possible aggregation patterns.
Please also refer to the demonstration video available at 
https://diglib.eg.org/handle/10.1111/cgf15090
for the details about how to use this program.
