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
	[34] = { "VMKToggle", true },
	[33] = { "BPSNonToggle", true },

	-- 5 блок
	[5] = { "R_UNchToggle", true },
	[7] = { "R_ZSToggle", true },
	[4] = { "R_GToggle", true },
	[6] = { "R_RadioToggle", true },
	[14] = { "VUD1Toggle", true },
	--[-] = { "R_VPRToggle" },
	--[-] = { "DoorSelectToggle" },

	-- 6 блок
	[30] = { "V13Toggle", true },
	[29] = { "V11Toggle", true },
	[31] = { "V12Toggle", true },

	--[-] = { "OtklAVUToggle"},
	--[-] = "???", -- Двери торцевые
	--[22] = "???", -- Вентиляция кабины
	[9] = { "ARSToggle", true },
	[10] = { "ALSToggle", true },
	-- [-] = { "ARSRToggle" },
	[11] = { "OVTToggle" } ,
	-- [-] = { "ALSFreqToggle" },
	[32] = { "L_1Toggle", true }, -- Аварийное освещение (1 блок)
	[20] = { "L_2Toggle", true }, 
	[21] = { "L_3Toggle", true },
	[22] = { "VPToggle", true },
	
	-- 7 блок
	[38] = { "L_4Toggle", true },
	[37] = { "VUSToggle", true }, 
	[36] = { "VADToggle", true },
	[35] = { "VAHToggle", true },
}

-- Список кнопок с соответствующим номером бита.
-- [Номер бита] = { "Имя кнопки", инвертировать ли (boolean) } 
local buttons = {
	-- 1 блок
	[39] = { "RezMKSet", false },
	[46] = { "ARS13Set", false },
	-- [???] = { "???", false }, -- Радио 13В

	-- 5 блок
	[3] = { "R_Program1Set", true },
	[2] = { "R_Program2Set", true },
	[15] = { "KRZDSet", true },
	[13] = { "VozvratRPSet", true },
	-- [1] =  { "KDLSet", false }, -- обрабатываются особым образом ниже
	-- [2] =  { "KDLSet", false }, 

	-- 6 блок
	[18] = {  "1:KVTSet", true },
	-- [16] = Вкл ЭПК
	[17] = {  "1:KVTRSet", false },
	[28] = {  "VZ1Set", false }, -- ЛКВ
	-- [-] = { "OtklBVSet", false },
	[12] = { "ConverterProtectionSet", false },
	[8] = {  "KSNSet", true },
	[23] = {  "RingSet", true },

	-- 7 блок
	-- [31] =  { "KDRSet", false }, -- обрабатываются особым образом ниже
	[24] = { "KRPSet", true },
	-- [--] = { "KAHSet", true },
}

PhysPult = PhysPult or {}

-- Интверал между обновлениями состояния (мс).
PhysPult.UpdateInterval = 25

PhysPult.FeatureFlags = {
	["Controller"] = 1,
	["Reverser"] = 2,
	["Crane"] = 4,
	["InputRegisters"] = 8,
    ["GaugesLighting"] = 16,
    ["Potentiometer"] = 32,
}

local previousNumericValues = {}

local function GetNumericValuesString(train)
	local numerics = {
		[0] = math.floor(train:GetPackedRatio("speed") * 100),
		[1] = math.floor(train:GetPackedRatio("BLPressure") * 100 * 8 / 5 * 1.43),
		[2] = math.floor(train:GetPackedRatio("TLPressure") * 100 * 8 / 5 * 1.63),
		[3] = math.floor(train:GetPackedRatio("BCPressure") * 100 * 5 / 9 * 3.15),
		[4] = math.floor(train:GetPackedRatio("BatteryVoltage") * 100 * 1.6 * 1.7),
		[5] = math.floor(train:GetPackedRatio("EnginesVoltage") * 100 * 2.17),
		[6] = -1 * math.Clamp(math.floor((train:GetPackedRatio("EnginesCurrent") * 1000 - 500) * 0.6), -255, 255),
	}

	local changed_numerics = {}

	for key, value in pairs(numerics) do
		if (previousNumericValues[key] != value) then
			previousNumericValues[key] = value
			changed_numerics[key] = value
		end
	end
	
	local str = ""

	for key, value in pairs(changed_numerics) do
		str = str..key..'/'..value..','
	end

	str = string.Trim(str, ',')..';'
	
	return str
end

local previousIndicatorsValues = {}

local function GetBinaryValuesString(train)
	local enabled_indicators = {}
	local disabled_indicators = {}

	for name, index in pairs(indicators) do
		local state = Metrostroi.GetTrainIndicatorStage(train, name)
		if(previousIndicatorsValues[name] != state) then
			previousIndicatorsValues[name] = state
			if(state) then
				table.insert(enabled_indicators, index)
			else
				table.insert(disabled_indicators, index)
			end
		end

	end

	return table.concat(enabled_indicators, ',')..';'..table.concat(disabled_indicators, ',')..';'
end

-- Синхронизация кнопок в поезде.
function PhysPult.SynchronizeButtons(train, registers)
	if(kdlOneState or kdlTwoState) then
		Metrostroi.SetTrainSwitchStage(train, "DoorSelectToggle", false)
		Metrostroi.DownTrainButton(train, "KDLSet")
	else
		Metrostroi.UpTrainButton(train, "KDLSet")
	end

	if(kpdState) then
		Metrostroi.UpTrainButton(train, "KDPSet")
	else
		Metrostroi.SetTrainSwitchStage(train, "DoorSelectToggle", true)
		Metrostroi.DownTrainButton(train, "KDPSet")
	end
end

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
		Metrostroi.UpTrainButton(train, { ["ID"] = "KVReverserUp" } )
		Metrostroi.UpTrainButton(train, { ["ID"] = "KVReverserDown" } )

		if(PhysPult.ReverserPosition == 0) then
			if(currentReverserPosition == 1) then 
				Metrostroi.DownTrainButton(train, { ["ID"] = "KVReverserDown" } )
			elseif(currentReverserPosition == 2) then
				Metrostroi.DownTrainButton(train, { ["ID"] = "KVReverserUp" } )
			end
			
			Metrostroi.PressTrainButton(train, { ["ID"] = "KVWrenchNone" })
		elseif(PhysPult.ReverserPosition == 1) then
			Metrostroi.PressTrainButton(train, { ["ID"] = "KVWrenchKV" })

			if(currentReverserPosition == 2) then
				Metrostroi.DownTrainButton(train, { ["ID"] = "KVReverserUp" } )
			elseif(currentReverserPosition == 0) then 
				Metrostroi.DownTrainButton(train, { ["ID"] = "KVReverserDown" } )
			end
		elseif (PhysPult.ReverserPosition == 2) then
			Metrostroi.PressTrainButton(train, { ["ID"] = "KVWrenchKV" })

			if(currentReverserPosition == 1) then
				Metrostroi.DownTrainButton(train, { ["ID"] = "KVReverserUp" } )
			elseif(currentReverserPosition == 0) then
				Metrostroi.DownTrainButton(train, { ["ID"] = "KVReverserDown" } )
			end
		end
	end
end

function PhysPult.SynchronizeController(train, controllerPosition)
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

	local previousKvPosition = PhysPult.ControllerPosition

	PhysPult.ControllerPosition = controllerPosition

	if(PhysPult.ControllerPosition != previousKvPosition) then
		local prevId = kvinputs["81-717"][previousKvPosition]
		if(prevId) then Metrostroi.UpTrainButton(train, { ["ID"] = prevId }) end
	end
	local newId = kvinputs["81-717"][PhysPult.ControllerPosition]
	Metrostroi.DownTrainButton(train, { ["ID"] = newId })	
end

function PhysPult.SynchronizeCrane(train, cranePosition)
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

-- todo: refactor
-- todo: crane
function PhysPult.SynchronizeInputs(train, enabledPins, disabledPins, newValues)
	local kvPairs = {}
	for _, v in pairs(newValues) do
		local substr = string.Explode('/', v)
		local key = tonumber(substr[1])
		local value = tonumber(substr[2])
		if(key) then
			kvPairs[key] = value
		end
	end
	
	local controllerPosition = kvPairs[0]
	local reverserPosition = kvPairs[1]
	local cranePosition = kvPairs[2]

	if (controllerPosition) then PhysPult.SynchronizeController(train, controllerPosition) end
	if (reverserPosition) then PhysPult.SynchronizeReverser(train, reverserPosition) end

	for k, v in pairs(enabledPins) do
		local pin = tonumber(v)
		local invert = false

		if(pin == 0 or pin == 1) then
			Metrostroi.UpTrainButton(train, "KDLSet")
		end

		if(pin == 25) then
			Metrostroi.SetTrainSwitchStage(train, "DoorSelectToggle", true)
			Metrostroi.DownTrainButton(train, "KDPSet")
		end

		if(buttons[pin]) then
			local name = buttons[pin][1]
			local invert = buttons[pin][2]

			if(invert == true) then 
				Metrostroi.UpTrainButton(train, name)
			else
				Metrostroi.DownTrainButton(train, name)
			end
		elseif(switches[pin]) then
			local name = switches[pin][1]
			local invert = switches[pin][2]

			if(invert == true) then 
				Metrostroi.SetTrainSwitchStage(train, name, false)
			else
				Metrostroi.SetTrainSwitchStage(train, name, true)
			end
		else
			-- wrong pin
		end
	end

	for k, v in pairs(disabledPins) do
		local pin = tonumber(v)
		local invert = false

		if(pin == 0 or pin == 1) then
			Metrostroi.SetTrainSwitchStage(train, "DoorSelectToggle", false)
			Metrostroi.DownTrainButton(train, "KDLSet")
		end

		if(pin == 25) then
			Metrostroi.UpTrainButton(train, "KDPSet")
		end

		if(buttons[pin]) then
			local name = buttons[pin][1]
			local invert = buttons[pin][2]

			if(invert) then
				Metrostroi.DownTrainButton(train, name)
			else
				Metrostroi.UpTrainButton(train, name)
			end
		elseif(switches[pin]) then
			local name = switches[pin][1]
			local invert = switches[pin][2]

			if(invert == true) then 
				Metrostroi.SetTrainSwitchStage(train, name, true)
			else
				Metrostroi.SetTrainSwitchStage(train, name, false)
			end
		else
			-- wrong pin
		end
	end
end

function PhysPult.Synchronize()
	local train = getPlayerDrivenTrain()
	if(not train or not checkTrainType(train)) then return end

	local msg = "S;"

	msg = msg..GetBinaryValuesString(train)
	msg = msg..GetNumericValuesString(train)

	if(msg != "S;;;;") then
		PhysPult.Send(msg)
	end
end

function PhysPult.AcceptMessage(msg)
	local train = getPlayerDrivenTrain()
	if(not train or not checkTrainType(train)) then return end

	local substrings = string.Explode(';', msg)
	local messageType = substrings[1]

	if(messageType == 'S') then
		local enabledPins = string.Explode(',', substrings[2])
		local disabledPins = string.Explode(',', substrings[3])
		local newValues = string.Explode(',', substrings[4])

		PhysPult.SynchronizeInputs(train, enabledPins, disabledPins, newValues)
	elseif(messageType == 'R') then
		previousIndicatorsValues = {}
		previousNumericValues = {}
		PhysPult.Synchronize()
	end
end

function PhysPult.ConfigurePult(featureFlags)
	local config = "C;"
		
	for key, value in pairs(PhysPult.FeatureFlags) do
		if(table.HasValue(featureFlags, key)) then
			print("enable", key)
			config = config..'0/'..value..','
		else
			print("disable", key)
			config = config..'1/'..value..','
		end
	end

	config = string.Trim(config, ',')..';'

	PhysPult.Send(config)
end

concommand.Add("pult_start", function()
	local onConnected = function()
		timer.Create("physpult_update", PhysPult.UpdateInterval / 1000, 0, PhysPult.Synchronize)


		local enabledFlags = {
			"Controller",
			"InputRegisters",
			"GaugesLighting"
			-- "Reverser"
		}

		PhysPult.ConfigurePult(enabledFlags)

		PhysPult.Send("R;")
	end
	local onDisconnected = function()
		if(timer.Exists("physpult_update")) then timer.Remove("physpult_update") end
	end

	PhysPult.Start(PhysPult.AcceptMessage, onConnected, onDisconnected)
end)

concommand.Add("pult_stop", PhysPult.Stop)

end)