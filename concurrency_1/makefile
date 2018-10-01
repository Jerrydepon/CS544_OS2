latexfile = main

init:pdf

pdf:
	latex ${latexfile}.tex
	dvips ${latexfile}.dvi
	ps2pdf ${latexfile}.ps
	rm -f ${latexfile}.aux ${latexfile}.dvi ${latexfile}.log ${latexfile}.ps
	gcc -o mt19937ar.o -c mt19937ar.c
	gcc -g concurrency_1.c mt19937ar.o -o concurrency_1 -lpthread
