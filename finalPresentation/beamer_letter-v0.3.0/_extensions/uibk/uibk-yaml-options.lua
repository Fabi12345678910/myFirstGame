

function Pandoc(doc)
    -- ---------------------------------------------------------------
    -- Allows to set 'footertext: "my footer text"' to be shown in the
    -- footer (sets \footertext{...} for the UIBK LaTeX beamer theme)
    -- ---------------------------------------------------------------

    -- Reading custom variable 'footertext' from document meta.
    local footer_text = nil
    if doc.meta.footertext then
        footer_text = pandoc.utils.stringify(doc.meta.footertext)
    end

    -- Check if variable exists and target format is beamer
    if footer_text and FORMAT:match("beamer") then
        -- Creating LaTeX command string
        local raw_tex_string = "\\footertext{" .. footer_text .. "}"

        -- Creating the raw LaTeX block object
        local raw_tex = pandoc.RawBlock("latex", raw_tex_string)
        io.stderr:write("DEBUG: Adding footer text: " .. raw_tex_string .. "\n")

        -- That creates a new frame
        -- -- Prepend to beginning of the documents content block
        -- table.insert(doc.blocks, 1, raw_tex)

        -- Instead we would like to modify the header includes.
        -- Create a new MetaList if 'header-includes' doesn't exist
        if not doc.meta["header-includes"] then
          doc.meta["header-includes"] = pandoc.MetaList({})
        end

        -- Append the raw LaTeX command as a MetaInlines element to 'header-includes'
        local new_preamble_item = pandoc.MetaInlines({
            pandoc.RawInline("latex", raw_tex_string)
        })
        table.insert(doc.meta["header-includes"], new_preamble_item)
    end

    -- ---------------------------------------------------------------
    -- Allows to set 'url: "https://<my_url>"' to be shown on the title
    -- page of the slide deck (sets \URL{...} for the UIBK LaTeX beamer theme)
    -- ---------------------------------------------------------------

    -- Reading custom variable 'footertext' from document meta.
    local url_text = nil
    if doc.meta.URL then
        url_text = pandoc.utils.stringify(doc.meta.URL)
    end

    -- Check if variable exists and target format is beamer
    if url_text and FORMAT:match("beamer") then
        -- Creating LaTeX command string
        local raw_tex_string = "\\URL{" .. url_text .. "}"

        -- Creating the raw LaTeX block object
        local raw_tex = pandoc.RawBlock("latex", raw_tex_string)
        io.stderr:write("DEBUG: Adding URL: " .. raw_tex_string .. "\n")

        -- That creates a new frame
        -- -- Prepend to beginning of the documents content block
        -- table.insert(doc.blocks, 1, raw_tex)

        -- Instead we would like to modify the header includes.
        -- Create a new MetaList if 'header-includes' doesn't exist
        if not doc.meta["header-includes"] then
          doc.meta["header-includes"] = pandoc.MetaList({})
        end

        -- Append the raw LaTeX command as a MetaInlines element to 'header-includes'
        local new_preamble_item = pandoc.MetaInlines({
            pandoc.RawInline("latex", raw_tex_string)
        })
        table.insert(doc.meta["header-includes"], new_preamble_item)
    end
    return doc
end
