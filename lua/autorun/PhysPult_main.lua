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

timer.Create("PhysPultInit", 0.5, 1, function()

-- Список индикаторов с соответствующим номером бита.
local indicators = {
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
}

-- Список тумблеров с соответствующим номером бита.
-- [Номер бита] = { "Имя кнопки", инвертировать ли(по стандарту false) } 
local switches = {
	-- 1 блок
	[0] = { "VMKToggle" },
	[1] = { "BPSNonToggle" }, -- Инвертировать?

	-- 5 блок
	[4] = { "R_UNchToggle" },
	[5] = { "R_ZSToggle" },
	[6] = { "R_GToggle" },
	[7] = { "R_RadioToggle" },
	[10] = { "VUD1Toggle" },
	[11] = { "R_VPRToggle" },
	[15] = { "DoorSelectToggle" },

	-- 6 блок
	[19] = { "V13Toggle" },
	[43] = { "V11Toggle" },
	[44] = { "V12Toggle" },

	[20] = { "OtklAVUToggle"},
	--[22] = "", -- Двери торцевые
	--[23] = "", -- Вентиляция кабины
	[24] = { "ARSToggle" },
	[25] = { "ALSToggle" },
	[26] = { "ARSRToggle" },
	[30] = { "OVTToggle" } ,
	[31] = { "ALSFreqToggle" },
	[32] = { "L_1Toggle" },
	[33] = { "L_2Toggle" }, 
	[34] = { "L_3Toggle" },
	[35] = { "VPToggle" },
	
	-- 7 блок
	[39] = { "L_4Toggle" },
	[40] = { "VUSToggle" }, 
	[41] = { "VADToggle" },
	[42] = { "VAHToggle" },
}

-- Список кнопок для снхронизации, с соответствующим номером бита.
local buttons = {
	-- 1 блок
	[2] = "RezMKSet",
	[3] = "ARS13Set",

	-- 5 блок
	[8] = "R_Program1Set",
	[9] = "R_Program2Set",
	[12] = "KRZDSet",
	[13] = "VozvratRPSet" ,
	[14] =  "KDLSet" ,

	-- 6 блок
	[16] =  "1:KVTSet" ,
	[17] =  "1:KVTRSet" ,
	[18] =  "VZ1Set" ,
	[21] = "OtklBVSet",
	[27] = "ConverterProtectionSet",
	[28] =  "KSNSet" ,
	[29] =  "RingSet" ,

	-- 7 блок
	[36] =  "KRPSet" ,
	[37] =  "KAHSet" ,
	[38] =  "KDPSet" ,
}

-- Возращает поезд а котором сидит игрок.
-- RETURNS - Ентити поезда в котором сидит игрок или nil, если игрок не находится в поезде.
local function getPlayerDrivenTrain()
    local seat = LocalPlayer():GetParent()
    if (IsValid(seat)) then
		local train = seat:GetParent()
		if(IsValid(train)) then return train end
	end 
end

-- Проверяет, является ли текущий тип поезда доупстимым.
-- (train) - Ентити поезда.
-- RETURNS - Является ли поезд допустимым.
local function checkTrainType(train)
	return train:GetClass() == "gmod_subway_81-717_mvm"
end

-- Отправляет соятоние кнопки(наверное). Взято из Metrostroi.
local function sendButtonMessage(button,train,outside)
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

-- Добивает строку до указанной длины необходиымым символами.
-- REMARK - Если длина строки больше необходимой, то строка будет просто возвращена.
-- (str) - Строка, которую необходимо дополнить.
-- (char) - Символ которым будет дополняться строка.
-- (len) - Необходимая длина.
-- RETURNS - Новая строка, дополненная до необходимой длины указанными символами.
local function stringPadRight(str, char, len)
	local strLen = string.len(str);

	if (strLen >= len) then return str end

	if (#char != 1) then return nil end

	for i = 1, len - strLen do
		str = str..char
	end

	return str
end

-- Возвращает имя по которому можно получить значение кнопки, на совное ID кнопки.
-- (buttonId) - ID кнокпи.
-- RETURNS - Имя по которому можно поулчитьб знаечени кнопки из метода поезда GetPackedRatio.
local function getButtonValueName(buttonId)
	local key = string.gsub(buttonId, "^.+:","")
	key = string.Replace(key, "Toggle", "")
	key = string.Replace(key, "Set", "")

	return key
end

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

----   API definition   ----

Metrostroi = Metrostroi or {}

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

PhysPult = PhysPult or {}

-- Количество переключателей и ламп (длины строк)
PhysPult.SwitchesNumber = 64 --math.max(table.maxn(switches), table.maxn(buttons))
PhysPult.IndicatorsNumber = 64 --table.maxn(indicators)

-- Частота обновления состояния (Гц).
PhysPult.UpdateFrequency = 5

-- Порт для подключения по сокетам
PhysPult.SocketPort = 61000

-- Синхронизация индикаторов в поезде.
function PhysPult.SynchronizeIndicators(train)
	local currentState = stringPadRight("", '0', PhysPult.IndicatorsNumber)

	for k, v in pairs(indicators) do
		if Metrostroi.GetTrainIndicatorStage(train, k) == true then
			currentState = string.SetChar(currentState, v + 1, '1')
		else 
			currentState = string.SetChar(currentState, v + 1, '0')
		end
	end

	local speed = train:GetPackedRatio("speed") * 100
	currentState = string.SetChar(currentState, 1, math.floor(speed / 10))
	currentState = string.SetChar(currentState, 2, math.floor(speed) % 10)

	PhysPult.SocketWrtData = currentState.."\0"
end

-- Синхронизация тумблреров в поезде.
function PhysPult.SynchronizeSwitches(train)
	for k, v in pairs(switches) do
		local stage = PhysPult.SocketRecData[k + 1] == '1'

		if (v[2]) then
			stage = not stage
		end
		
		Metrostroi.SetTrainSwitchStage(train, v[1], stage)	
	end
end

-- Синхронизация кнопок в поезде.
function PhysPult.SynchronizeButtons(train)
	for k, v in pairs(buttons) do
		if (PhysPult.SocketRecData[k] == '1') then
			Metrostroi.DownTrainButton(train, v)
		else 
			Metrostroi.UpTrainButton(train, v)
		end
	end
end

-- Синхронизация физического пульта, с виртуальным пультом поезда, в котром сидит игрок.
function PhysPult.Synchronize()
	local train = getPlayerDrivenTrain()

	if(train and checkTrainType(train)) then
		if(PhysPult.SocketRecData) then
			PhysPult.SynchronizeSwitches(train)
			PhysPult.SynchronizeButtons(train)
		end

		PhysPult.SynchronizeIndicators(train)
	end
end

-- Старт синхронизации физического пульта и виртуального поезда, в котром сидит игрок.
function PhysPult.StartSynchronize()
	PhysPult.StartServer("127.0.0.1", PhysPult.SocketPort)

	timer.Create("stateStringUpdate", 1 / PhysPult.UpdateFrequency, 0, function()
		PhysPult.Synchronize()
	end)
	if(timer.Exists("stateStringUpdate")) then chat.AddText("PhysPult by MetroPack started!") end
end

PhysPult.StartSynchronize()

end)