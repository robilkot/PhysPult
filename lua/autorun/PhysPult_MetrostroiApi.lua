if(SERVER) then return end

timer.Create("PhysPultInitApi", 0.5, 1, function()

Metrostroi = Metrostroi or {}

-- Находит кнопку в поезде или возращает её, если кнопка уже передана.
-- (train) - Ентити поезда.
-- (button) - Кнопка или ID кнопки.
-- RETURNS - Таблица кнопки, содержащие данные её инициализации или nil, если кнопка не найдена.
local function returnOrfindButton(train, button)
	local buttonType = type(button)

	if (buttonType == "table") then
		return button
	elseif (buttonType == "string") then
		return Metrostroi.GetTrainButtonById(train, button)
	end
end

-- Отправляет соятоние кнопки(наверное). Взято из Metrostroi.
local function sendButtonMessage(button,train,outside)
    local tooltip,buttID = nil,button.ID
    if button.plombed then
        tooltip,buttID = button.plombed(train)
    end
    if not buttID then 
		PrintTable(button)
		print(debug.traceback("Can't send button message!", 3))
		-- Error(Format( %s\n", ))
		return end
    net.Start("metrostroi-cabin-button")
        net.WriteEntity(train)
        net.WriteString(buttID:gsub("^.+:",""))
        net.WriteBit(button.state)
        net.WriteBool(outside)
    net.SendToServer()
    return buttID
end

-- Возвращает состояние индикатора.
-- (train) - Текущий поезд.
-- (indicatorId) - Имя индикатора.
-- RETURNS - Значени индикатора true/false, или nil, если индикатор не найден.
function Metrostroi.GetTrainIndicatorStage(train, indicatorId)
	return train:GetNW2Bool(indicatorId)
end

-- Устанавливает положени кпокпи "Нажато".
-- (train) - Текущий поезд.
-- (button) - Имя кнопки или сама кнопка.
function Metrostroi.DownTrainButton(train, button) 
    button = returnOrfindButton(train, button)

    if (button) then
        button.state = true
        sendButtonMessage(button, train, true)
    end
end

-- Устанавливает положени кпокпи "Отпущена".
-- (train) - Текущий поезд.
-- (button) - Имя кнопки или сама кнопка.
function Metrostroi.UpTrainButton(train, button)
	button = returnOrfindButton(train, button)

    if (button) then
        button.state = false
        sendButtonMessage(button, train, true)
    end
end

-- Иницализиурет короткое нажатие на кнопку.
-- (train) - Текущий поезд.
-- (button) - Имя кнопки или сама кнопка.
function Metrostroi.PressTrainButton(train, button)
	button = returnOrfindButton(train, button)

	Metrostroi.DownTrainButton(train, button)
    timer.Simple(0.1, function() Metrostroi.UpTrainButton(train, button) end)
end

-- Возвращает текущее положение кнопки.
-- REMARK - Только кнопки которые являются тумблерами имеют постоянные состояния true/false,
-- в то ремя как обычные кнопки имею положени true, только при нажатии.
-- (train) - Текущий поезд.
-- (button) - Имя кнопки или сама кнопка.
-- RETURNS - Текущее состояние кнопки true/false, или nil, если кнопка не найдена.
function Metrostroi.GetTrainSwitchStage(train, button)
	button = returnOrfindButton(train, button)
	if (button) then
		return train:GetPackedRatio(getButtonValueName(button.ID)) > 0
	end
end

-- Устанвливает текущее положение кнопки.
-- REMARK - Только кнопки которые являются тумблерами имеют постоянные состояния true/false,
-- в то ремя как обычные кнопки имею положени true, только при нажатии.
-- (train) - Текущий поезд.
-- (button) - Имя кнопки или сама кнопка.
function Metrostroi.SetTrainSwitchStage(train, button, stage)
	button = returnOrfindButton(train, button)

	if (button) then
		if (Metrostroi.GetTrainSwitchStage(train, button) != stage) then
			Metrostroi.PressTrainButton(train, button)
		end
	end
end

-- Воззращает кнопку поезда по её ID(имени).
-- (train) - Текущий поезд.
-- (buttonId) - ID кнопки(имя).
-- RETURNS - Таблица кнопки, содержащие данные её инициализации или nil, если кнопка не найдена.
function Metrostroi.GetTrainButtonById(train, buttonId)
    for kp, panel in pairs(train.ButtonMap) do
        if not train:ShouldDrawPanel(kp) then continue end
        if (panel.buttons) then
            for _, button in pairs(panel.buttons) do
                if (button.ID == buttonId) then
                    return button 
                end
            end
        end
    end
end

end)