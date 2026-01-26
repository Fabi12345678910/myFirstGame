
-- Adding custom font
quarto.doc.addHtmlDependency({
  name = 'uibkfonts',
  version = '0.0.1',
  stylesheets = {'assets/css/uibk-fonts.css'}
})


uibk_modify_footer = function(elem)
    local found_footer = false
    if quarto.doc.isFormat("html:js") then
        -- find footer element
        for key,value in pairs(elem.classes) do
            if value == "footer" then
                found_footer = true
             end
        end
        -- if we have a footer: check whether it should be shown on the slides or not.
        -- If not, delete (return {}), else keep it as is (return nil).
        if found_footer then
            if show_footer == true then
                return nil -- leave the footer as it is
            else
                -- devel -- print("Footer: hide, trying to remove footer block")
                -- devel -- print(elem)
                -- devel -- print(elem.t)
                -- devel -- for key,value in pairs(elem.classes) do
                -- devel --     print(key .. "  " .. value)
                -- devel -- end
                return {} -- delete footer
            end
        end
    else
        return nil
    end
end -- function

-- loading meta information
uibk_get_meta = function(meta)
    title = meta.title
    show_footer  = meta["show-footer"]
end

-- Filter images with this function if the target format is HTML
return {
    {Meta  = uibk_get_meta}, -- init meta variables
    {Div = uibk_modify_footer}, -- remove footer if show-footer is set false
}
