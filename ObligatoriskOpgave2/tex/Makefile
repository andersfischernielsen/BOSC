OBJS = main.tex chapters/1-indledning.tex chapters/2-implementation.tex \
	chapters/3-test.tex chapters/4-konklusion.tex

main.pdf: $(OBJS)
	pdflatex main.tex
	pdflatex main.tex

clean:
	rm -f *.aux *.log main.pdf *.out *.toc
	rm -f chapters/*.aux
