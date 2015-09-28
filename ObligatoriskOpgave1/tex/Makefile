OBJS = main.tex chapters/1-indledning.tex chapters/2-implementation.tex \
	chapters/3-test.tex chapters/4-konklusion.tex

main.pdf: $(OBJS)
	pdflatex main.tex
	pdflatex main.tex

clean:
	rm *.aux *.log main.pdf
	rm chapters/*.aux
