
# The UIBK beamer theme provides support for pdflatex, xetex, and lualatex.
# By default pdflatex is used in this Makefile but can be changed if needed.
# Additional -output-driver argument for xelatex can be used to enforce
# PDF version 1.4. Important if you would like to print the poster (Studia/print shop),
# newer versions can cause problems. LuaTeX users should check the notes in
# `poster.tex`. To check PDF version use 'pdfinfo <pdffile>'.
#; TEX=lualatex
#; TEX=xelatex
#; TEX="xelatex -output-driver='xdvipdfmx -V4'"
TEX=pdflatex

all: letter.pdf slides.pdf poster.pdf clean

uibklatex.zip: README.pdf letter.pdf slides.pdf poster.pdf slides-revealjs.html clean
	zip -r uibklatex.zip uibklttr.cls beamerthemeuibk.sty beamerthemeuibkposter.sty README.pdf LICENSE _images letter.tex letter.pdf slides.tex slides.pdf poster.tex poster.pdf slides-revealjs.qmd slides-revealjs.html qrcode.pdf

README.pdf: README.md
	pandoc -o README.pdf README.md

letter.pdf: uibklttr.cls uibklttr.cfg letter.tex
	pdflatex letter.tex
	pdflatex letter.tex

slides.pdf: beamerthemeuibk.sty slides.tex
	$(TEX) slides.tex
	$(TEX) slides.tex

slides-beamer.pdf: beamerthemeuibk.sty slides-beamer.qmd
	quarto render slides-beamer.qmd

slides-revealjs.html: slides-revealjs.qmd
	# Note that the installation is only required once;
	# included here for testing/to make sure the extension
	# is installed before rendering.
	quarto install extension . --no-prompt
	# In addition, install the fontawesome extension
	# Note: If fontawesome is used in combination with latex beamer,
	# the `fontawesome*` font (texlive) is required. On some systems
	# texlive comes with an older version (e.g., fontawesome5). Try
	# to install an older version of the extension, e.g.:
	##   quarto install extension quarto-ext/fontawesome@v1.2.0 --no-prompt
	quarto install extension quarto-ext/fontawesome --no-prompt
	quarto render slides-revealjs.qmd

poster.pdf: beamerthemeuibk.sty beamerthemeuibkposter.sty poster.tex
	$(TEX) poster.tex
	$(TEX) poster.tex

clean:
	-rm -f *.aux *.log *.nav *.out *.snm *.toc *.vrb *.dvi
	-rm -f *.fdb_latexmk *.fls
	-rm -rf slides-*_files/
	-rm -f *~

distclean: clean
	rm -f letter.pdf slides.pdf poster.pdf slides-beamer.tex slides-beamer.pdf slides-revealjs.html README.pdf uibklatex.zip

# Run .gitlab-ci.yml; shell mode
gitlab:
	gitlab-runner exec shell pdf

