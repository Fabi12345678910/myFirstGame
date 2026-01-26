# -------------------------------------------------------------------
# - NAME:        colors.R
# - AUTHOR:      Reto Stauffer
# - DATE:        2017-06-28
# -------------------------------------------------------------------
# - DESCRIPTION: Reading 'colors.txt' containing the faculty colors
#                of all faculties at the university of innsbruck and
#                creates a light version of the color.
#
#                The output of this script is what is used in the
#                beamerclass 'uibk' to define the two colors 'uibkcol'
#                and 'uibkcoll'.
# -------------------------------------------------------------------
# - EDITORIAL:   2017-06-28, RS: Created file on thinkreto.
# -------------------------------------------------------------------
# - L@ST MODIFIED: 2017-07-18 19:07 on thinkreto
# -------------------------------------------------------------------

rm(list = objects())

# -------------------------------------------------------------------
# Loading the required package to perform the color manipulation
# Can be installed via CRAN; call 'install.packages("colorspace").
# More information about the R package colorspace can be found on:
# * https://cran.r-project.org/package=colorspace
# * http://colorspace.r-forge.r-project.org/
# -------------------------------------------------------------------
library("colorspace")

# -------------------------------------------------------------------
# Reading color definition.
# -------------------------------------------------------------------
data <- read.table("colors.txt", header=TRUE, comment.char = "@")

# Convert hexcolors to rgb and hcl
rgb <- hex2RGB(data$original)

# Create light version of the color.
# For the light version: setting luminance to 95%
# and recude chroma for high chromatic colors.
hcl <- as(rgb, "polarLUV")
hcl@coords[,"L"] <- rep(95, nrow(data))
hcl@coords[,"C"] <- ifelse(hcl@coords[,"C"] > 50, 50, hcl@coords[,"C"])
data$man_light <- hex(hcl, fixup = TRUE)

# ... using colorspace::lighten()
data$lighter <- lighten(data$original, amount = .8)

# Create dark colors; not required for all faculty colors.
data$darker  <- darken(data$original, amount = .3)


## Swatch plot
swatchplot("Lighter" = rbind(original = data$original, cs_lighter = data$lighter, man_light = data$man_light),
           "Darker"  = rbind(original = data$original, cs_darker  = data$darker),
           nrow = 7)
#swatchplot(cbind(data$darker, data$original, data$lighter), off = 0)

# -------------------------------------------------------------------
# Create output for LaTeX beamer theme 
# -------------------------------------------------------------------

# RGB coordinates of lighter/darker colors (from lighter(); darker())
lightrgb <- hex2RGB(data$lighter)
darkrgb  <- hex2RGB(data$darker)

cat("   % Depending on the input in function \\setfaculty:\n")
cat("   % define the two colors 'uibkcol' and 'uibkcoll'.\n")
cat("   \\IfStrEqCase{#1}{%\n")
for (i in 1:nrow(data)) {
   xtra <- if (i == 1) "" else "}"
   cat(sprintf("      %s{%s}{", xtra, data$faculty[i]), "%\n", sep = "")
   # Dark faculty color
   cat(sprintf("         \\definecolor{uibkcold}{rgb}{%.2f,%.2f,%.2f}",
      darkrgb@coords[i,'R'], darkrgb@coords[i, "G"], darkrgb@coords[i, "B"]), "%\n", sep = "")
   # Official faculty color
   cat(sprintf("         \\definecolor{uibkcol}{rgb}{%.2f,%.2f,%.2f}",
       rgb@coords[i, "R"], rgb@coords[i, "G"], rgb@coords[i, "B"]), "%\n", sep = "")
   # Light faculty color
   cat(sprintf("         \\definecolor{uibkcoll}{rgb}{%.2f,%.2f,%.2f}",
      lightrgb@coords[i, "R"], lightrgb@coords[i, "G"], lightrgb@coords[i, "B"]), "%\n", sep = "")
}
# Default color
idx <- which(data$faculty == "none")
cat("      }}[%\n")
# Darker version
cat(sprintf("         \\definecolor{uibkcold}{rgb}{%.2f,%.2f,%.2f}",
   darkrgb@coords[idx, "R"], darkrgb@coords[idx, "G"], darkrgb@coords[idx, "B"]), "%\n", sep = "")
# Official uibk blue (dark)
cat(sprintf("         \\definecolor{uibkcol}{rgb}{%.2f,%.2f,%.2f}",
   rgb@coords[idx, "R"], rgb@coords[idx, "G"], rgb@coords[idx, "B"]), "%\n", sep = "")
# Light version of the uibk blue
cat(sprintf("         \\definecolor{uibkcoll}{rgb}{%.2f,%.2f,%.2f}",
   lightrgb@coords[idx, "R"], lightrgb@coords[idx, "G"], lightrgb@coords[idx, "B"]), "%\n", sep = "")
cat("      ]\n")
   

## Markdown explanation (from old README.md)

## Setfacultycolors
## ----------------
## 
## The ``\setfacultycolors{slug}`` command in beamerthemeuibk and beamerthemeuibkposter
## is used to specify the faculty specific colors (namely  ``\uibkcol`` and ``\uibkcoll``)
## which are used in some of the commands provided by the beamertheme.
## PLEASE NOTE that this option should only be used in very special situations and not
## for your day-by-day work!
## 
## The following `slug`'s are currently implemented:
## 
## * `architektur`:  Fakultaet fuer Architektur
## * `betriebswirtschaft`: F. f. Betriebswirtschaft
## * `bildungswissenschaften`: F. f. Bildungswissenschaften
## * `biologie`: F. f. Biologie
## * `chemieundpharmazie`: F. f. Chemie und Pharmazie
## * `geoundatmosphaerenwissenschaften`: F. f. Geo- und Atmosphaerenwissenschaften
## * `mip`: F. f. Mathematik, Informatik und Physik
## * `socialandpoliticalsciences`: F. f. Soziale und Politische Wissenschaften
## * `psychologieundsportwissenschaften`: F. f. Psychologie und Sportwissenschaften
## * `technischewissenschaften`: F. f. Technische Wissenschaften
## * `volkswirtschaftundstatistik`: F. f. Volkswirtschaft und Statistik
## * `theol`: Katholisch-Theologische F.
## * `philologisch`: Philologisch-Kulturwissenschaftliche F.
## * `philosophisch`: Philosophisch-Historische F.
## * `law`: Rechtswissenschaftliche F.
## * `soe`: School of Education -- F. f. LehrerInnenbildung
## * `none`: Uses the color 'uibkblue' and a light version of it.
## 
## * These two colors could also be overruled manually (please dont).
## * `\definecolor{uibkcol}{cmyk}{.80,0,0,.20}`: defines the darker of the two colors.
## * `\definecolor{uibkcoll}{cmyk}{.24,.07,.06,0}`: defines the lighter of the two colors.
