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

-- Интервал обновления состояния индикаторов в секундах
local updateInterval = 0.1 
-- Путь к файлу где хранится строка состояния
local filenameIndicators = "lamps.txt" 
-- Соотвествие битов строки состояния лампам
local stateTable = { 
	--["speed"] = 0,
	--["speed"] = 1,
	["SD"] = 6,
	["VD"] = 7,
	["HRK"] = 10,
	["ST"] = 12,
	["LN"] = 14,
	["KVD"] = 11,
	["KT"] = 13,
	["SN"] = 9,
	["KVC"] = 16,
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

local function writeCurrentState(table, state, filename)
	local train = getPlayerDrivenTrain()

	if(not train) then return end
	if(not checkTrainType(train)) then return end

	for k, v in pairs(table) do
		if train:GetNW2Bool(k) == true then
			state = string.SetChar(state, v+1, '1')
		end
	end

	local speed = train:GetPackedRatio("speed")*100
	state = string.SetChar(state, 1, math.floor(speed/10))
	state = string.SetChar(state, 2, math.floor(speed)%10)

	--print(state)
	file.Write (filename, state)
end

local function startIndicatorsUpdate()
	local currentState = "0000000000000000000000000000000000000000000000000000000000000000" 

	timer.Create("stateStringUpdate", updateInterval, 0, function()
		writeCurrentState(stateTable, currentState, filenameIndicators)
	end)
end

startIndicatorsUpdate()