if(SERVER) then return end

PhysPult = PhysPult or {}

local url = "ws://127.0.0.1:8080/ws"

require("gwsockets")

function PhysPult.Send(msg)
    if(PhysPult.Socket) then
        PhysPult.Socket:write(msg) 
        chat.AddText("snt ", msg)
    end
end

function PhysPult.Start(onMessage, onConnected, onDisconnected)
    if PhysPult.Socket then PhysPult.Socket:close() end
    socket = GWSockets.createWebSocket(url)

    function socket:onMessage(txt)
        chat.AddText("rec ", tostring(txt))
        onMessage(txt)
    end
    
    function socket:onError(txt)
        chat.AddText("Error: ", txt)
    end
    
    function socket:onConnected()
        chat.AddText("PhysPult: Connected.")
        onConnected()
    end
    
    function socket:onDisconnected()
        chat.AddText("PhysPult: Disconnected.")
        onDisconnected()
    end

    PhysPult.Socket = socket
    PhysPult.Socket:open()
end

function PhysPult.Stop()
    if(PhysPult.Socket) then PhysPult.Socket:close() end
    PhysPult.Socket = nil
end
