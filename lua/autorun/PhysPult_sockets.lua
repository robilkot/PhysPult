if(SERVER) then return end

PhysPult = PhysPult or {}

local url = "ws://192.168.1.41:80"

require("gwsockets")
PhysPult.Socket = GWSockets.createWebSocket(url)

function PhysPult.Socket:onMessage(txt)
    -- chat.AddText("Received: ", txt)
    PhysPult.SocketRecData = txt
end

function PhysPult.Socket:onError(txt)
    chat.AddText("Error: ", txt)
end

function PhysPult.Socket:onConnected()
    chat.AddText("PhysPult: Connected to device.")

    timer.Create("PhysPultUpdate", PhysPult.UpdateInterval / 1000, 0, function()
        if(PhysPult.SocketWrtData) then
            PhysPult.Socket:write(PhysPult.SocketWrtData)
        end
	end)
end

function PhysPult.Socket:onDisconnected()
    if(timer.Exists("PhysPultUpdate")) then timer.Remove("PhysPultUpdate") end
    
    chat.AddText("PhysPult: Disconnected.")
end