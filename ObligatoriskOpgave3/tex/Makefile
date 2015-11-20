OBJS = main.tex chapters/1-indledning.tex chapters/2-implementation.tex \
	chapters/3-test.tex chapters/4-diskussion.tex \
	chapters/5-konklusion.tex chapters/6-sourcecode.tex

main.pdf: $(OBJS)
	pdflatex main.tex
	pdflatex main.tex

clean:
	rm -f *.aux *.log main.pdf *.out *.toc *.synctex.gz
	rm -f chapters/*.aux chapters/*.log
