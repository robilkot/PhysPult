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

-- Список индикаторов с соответствующим номером бита. Биты 1-12, 15-16 зарезервированы под скорость.
local indicators = {
	-- 2 Блок
	["KVC"] = 14,
	["AR0"] = 20,
	["AR04"] = 19, 
	["AR40"] = 23, 
	["AR60"] = 17,
	["AR70"] = 24, 
	["AR80"] = 18,
	["SD"] = 27, 
	["VD"] = 25, 
	["RP"] = 28, 
	["SN"] = 13,
	["HRK"] = 32, 
	["KVD"] = 21, 
	["ST"] = 30, 
	--["???"] = 29, -- ДВ
	["KT"] = 22,
	["LN"] = 31,
	["GLIB"] = 26, -- ЛЭКК

	-- 5-6 Блоки
	["GreenRP"] = 33,
	-- ["DoorsLeftL"] = ???,
	["L1"] = 40,
	["LSP"] = 39,
	["AVU"] = 38,
	["LKVP"] = 37,
	-- ["RZP"] = ???,

	-- 7 Блок
	--["???"] = ???, -- Контроль печи
	["PN"] = 34,
	-- ["DoorsRightR"] = ???,
}

-- Список тумблеров с соответствующим номером бита.
-- [Номер бита] = { "Имя тумблера", инвертировать ли (boolean) } 
local switches = {
	-- 1 блок
	[33] = { "VMKToggle" },
	[34] = { "BPSNonToggle" },

	-- 5 блок
	[5] = { "R_UNchToggle" },
	[6] = { "R_ZSToggle" },
	[7] = { "R_GToggle" },
	[8] = { "R_RadioToggle" },
	[13] = { "VUD1Toggle" },
	-- [11] = { "R_VPRToggle" },
	-- [15] = { "DoorSelectToggle" },

	-- 6 блок
	[20] = { "V13Toggle" },
	[17] = { "V11Toggle" },
	[19] = { "V12Toggle" },

	-- [18] = { "OtklAVUToggle"},
	--[22] = "???", -- Двери торцевые
	--[23] = "???", -- Вентиляция кабины
	[9] = { "ARSToggle" },
	[10] = { "ALSToggle" },
	[26] = { "ARSRToggle" },
	[11] = { "OVTToggle" } ,
	-- [34] = { "ALSFreqToggle" },
	[35] = { "L_1Toggle" },
	[31] = { "L_2Toggle" }, 
	-- [30] = { "L_3Toggle" },
	-- [35] = { "VPToggle" },
	
	-- 7 блок
	[25] = { "L_4Toggle" },
	[26] = { "VUSToggle" }, 
	[27] = { "VADToggle" },
	[28] = { "VAHToggle" },
}

-- Список кнопок с соответствующим номером бита.
-- [Номер бита] = { "Имя кнопки", инвертировать ли (boolean) } 
local buttons = {
	-- 1 блок
	[37] = { "RezMKSet", true },
	-- [36] = { "ARS13Set", false },

	-- 5 блок
	[4] = { "R_Program1Set", false },
	[3] = { "R_Program2Set", false },
	[15] = { "KRZDSet", false },
	[14] = { "VozvratRPSet", false },
	-- [1] =  { "KDLSet", false }, -- обрабатываются особым образом ниже
	-- [2] =  { "KDLSet", false }, 

	-- 6 блок
	[22] = {  "1:KVTSet", false }, --21
	-- [22] = {  "1:KVTRSet", false }, --24
	[18] = {  "VZ1Set", true },
	[23] = { "OtklBVSet", false },
	[12] = { "ConverterProtectionSet", true },
	[29] = {  "KSNSet", false },
	[30] = {  "RingSet", false },

	-- 7 блок
	[38] = { "KRPSet", false },
	--[37] =  "KAHSet" ,
}

local kvinputs = {
	"KVSetX1B",
	"KVSetX2",
	"KVSetX3",
	"KVSet0",
	"KVSetT1B",
	"KVSetT1AB",
	"KVSetT2",
}

local kminputs = {
	"PneumaticBrakeSet1",
	"PneumaticBrakeSet2",
	"PneumaticBrakeSet3",
	"PneumaticBrakeSet4",
	"PneumaticBrakeSet5",
	"PneumaticBrakeSet6",
	"PneumaticBrakeSet7",
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
PhysPult.GameKVPosition = 4

-- Интверал между обновлениями состояния (мс).
PhysPult.UpdateInterval = 20

-- Синхронизация индикаторов в поезде.
function PhysPult.SynchronizeIndicators(train)
	-- todo: R когда индикаторы не изменились
	local msg = "W;"

	local speed = math.floor(train:GetPackedRatio("speed") * 100)

	local pressureTM = math.floor(train:GetPackedRatio("BLPressure") * 100 * 8 / 5 * 1.43)

	local pressureNM = math.floor(train:GetPackedRatio("TLPressure") * 100 * 8 / 5 * 1.63)
	
	local pressureBC = math.floor(train:GetPackedRatio("BCPressure") * 100 * 5 / 9 * 3.15)
	
	local batteryVoltage = math.floor(train:GetPackedRatio("BatteryVoltage") * 100 * 1.6 * 1.7)

	local supplyVoltage = math.floor(train:GetPackedRatio("EnginesVoltage") * 100 * 2.17)
	
	local enginesCurrent = -1 * math.Clamp(math.floor((train:GetPackedRatio("EnginesCurrent") * 1000 - 500) * 0.6), -255, 255)
	
	msg = msg..speed..','
	msg = msg..pressureTM..','
	msg = msg..pressureNM..','
	msg = msg..pressureBC..','
	msg = msg..batteryVoltage..','
	msg = msg..supplyVoltage..','
	msg = msg..enginesCurrent..';'

	-- chat.AddText(supplyVoltage..' - '..enginesCurrent)

	local registers = {
		0, 0, 0, 0, 0, 0, 0
	}

	for k, v in pairs(indicators) do
		local bitIndex = v

		local bitInRegister = 7 - (bitIndex - 1) % 8
		local registerNumber = math.floor((bitIndex - 1)/ 8) + 1
		
		local indicatorState = Metrostroi.GetTrainIndicatorStage(train, k)

		if(indicatorState == true) then
			registers[registerNumber] = bit.bor(registers[registerNumber], bit.lshift(1, bitInRegister))
		else
			registers[registerNumber] = bit.band(registers[registerNumber], bit.bnot(bit.lshift(1, bitInRegister)))
		end
	end

	table.foreach(registers, function(key, value)
		msg = msg..value..','
	end)

	msg = string.SetChar(msg, -1, ';')

	PhysPult.SocketWrtData = msg
end

-- Синхронизация тумблреров в поезде.
function PhysPult.SynchronizeSwitches(train)
	local substrings = string.Explode(';', PhysPult.SocketRecData)
	local registers = string.Explode(',', substrings[3])

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
function PhysPult.SynchronizeButtons(train)
	local substrings = string.Explode(';', PhysPult.SocketRecData)
	local registers = string.Explode(',', substrings[3])

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
	local kdlOneState = bit.band(1, bit.rshift(tonumber(kdlRegister), 7)) == 1
	local kdlTwoState = bit.band(1, bit.rshift(tonumber(kdlRegister), 6)) == 1

	if(kdlOneState or kdlTwoState) then
		Metrostroi.SetTrainSwitchStage(train, "DoorSelectToggle", false)
		Metrostroi.DownTrainButton(train, "KDLSet")
	else
		Metrostroi.UpTrainButton(train, "KDLSet")
	end

	local kpdRegister = registers[6]
	local kpdState = bit.band(1, bit.rshift(tonumber(kpdRegister), 2)) == 1

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
	-- ["013"] = {

	-- }
}

function PhysPult.SynchronizeControllerAndCrane(train)
	-- todo: explode strings in separate method, not three times in these methods
	local substrings = string.Explode(';', PhysPult.SocketRecData)
	local numerics = string.Explode(',', substrings[2])
	
	local realPosition = 255 - tonumber(numerics[1])
	local positions = CRANE_POSITIONS["334"]

	local prevPosition = PhysPult.GameCranePosition

	for k, v in pairs(positions) do
		if(realPosition > v) then continue end
		
		local median = positions[k]

		if (k > 1) then
			median = (median + positions[k - 1]) / 2

			if(realPosition - median > 0) then
				PhysPult.GameCranePosition = k
			else
				PhysPult.GameCranePosition = k - 1
			end
		end
		
		break
	end

	Metrostroi.DownTrainButton(train, { ["ID"] = kminputs[PhysPult.GameCranePosition] })
	if(PhysPult.GameCranePosition != prevPosition) then
		Metrostroi.UpTrainButton(train, { ["ID"] = kminputs[prevPosition] })
	end
	
	-- chat.AddText(tostring(PhysPult.GameCranePosition), " ", tostring(realPosition))
end

-- Синхронизация физического пульта, с виртуальным пультом поезда, в котром сидит игрок.
function PhysPult.Synchronize()
	local train = getPlayerDrivenTrain()
	
	if(train and checkTrainType(train)) then
		if(PhysPult.SocketRecData) then 
			--PhysPult.SynchronizeSwitches(train)
			--PhysPult.SynchronizeButtons(train)
			PhysPult.SynchronizeControllerAndCrane(train)
		end

		PhysPult.SynchronizeIndicators(train)
	end
end

concommand.Add("physpult_start", function()

    timer.Create("ControlsSync", PhysPult.UpdateInterval / 1000, 0, function()
		PhysPult.Synchronize()
	end)

    PhysPult.Socket:close()
    PhysPult.Socket:open()
end)

concommand.Add("physpult_stop", function()
	if(timer.Exists("ControlsSync")) then timer.Remove("ControlsSync") end
    
    PhysPult.Socket:close()
end)


end)