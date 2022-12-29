-----------------------------------------------------------------------------------------
--                          Творческое объединение "MetroPack"
--	Скрипт написан в 2022 году для тренажёра 81-717.5 на основе Arduino UNO.
--	Инициализация рабочих файлов для тренажёра.
--	Автор: 	robilkot
--	Steam: 	https://steamcommunity.com/id/metropacker/
--	VK:		https://vk.com/robilkot
--  Дополнительная информация в файле lua/licence.lua
-----------------------------------------------------------------------------------------

if(not SERVER) then return end

timer.Simple(1, function()

local Trains = {
    "gmod_subway_81-717_mvm"
}

--local system = Metrostroi.Systems["_ALS_ARS_D"]
--local m_Think = system.Think

for _, train in ipairs(Trains) do
	local ENT = scripted_ents.GetStored(train).t
	local m_Initialize = ENT.Initialize 
	function ENT:Initialize()
		local retVal = m_Initialize(self)

		--- Initialize variables in ALS.Initialize
		self.ALS_ARS.lsn_last_value = 0
		self.ALS_ARS.lsn_prelast_value = 0
		
		self.ALS_ARS.rp_last_value = 0
		self.ALS_ARS.rp_prelast_value = 0
			
		self.ALS_ARS.kvc_last_value = 0
		self.ALS_ARS.kvc_prelast_value = 0
			
		self.ALS_ARS.filename_indicators = "lamps.txt"
		---

		local m_Think = self.ALS_ARS.Think -- Redefining think function in ALS system
		self.ALS_ARS.Think = function()
			local self = self.ALS_ARS
			local retVal = m_Think(self)
			
			--- OUR CODE HERE

			local Panel = self.Train.Panel

			---- WRITING INDICATORS' STATE

			local output_indicators = "0000000000000000000000000000000000000000000000000000000000000000" -- String to store indicators state (64)

			if Panel.SD > 0 then
			output_indicators = string.SetChar(output_indicators, 6+1, '1') -- LSD -- in every line we add 1 because of indexing starting from 1, not 0
			end	
			
			if Panel.LVD > 0 then
			output_indicators = string.SetChar(output_indicators, 7+1, '1') --LVD
			end
			
			if Panel.LhRK > 0 then
			output_indicators = string.SetChar(output_indicators, 10+1, '1') --LhRK
			end
			
			if Panel.LST > 0 then
			output_indicators = string.SetChar(output_indicators, 12+1, '1') --LST
			end
			
			if Panel.LN > 0 then
			output_indicators = string.SetChar(output_indicators, 14+1, '1') --LN
			end
			
			if Panel.LKVD > 0 then 
			output_indicators = string.SetChar(output_indicators, 11+1, '1') --LKVD 
			end
			
			if Panel.KT > 0 then 
			output_indicators = string.SetChar(output_indicators, 13+1, '1') --LKT 
			end
			
			if self.KVC_last_value ~= 0 or self.KVC_prelast_value ~= 0 or Panel.KVC ~= 0 then
				output_indicators = string.SetChar(output_indicators, 16+1, '1') --LKVC
			end
			self.KVC_prelast_value = self.KVC_last_value
			self.KVC_last_value = Panel.KVC
			
			if self.LSN_last_value ~= 0 or self.LSN_prelast_value ~= 0 or Panel.LSN ~= 0 then
				output_indicators = string.SetChar(output_indicators, 9+1, '1') --LSN
			end
			self.LSN_prelast_value = self.LSN_last_value
			self.LSN_last_value = Panel.LSN
			
			--print(output_indicators)
			file.Write (self.filename_indicators, output_indicators)
	
			---
			return retVal
		end		

		return retVal
	end
end

end) 