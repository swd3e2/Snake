if isButtonPressed(265) then
    translation.z = translation.z + 0.000000001
elseif isButtonPressed(264) then
    translation.z = translation.z - 0.000000001
end

if isButtonPressed(263) then
    translation.x = translation.x + 0.00000001
elseif isButtonPressed(262) then
    translation.x = translation.x - 0.000000001
end

rotation.y = rotation.y + 0.01