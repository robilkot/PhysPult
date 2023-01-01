-----------------------------------------------------------------------------------------
--                          Творческое объединение "MetroPack"
--	Скрипт написан в 2022 году для тренажёра 81-717.5 на основе Arduino UNO.
--	Инициализация рабочих файлов для тренажёра.
--	Автор: 	robilkot
--	Steam: 	https://steamcommunity.com/id/metropacker/
--	VK:		https://vk.com/robilkot
--  Дополнительная информация в файле lua/licence.lua
-----------------------------------------------------------------------------------------

if(SERVER) then return end

-- Частота обновления состояния (Гц)
local updateFrequency = 3

local indicators = {
	-- Соотвествие битов строки состояния лампам (индексация с нуля)
	["mapping"] = { 
		
		-- 2 Блок
		["AR0"] = 3,
		["AR04"] = 4,
		["AR40"] = 5,
		["AR60"] = 6,
		["AR70"] = 7,
		["AR80"] = 8,
		["SD"] = 9,
		["VD"] = 10,
		["RP"] = 11,
		["SN"] = 12,
		["HRK"] = 13,
		["KVD"] = 14,
		["ST"] = 15,
		["KT"] = 16,
		["LN"] = 17,
		["KVC"] = 19,
		["GLIB"] = 20, -- ЛЭКК

		-- 5-6 Блоки
		["GreenRP"] = 21,
		["DoorsLeftL"] = 22,
		["L1"] = 23,
		["LSP"] = 24,
		["AVU"] = 25,
		["LKVP"] = 26,
		["RZP"] = 27,

		-- 7 Блок
		--["???"] = 28, -- Контроль печи
		["PN"] = 29,
		["DoorsRightL"] = 30,

	},
	-- Путь к файлу где содержится строка состояния
	["filename"] = "lamps.txt",
}

local switches = {
	-- Соотвествие битов строки состояния названиям кнопок (индексация с нуля)
	["mapping"] = { 

		-- 1 блок
		[0] = "VMKToggle",
		[1] = "BPSNonToggle", -- Инвертировать!
		[2] = "RezMKSet",
		[3] = "ARS13Set",

		-- 5 блок
		[4] = "R_UNchToggle",
		[5] = "R_ZSToggle",
		[6] = "R_GToggle",
		[7] = "R_RadioToggle",
		[8] = "R_Program1Set",
		[9] = "R_Program2Set",
		[10] = "VUD1Toggle",
		[11] = "R_VPRToggle",
		[12] = "KRZDSet",
		[13] = "VozvratRPSet",
		[14] = "KDLSet",
		[15] = "DoorSelectToggle",

		-- 6 блок
		[16] = "1:KVTSet",
		[17] = "1:KVTRSet",
		[18] = "VZ1Set",

		[19] = "V13Toggle",
		[43] = "V11Toggle",  -- Инвертировать!
		[44] = "V12Toggle",  -- Инвертировать!

		[20] = "OtklAVUToggle",
		[21] = "OtklBVSet",
		--[22] = "", -- Двери торцевые
		--[23] = "", -- Вентиляция кабины
		[24] = "ARSToggle",
		[25] = "ALSToggle",
		[26] = "ARSRToggle", -- Инвертировать!
		[27] = "ConverterProtectionSet",
		[28] = "KSNSet",
		[29] = "RingSet",
		[30] = "OVTToggle", -- Инвертировать!
		[31] = "ALSFreqToggle", -- Инвертировать!
		[32] = "L_1Toggle",
		[33] = "L_2Toggle", -- Инвертировать!
		[34] = "L_3Toggle", -- Инвертировать!
		[35] = "VPToggle",
		
		-- 7 блок
		[36] = "KRPSet",
		[37] = "KAHSet",
		[38] = "KDPSet",
		[39] = "L_4Toggle",
		[40] = "VUSToggle", -- Инвертировать!
		[41] = "VADToggle",
		[42] = "VAHToggle", -- Инвертировать!

	},
	-- Путь к файлу где содержится строка состояния
	["filename"] = "switches.txt", 
}


local function getPlayerDrivenTrain()
    local seat = LocalPlayer():GetParent()
    if (IsValid(seat)) then
		local train = seat:GetParent()
		if(IsValid(train)) then return train end
	end 
end

local function checkTrainType(train)
	return train:GetClass() == "gmod_subway_81-717_mvm"
end

----   API definition   ----

Metrostroi = Metrostroi or {}

function Metrostroi.TrainButtonDown(train, buttonId) 
    local button = Metrostroi.TrainGetButtonById(train, buttonId)
    if (button) then
        button.state = true
        Pult.SendButtonMessage(button, train, true)

       --[[  if train.OnButtonPressed then
            train:OnButtonPressed(button.ID:gsub("^.+:",""))
        end ]]
    end
end

function Metrostroi.TrainButtonUp(train, buttonId)
    local button = Metrostroi.TrainGetButtonById(train, buttonId)
    if (button) then
        button.state = false
        Pult.SendButtonMessage(button, train, true)

        --[[ if train.OnButtonReleased and button then
            local tooltip, buttID = nil, button.ID
            if button.plombed then
                tooltip, buttID = button.plombed(train)
            end
            train:OnButtonReleased(buttID:gsub("^.+:",""))
        end ]]
    end
end

function Metrostroi.TrainButtonToggle(train, buttonId)
    Metrostroi.TrainButtonDown(train, buttonId)
    timer.Simple(0.1, function()
        Metrostroi.TrainButtonUp(train, buttonId)
    end)
end

function Metrostroi.TrainGetButtonById(train, buttonId)
    for kp, panel in pairs(train.ButtonMap) do
        if not train:ShouldDrawPanel(kp) then continue end
        --If player is looking at this panel
        --print(panel.buttons)
        if (panel.buttons) then
            for _, button in pairs(panel.buttons) do
                if (button.ID == buttonId) then
                    return button 
                end
            end
        end
    end
end

Pult = Pult or {}

function Pult.SendButtonMessage(button,train,outside)
    local tooltip,buttID = nil,button.ID
    if button.plombed then
        tooltip,buttID = button.plombed(train)
    end
    if not buttID then Error(Format("Can't send button message! %s\n",button.ID)) return end
    net.Start("metrostroi-cabin-button")
        net.WriteEntity(train)
        net.WriteString(buttID:gsub("^.+:",""))
        net.WriteBit(button.state)
        net.WriteBool(outside)
    net.SendToServer()
    return buttID
end

-- Запись текущих состояний индикаторов в файл
function Pult.WriteIndicatorsState(train, tableIndicators, stateIndicators, filenameIndicators)
	for k, v in pairs(tableIndicators) do
		if train:GetNW2Bool(k) == true then
			stateIndicators = string.SetChar(stateIndicators, v+1, '1')
		end
	end

	local speed = train:GetPackedRatio("speed")*100
	stateIndicators = string.SetChar(stateIndicators, 1, math.floor(speed/10))
	stateIndicators = string.SetChar(stateIndicators, 2, math.floor(speed)%10)

	--print(stateIndicators)
	file.Write (filenameIndicators, stateIndicators)
end

-- Чтение текущих состояний переключателей из файла
function Pult.UpdateSwitchesState(train, tableSwitches, filenameSwitches)
	local stateString

	if file.Exists(filenameSwitches, "DATA") then stateString = file.Read(filenameSwitches) end

	for k, v in pairs(tableSwitches) do
		if stateString[k+1] == '1' then
			Metrostroi.TrainButtonDown(train, v)
		else
			Metrostroi.TrainButtonUp(train, v)
		end
	end
end

-- Обновление состояния индикаторов в файле и переключателей на пульте
function Pult.UpdateControls(tableIndicators, tableSwitches, stateString, filenameIndicators, filenameSwitches)
	local train = getPlayerDrivenTrain()

	if(not train or not checkTrainType(train)) then return end

	Pult.WriteIndicatorsState(train, tableIndicators, stateString, filenameIndicators)
	Pult.UpdateSwitchesState(train, tableSwitches, filenameSwitches)
end

function Pult.StartControlsUpdate()
	local currentState = "0000000000000000000000000000000000000000000000000000000000000000" 

	timer.Create("stateStringUpdate", 1/updateFrequency, 0, function()
		Pult.UpdateControls(indicators.mapping, switches.mapping, currentState, indicators.filename, switches.filename)
	end)
end

Pult.StartControlsUpdate()