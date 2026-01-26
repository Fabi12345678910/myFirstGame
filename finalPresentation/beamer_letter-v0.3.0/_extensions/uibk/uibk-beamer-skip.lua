

-- custom filter to add functionality for small-, med-, and bigskip
-- in uibk-beamer. Many thanks to Maximilian Breitenlechner!
-- Allows for the following block definitions in the quarto document:
--
-- ::: {.smallskip}
-- :::
--
-- ::: {.medskip}
-- :::
-- 
-- ::: {.bigskip}
-- :::
function Div(el)
  if el.classes:includes("bigskip") then
    if FORMAT:match("beamer") then
      return pandoc.RawBlock("latex", "\\bigskip")
    end
  elseif el.classes:includes("medskip") then
    if FORMAT:match("beamer") then
      return pandoc.RawBlock("latex", "\\medskip")
    end
  elseif el.classes:includes("smallskip") then
    if FORMAT:match("beamer") then
      return pandoc.RawBlock("latex", "\\smallskip")
    end
  end
  return el
end
