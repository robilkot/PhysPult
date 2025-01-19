if(SERVER) then return end

PhysPult = PhysPult or {}

local url = "ws://192.168.1.10:8080"

require("gwsockets")

function PhysPult.CreateSocket()
    if PhysPult.Socket then PhysPult.Socket:close() end
    socket = GWSockets.createWebSocket(url)

    function socket:onMessage(txt)
        PhysPult.SocketRecData = txt
    end
    
    function socket:onError(txt)
        chat.AddText("Error: ", txt)
    end
    
    function socket:onConnected()
        chat.AddText("PhysPult: Connected to device.")
    
        timer.Create("PhysPultUpdate", PhysPult.UpdateInterval / 1000, 0, function()
            if(PhysPult.Socket and PhysPult.SocketWrtData) then
                PhysPult.Socket:write(PhysPult.SocketWrtData)
            end
        end)
    end
    
    function socket:onDisconnected()
        if(timer.Exists("PhysPultUpdate")) then timer.Remove("PhysPultUpdate") end
        
        chat.AddText("PhysPult: Disconnected.")
    end

    PhysPult.Socket = socket
    PhysPult.Socket:open()
end

function PhysPult.CloseSocket()
    if(PhysPult.Socket) then PhysPult.Socket:close() end
    PhysPult.Socket = nil
end
