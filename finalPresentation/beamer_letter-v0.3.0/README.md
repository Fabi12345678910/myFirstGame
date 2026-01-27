# UIBK Corporate Design: LaTeX Style Files

## Table of contents

- [Overview](#overview)
- [Beamer slides](#beamer-slides)
- [Quarto slides](#quarto-slides)
- [Posters](#posters)
- [Letters](#letters)


## Overview

### _Description_

* _Summary:_ LaTeX style files and templates for the [corporate design](https://www.uibk.ac.at/public-relations/intranet/)
  of Universität Innsbruck, with feedback from the BfÖ, providing `beamer`-based slides and posters as well
  as KOMA-script `scrlttr2`-based letters. Quarto Markdown templates and extentions of PDF and HTML slides are also provided.
* _Authors:_ Justus Piater, Reto Stauffer (`Reto.Stauffer@uibk.ac.at`), Christian Sternagel, Achim Zeileis (`Achim.Zeileis@uibk.ac.at`).
* _Copyright:_ Justus Piater, Reto Stauffer, Christian Sternagel, Achim Zeileis (style files), Universität Innsbruck (logos/images).
* _License:_ GPL-2 | GPL-3
* _URL:_ [https://git.uibk.ac.at/uibklatex/beamer_letter/](https://git.uibk.ac.at/uibklatex/beamer_letter/)
* _Infrastructure:_ LaTeX (`pdflatex`, `xelatex`, `lualatex`), beamer (for PDF slides and posters),
  KOMA-script (for letters), Quarto/Pandoc (for PDF or HTML slides from Markdown).


### _Files_

* `beamerthemeuibk.sty`: Main beamer theme style file.
* `beamerthemeuibkposter.sty`: Additional styles for posters (based on `beamerthemeuibk.sty`).
* `uibklttr.cls`: LaTeX letter class.
* `uibklttr.cfg`: Personal configuration file for letters.
* `letter.tex`: Example LaTeX file for letters.
* `slides.tex`: Example LaTeX file for presentation slides (16:9 and 4:3 option available).
* `poster.tex`: Example LaTeX file for conference posters (portrait and landscape option available). 
* `qrcode.pdf`: Example of a QR code for a poster with more information or poster download.
*  `_/images/` contains the images used by the `beamerthemeuibk`, `beamerthemeuibkposter`, and `uibklttr`. Namely:
   - `_/images/uibk_logo_4c_cmyk.pdf`: UIBK logo.
   - `_/images/uibk_header1.png`: UIBK header image 1 for slides
   - `_/images/uibk_header2.png`: UIBK header image 2 for slides
   - `_/images/uibk_header3.png`: UIBK header image 3 for slides
   - `_/images/uibk_header4.png`: UIBK header image 4 for slides 
   - `_/images/uibk_header5.png`: UIBK header image 5 for slides 
   - `_/images/uibk_header6.png`: UIBK header image 6 for slides 
   - `_/images/uibk_header1p.png`: UIBK header image 1 for posters 
   - `_/images/uibk_header2p.png`: UIBK header image 2 for posters
   - `_/images/uibk_header3p.png`: UIBK header image 3 for posters
   - `_/images/uibk_header4p.png`: UIBK header image 4 for posters
   - `_/images/uibk_header5p.png`: UIBK header image 5 for posters
   - `_/images/uibk_header6p.png`: UIBK header image 6 for posters
   - `_/images/license_ccby.pdf`: CC-BY license logo.
* `slides-beamer.qmd`: Example Quarto file for PDF slides (based on `beamerthemeuibk.sty`).
* `slides-revealjs.qmd`: Example Quarto file for HTML slides (based on `_extensions/uibk/`).
* `_extensions/uibk/`: Quarto reveal.js extension.


### _Installation_

* _All-inclusive (recommended):_ Copy all files/folders to your TEXMF tree, e.g., `texmf/tex/latex/uibklatex/`.
* _Letter-only:_ Copy the file `uibklttr.cls` (and optionally `uibklttr.cfg`) along with `uibk_logo_4c_cmyk.pdf` to your working directory.
* _Beamer-only:_ Copy the file `beamerthemeuibk.sty` and the entire folder `_images` to your local working directory.
* _Poster-only:_ Copy `beamerthemeuibkposter.sty` in addtion to the beamer-only files.
* _Templates:_ Start with the example LaTeX file (`letter.tex`, `slides.tex`, or `poster.tex`)
  or Quarto/Markdown file (`slides-beamer.qmd` or `slides-revealjs.qmd`),
  containing the sources for a demo along with many useful hints and comments in the source code.


### _Implementation details_

_Colors:_ The corporate design colors are pre-defined: `uibkblue` and `uibkorange`. Furthermore,
light versions of these colors (e.g., for backgrounds or shadings of images) are provided
(`uibkbluel` and `uibkorangel`). Moreover, three levels of gray are provided: `uibkgray`
(used for the regular text), a medium `uibkgraym` (e.g., used for frame titles), and a light
`uibkgrayl` (for backgrounds and shadings).

_Fonts:_ The corporate design employs the fonts Frutiger and Calibri which are not easily available
across systems in LaTeX. Therefore, for `pdflatex` the font "Carlito" from the LaTeX package `{carlito}`
is used as a font-metric compatible replacement for Calibri and the font "Bera Sans"
from the LaTeX package `{berasans}` is employed as a Frutiger descendant.
To be able to use the font on linux systems the `texlive-fonts-extra` package might have to be installed.
For `xelatex` and `lualatex` the DejaVu Sans font family is used via the `{fontspec}` package.

_KOMA-Script:_ The `uibklttr.cls` class depends on the KOMA-Script class
`scrlttr2.cls` (see also below), necessitating a recent version of KOMA-Script. Older LaTeX
distributions may ship with outdated versions, though, leading to an error during compilation
(error in: `\ifstrstart{#1}{plain.scrheadings.foot}`). If you encounter this problem please
update KOMA-Script on your system, e.g.:

* Visit [https://www.ctan.org/pkg/koma-script](https://www.ctan.org/pkg/koma-script).
* Download the latest `koma-script.tds.zip` and unzip the archive.
* Copy the folder `tex/latex/koma-script/` into your TEXMF tree, e.g., `~/texmf/tex/latex/koma-script/`.
* On Linux systems the berasans font (`bera.map`) has to be installed. If not please install the `texlive-fonts-extra` package.

_Quarto:_ The open-source scientific publishing system [Quarto](https://quarto.org/) is used for
rendering PDF or HTML slides from Markdown source files (possibly also containing code in Python, R, Julia, etc.)
For PDF slides the standard Quarto beamer output is used in combination with the UIBK beamer theme.
For HTML slides the standard Quarto reveal.js output is used in combination with a custom UIBK extension.


## Beamer slides

### _Features_

* Based on the general LaTeX class `beamer`, thus providing the corresponding usual formatting options.
* Aspect ratio: 16:9 or 4:3.
* Header images: One of four images from the corporate design.
* Colors: Predefined colors from the corporate design in different shadings (see above).
* Footer options: Formatting, page numbering, Creative Commons license.

### _Example_

16:9 slides with footer, university logo, and no total frame count.

    \documentclass[11pt,t,usepdftitle=false,aspectratio=169]{beamer}
    \usetheme[nototalframenumber,foot,logo]{uibk}
    \headerimage{3}

### _Verbatim commands_

The beamer theme provides a set of useful functions such as `\email`, `\doi`,
`\file` and `\dataset` (among others) which produce verbatim output. By default
they use the text color (dark gray) but could be changed manually, e.g., to `uibkblue`.

    \setbeamercolor{verbcolor}{fg=uibkblue}


## Quarto slides

### _Features_

* Based on Quarto Markdown which supports dynamic code chunks in Python, R, Julia, and Observable and which can be rendered into many output formats.
* Here, focus on PDF slides (via beamer) or HTML slides (via reveal.js).
* The template `slides-beamer.qmd` illustrates how to produce PDF slides from Quarto using the `uibk` beamer theme (see above).
* The template `slides-revealjs.qmd` illustrates how to produce HTML slides from Quarto using the custom `uibk-revealjs` extension.
* See the corresponding output files, `slides-beamer.pdf` and `slides-revealjs.html`, for further details and instructions.
* Note that `slides-beamer.qmd` (unlike `slides-revealjs.qmd`) does not employ custom Quarto extensions.
  This is possible, though, enabling more custom filters. For those who want to try it out, use `format: uibk-beamer` instead of `format: beamer`.


## Posters

### _Features_

* The poster theme is an extension of the UIBK `beamer` class.
* **IMPORTANT**: When using the poster theme to print/plot posters (e.g., for
    conferences) make sure the final PDF is of version 1.4; newer versions cause
    problems in combination with Adobe Acrobat (e.g., Studio, print shop).
    `poster.tex` forces `pdflatex` to use pdf minor version 1.4.  In case of using
    XeTeX or LuaTeX: check the comments in `Makefile` and/or `poster.tex`,
    solutions for both are included. Use `pdfinfo <pdffile>` to check the version
    used to create the PDF file.
* Allows for both, landscape and portrait posters.
* Size: A0 or A1.
* Color themes: Default theme (using `uibkblue` and `uibkgraym`) or
  alternatively `orangetheme` (using `uibkorange` for titles etc.).
* Flexible specification of the number of columns.

### _Example_

A0 landscape poster with 3 columns using the orange theme

    \documentclass[final]{beamer}
    \usepackage[orientation=landscape,size=a0,scale=1.30]{beamerposter}
    \usetheme[ncols=3,orangetheme]{uibkposter}
    \headerimage{3}


## Letters

### _Features_

* Based on the general LaTeX class `scrlttr2` (using KOMA-Script), thus providing the corresponding usual formatting options.
* Flexible configuration of header (faculty/department/name), footer (address), and reference lines (telephone/fax/etc.).
* Colors: Predefined colors from the corporate design in different shadings (see above).
* Footer options: Formatting, page numbering, Creative Commons license.

### _KOMA variables_

_Header:_

* `subtext`: Additional text (faculty) beneath the logo.
* `institute`: Additional text (department/name) "top right".

_Footer:_

* `fromaddress`: Address line.

_Reference line:_

* `fromname`: Name/title.
* `fromemail`: E-mail.
* `fromphone`: Telephone number.
* `fromfax`: Fax number.
* `refnum`: Reference number (Geschäftszahl).

_Formatting:_

* `backaddress`: Empty.
* `firstfoot`: Positioning `fromaddress`.
* `firsthead`: Positioning `fromlogo`, `subtext` and `institute`.
* `fromlogo`: UIBK logo.
* `invoice`: Positioning `date`.
* `yourref`: Positioning reference line (`fromname`/`fromemail`/...).

The personal configuration for letters (name, address, faculty, department, ...)
can be stored in `uibklttr.cfg`. This is then loaded automatically by `uibklttr`
(unless suppressed by the `noconfig` option as in the `letter.tex` template).

To omit the entire header (including the logo) for pre-printed letter paper
the `noheader` option, e.g., `\documentclass[german,noheader]{uibklttr}`.
