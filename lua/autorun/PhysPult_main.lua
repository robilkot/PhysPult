-----------------------------------------------------------------------------------------
--                          Творческое объединение "MetroPack"
--	Скрипт написан в 2022 году для тренажёра 81-717 на основе ESP32.
--	Инициализация рабочих файлов для тренажёра.
--	Автор: 	robilkot
--	Steam: 	https://steamcommunity.com/id/metropacker/
--	VK:		https://vk.com/robilkot
--  Дополнительная информация в файле lua/licence.lua
-----------------------------------------------------------------------------------------

if(SERVER) then return end

timer.Create("PhysPultInit", 0.5, 1, function()

-- Список индикаторов с соответствующим номером бита. Биты 26-39 под скорость нужны. Индексация с нуля
local indicators = {
	-- 2 Блок
	["KVC"] = 19, -- 19
	["AR0"] = 11,
	["AR04"] = 13, 
	["AR40"] = 12, 
	["AR60"] = 9,
	["AR70"] = 25, 
	["AR80"] = 10,
	["SD"] = 18, 
	["VD"] = 24, 
	["RP"] = 20,
	["SN"] = 15,
	["HRK"] = 22, 
	["KVD"] = 8, 
	["ST"] = 17, 
	--["???"] = 21, -- ДВ
	["KT"] = 23,
	["LN"] = 14,
	["GLIB"] = 16, -- ЛЭКК

	-- 5-6 Блоки
	["GreenRP"] = 7,
	-- ["DoorsLeftL"] = ???,
	["L1"] = 6,
	["LSP"] = 4,
	["AVU"] = 3,
	["LKVP"] = 5,
	-- ["RZP"] = ???,

	-- 7 Блок
	--["???"] = -, -- Контроль печи
	["PN"] = 2,
	-- ["DoorsRightR"] = -,
}

-- Список тумблеров с соответствующим номером бита.
-- [Номер бита] = { "Имя тумблера", инвертировать ли (boolean) } 
local switches = {
	-- 1 блок
	[38] = { "VMKToggle", true },
	[39] = { "BPSNonToggle", true },

	-- 5 блок
	[3] = { "R_UNchToggle", true },
	[1] = { "R_ZSToggle", true },
	[4] = { "R_GToggle", true },
	[2] = { "R_RadioToggle", true },
	[10] = { "VUD1Toggle", true },
	--[-] = { "R_VPRToggle" },
	--[-] = { "DoorSelectToggle" },

	-- 6 блок
	[26] = { "V13Toggle", true },
	[27] = { "V11Toggle", true },
	[25] = { "V12Toggle", true },

	--[-] = { "OtklAVUToggle"},
	--[-] = "???", -- Двери торцевые
	--[18] = "???", -- Вентиляция кабины
	[15] = { "ARSToggle", true },
	[14] = { "ALSToggle", true },
	-- [-] = { "ARSRToggle" },
	[13] = { "OVTToggle" } ,
	-- [-] = { "ALSFreqToggle" },
	[40] = { "L_1Toggle", true }, -- Аварийное освещение (1 блок)
	[20] = { "L_2Toggle", true }, 
	[19] = { "L_3Toggle", true },
	[18] = { "VPToggle", true },
	
	-- 7 блок
	[34] = { "L_4Toggle", true },
	[35] = { "VUSToggle", true }, 
	[36] = { "VADToggle", true },
	[37] = { "VAHToggle", true },
}

-- Список кнопок с соответствующим номером бита.
-- [Номер бита] = { "Имя кнопки", инвертировать ли (boolean) } 
local buttons = {
	-- 1 блок
	[33] = { "RezMKSet", false },
	[42] = { "ARS13Set", false },
	-- [41] = { "???", false }, -- Радио 13В

	-- 5 блок
	[5] = { "R_Program1Set", true },
	[6] = { "R_Program2Set", true },
	[9] = { "KRZDSet", true },
	[11] = { "VozvratRPSet", true },
	-- [1] =  { "KDLSet", false }, -- обрабатываются особым образом ниже
	-- [2] =  { "KDLSet", false }, 

	-- 6 блок
	[22] = {  "1:KVTSet", true },
	-- [24] = Вкл ЭПК
	[23] = {  "1:KVTRSet", false },
	[28] = {  "VZ1Set", false }, -- ЛКВ
	-- [-] = { "OtklBVSet", false },
	[12] = { "ConverterProtectionSet", false },
	[16] = {  "KSNSet", true },
	[17] = {  "RingSet", true },

	-- 7 блок
	-- [31] =  { "KDRSet", false }, -- обрабатываются особым образом ниже
	[32] = { "KRPSet", true },
	-- [--] = { "KAHSet", true },
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
    if not buttID then 
		PrintTable(button)
		Error(Format("Can't send button message! %s\n",button.ID))
		return end
    net.Start("metrostroi-cabin-button")
        net.WriteEntity(train)
        net.WriteString(buttID:gsub("^.+:",""))
        net.WriteBit(button.state)
        net.WriteBool(outside)
    net.SendToServer()
    return buttID
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

PhysPult.GameCranePosition = 1
PhysPult.GameControllerPosition = 4

-- Интверал между обновлениями состояния (мс).
PhysPult.UpdateInterval = 50

local function GetNumericValuesString(train)
	local numerics = {
		math.floor(train:GetPackedRatio("speed") * 100),
		math.floor(train:GetPackedRatio("BLPressure") * 100 * 8 / 5 * 1.43),
		math.floor(train:GetPackedRatio("TLPressure") * 100 * 8 / 5 * 1.63),
		math.floor(train:GetPackedRatio("BCPressure") * 100 * 5 / 9 * 3.15),
		math.floor(train:GetPackedRatio("BatteryVoltage") * 100 * 1.6 * 1.7),
		math.floor(train:GetPackedRatio("EnginesVoltage") * 100 * 2.17),
		-1 * math.Clamp(math.floor((train:GetPackedRatio("EnginesCurrent") * 1000 - 500) * 0.6), -255, 255),
	}
	
	return table.concat(numerics, ',')..';'
end

local function GetBinaryValuesString(train)
	local registers = {
		0, 0, 0, 0, 0
	}

	for k, v in pairs(indicators) do
		local bitIndex = v

		local bitInRegister = bitIndex % 8
		local registerNumber = math.floor(bitIndex/ 8) + 1
		
		local indicatorState = Metrostroi.GetTrainIndicatorStage(train, k)

		if(indicatorState == true) then
			registers[registerNumber] = bit.bor(registers[registerNumber], bit.lshift(1, bitInRegister))
		else
			registers[registerNumber] = bit.band(registers[registerNumber], bit.bnot(bit.lshift(1, bitInRegister)))
		end
	end

	return table.concat(registers, ',')..';'
end

-- Синхронизация индикаторов в поезде.
function PhysPult.SynchronizeIndicators(train)
	local msg = "W;"

	msg = msg..GetNumericValuesString(train)
	msg = msg..GetBinaryValuesString(train)

	PhysPult.SocketWrtData = msg
end

-- Синхронизация тумблреров в поезде.
function PhysPult.SynchronizeSwitches(train, registers)
	for k, v in pairs(switches) do
		local bitIndex = k

		local bitInRegister = 7 - (bitIndex - 1) % 8
		local registerNumber = math.floor((bitIndex - 1)/ 8) + 1
		local register = registers[registerNumber]
		
		local switchState = bit.band(1, bit.rshift(tonumber(register), bitInRegister)) == 1

		if (v[2]) then
			switchState = not switchState
		end

		Metrostroi.SetTrainSwitchStage(train, v[1], switchState)	
	end
end

-- Синхронизация кнопок в поезде.
function PhysPult.SynchronizeButtons(train, registers)
	for k, v in pairs(buttons) do
		local bitIndex = k

		local bitInRegister = 7 - (bitIndex - 1) % 8
		local registerNumber = math.floor((bitIndex - 1)/ 8) + 1
		local register = registers[registerNumber]
		
		local switchState = bit.band(1, bit.rshift(tonumber(register), bitInRegister)) == 1

		if (v[2] == false) then
			switchState = not switchState
		end
		
		if(switchState == true) then
			Metrostroi.UpTrainButton(train, v[1])
		else
			Metrostroi.DownTrainButton(train, v[1])
		end
	end

	local kdlRegister = registers[1]
	local kdlOneState = bit.band(1, bit.rshift(tonumber(kdlRegister), 1)) == 0
	local kdlTwoState = bit.band(1, bit.rshift(tonumber(kdlRegister), 0)) == 0

	if(kdlOneState or kdlTwoState) then
		Metrostroi.SetTrainSwitchStage(train, "DoorSelectToggle", false)
		Metrostroi.DownTrainButton(train, "KDLSet")
	else
		Metrostroi.UpTrainButton(train, "KDLSet")
	end

	local kpdRegister = registers[4]
	local kpdState = bit.band(1, bit.rshift(tonumber(kpdRegister), 1)) == 0

	if(kpdState) then
		Metrostroi.UpTrainButton(train, "KDPSet")
	else
		Metrostroi.SetTrainSwitchStage(train, "DoorSelectToggle", true)
		Metrostroi.DownTrainButton(train, "KDPSet")
	end
end

local CRANE_POSITIONS = {
	["334"] = {
		15,
		31,
		43,
		62,
		180,
	},
	["013"] = {
		-- Not implemented in hardware
	}
}

local kvinputs = {
	["81-717"] = {
		[1] = "KVSetX1B",
		[2] = "KVSetX2",
		[3] = "KVSetX3",
		[0] = "KVSet0",
		[4] = "KVSetT1B",
		[5] = "KVSetT1AB",
		[6] = "KVSetT2",
	}
}

local kminputs = {
	["81-717"] = {
		"PneumaticBrakeSet1",
		"PneumaticBrakeSet2",
		"PneumaticBrakeSet3",
		"PneumaticBrakeSet4",
		"PneumaticBrakeSet5",
		"PneumaticBrakeSet6",
		"PneumaticBrakeSet7",
	}
}

function PhysPult.SynchronizeReverser(train, reverserPosition)
	PhysPult.ReverserPosition = reverserPosition

	local nativeReverserPosition = train:GetNW2Int("ReverserPosition") -- 0 = B, 1 = 0, 2 = F
	local currentReverserPosition = 0

	if(nativeReverserPosition == 0) then
		 currentReverserPosition = 2
	elseif(nativeReverserPosition == 1) then
		 currentReverserPosition = 0
	elseif(nativeReverserPosition == 2) then
		 currentReverserPosition = 1
	end

	if(currentReverserPosition != PhysPult.ReverserPosition) then
		if(PhysPult.ReverserPosition == 0) then
			if(currentReverserPosition == 1) then 
				Metrostroi.UpTrainButton(train, { ["ID"] = "KVReverserUp" } )
				Metrostroi.UpTrainButton(train, { ["ID"] = "KVReverserDown" } )
				Metrostroi.DownTrainButton(train, { ["ID"] = "KVReverserDown" } )
			elseif(currentReverserPosition == 2) then
				Metrostroi.UpTrainButton(train, { ["ID"] = "KVReverserUp" } )
				Metrostroi.UpTrainButton(train, { ["ID"] = "KVReverserDown" } )
				Metrostroi.DownTrainButton(train, { ["ID"] = "KVReverserUp" } )
			end
			
			Metrostroi.PressTrainButton(train, { ["ID"] = "KVWrenchNone" })
		elseif(PhysPult.ReverserPosition == 1) then
			Metrostroi.PressTrainButton(train, { ["ID"] = "KVWrenchKV" })

			if(currentReverserPosition == 2) then
				Metrostroi.UpTrainButton(train, { ["ID"] = "KVReverserUp" } )
				Metrostroi.UpTrainButton(train, { ["ID"] = "KVReverserDown" } )
				Metrostroi.DownTrainButton(train, { ["ID"] = "KVReverserUp" } )
			elseif(currentReverserPosition == 0) then 
				Metrostroi.UpTrainButton(train, { ["ID"] = "KVReverserUp" } )
				Metrostroi.UpTrainButton(train, { ["ID"] = "KVReverserDown" } )
				Metrostroi.DownTrainButton(train, { ["ID"] = "KVReverserDown" } )
			end
		elseif (PhysPult.ReverserPosition == 2) then
			Metrostroi.PressTrainButton(train, { ["ID"] = "KVWrenchKV" })

			if(currentReverserPosition == 1) then 
				Metrostroi.UpTrainButton(train, { ["ID"] = "KVReverserUp" } )
				Metrostroi.UpTrainButton(train, { ["ID"] = "KVReverserDown" } )
				Metrostroi.DownTrainButton(train, { ["ID"] = "KVReverserUp" } )
			elseif(currentReverserPosition == 0) then 
				Metrostroi.UpTrainButton(train, { ["ID"] = "KVReverserUp" } )
				Metrostroi.UpTrainButton(train, { ["ID"] = "KVReverserDown" } )
				Metrostroi.DownTrainButton(train, { ["ID"] = "KVReverserDown" } )
			end
		end
	end
end

function PhysPult.SynchronizeController(train, controllerPosition)
	local previousKvPosition = PhysPult.ControllerPosition

	PhysPult.ControllerPosition = controllerPosition

	if(PhysPult.ControllerPosition != previousKvPosition) then
		Metrostroi.UpTrainButton(train, { ["ID"] = kvinputs["81-717"][previousKvPosition] })
	end
	Metrostroi.DownTrainButton(train, { ["ID"] = kvinputs["81-717"][PhysPult.ControllerPosition] })

	
end

function PhysPult.SynchronizeCrane(train, cranePosition)
	local cranePositions = CRANE_POSITIONS["334"]

	local previousKmPosition = PhysPult.CranePosition

	for k, v in pairs(cranePositions) do
		if(cranePosition > v) then continue end
		
		local median = cranePositions[k]

		if (k > 1) then
			median = (median + cranePositions[k - 1]) / 2

			if(cranePosition - median > 0) then
				PhysPult.CranePosition = k
			else
				PhysPult.CranePosition = k - 1
			end
		end
		
		break
	end

	Metrostroi.DownTrainButton(train, { ["ID"] = kminputs["81-717"][PhysPult.CranePosition] })
	if(PhysPult.CranePosition != previousKmPosition) then
		Metrostroi.UpTrainButton(train, { ["ID"] = kminputs["81-717"][previousKmPosition] })
	end
end

-- Синхронизация физического пульта, с виртуальным пультом поезда, в котром сидит игрок.
function PhysPult.Synchronize()
	local train = getPlayerDrivenTrain()
	
	if(train and checkTrainType(train)) then
		if(PhysPult.SocketRecData) then 
			local substrings = string.Explode(';', PhysPult.SocketRecData)
			local numerics = string.Explode(',', substrings[2])
			local binaries = string.Explode(',', substrings[3])
			
			local cranePosition = 255 - tonumber(numerics[1])
			local reverserPosition = tonumber(numerics[2])
			local controllerPosition = tonumber(numerics[3])
			
			PhysPult.SynchronizeSwitches(train, binaries)
			PhysPult.SynchronizeButtons(train, binaries)
			PhysPult.SynchronizeController(train, controllerPosition)
			-- PhysPult.SynchronizeReverser(train, reverserPosition)
			-- todo: crane
		end

		PhysPult.SynchronizeIndicators(train)
	end
end

concommand.Add("physpult_start", function()
	PhysPult.CreateSocket()

    timer.Create("ControlsSync", PhysPult.UpdateInterval / 1000, 0, function()
		PhysPult.Synchronize()
	end)
end)

concommand.Add("physpult_stop", function()
	if(timer.Exists("ControlsSync")) then timer.Remove("ControlsSync") end
    
    PhysPult.CloseSocket()
end)

end)