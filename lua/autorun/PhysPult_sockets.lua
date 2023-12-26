if(SERVER) then return end

PhysPult = PhysPult or {}

local url = "ws://192.168.1.41:80"
-- local url = "ws://echo.websocket.events/"
require("gwsockets")
local socket = GWSockets.createWebSocket(url)

<<<<<<< Updated upstream
local function clientHanlder(tcpClient)
    timer.Pause("tcp-server-physpult")

    if file.Exists("metrostroi_data/physpult.txt", "DATA") then
        local paramsString = file.Read("metrostroi_data/physpult.txt", "DATA")
        local params = string.Split(paramsString, " ")
        
        PhysPult.SocketPort = tonumber(params[1])
        PhysPult.UpdateInterval = tonumber(params[2])
        PhysPult.IndicatorsNumber = tonumber(params[3])
        PhysPult.SwitchesNumber = tonumber(params[4])
    else
        chat.AddText("PhysPult: Couldn't load config for current connection! Using default parameters.")
    end

    local timerName = "tcp-client-handler"..tcpClient:getsockname()
    local errorCount = 0

    if(PhysPult.SocketWrtData) then tcpClient:send(PhysPult.SocketWrtData) end
    tcpClient:settimeout(0.01)

    timer.Create(timerName, PhysPult.UpdateInterval / 1000, 0, function()
        tcpClient:send(PhysPult.SocketWrtData.."\0")
        --chat.AddText("wrt"..PhysPult.SocketWrtData)

        local data, error = tcpClient:receive(PhysPult.SwitchesNumber + 1)

        if (data) then
            errorCount = 0
            --chat.AddText("rec"..data)
            PhysPult.SocketRecData = data
        else 
            errorCount = errorCount + 1
            --chat.AddText("rec failed: "..error)
            if(errorCount == 3) then 
                tcpClient:close()
                PhysPult.SocketRecData = nil
                chat.AddText("PhysPult: Client disconnected!")
                timer.UnPause("tcp-server-physpult")
                timer.Remove(timerName)
            end
        end
    end) 
=======
function socket:onMessage(txt)
    chat.AddText("Received: ", txt)
>>>>>>> Stashed changes
end

function socket:onError(txt)
    chat.AddText("Error: ", txt)
end

-- We start writing only after being connected here. Technically this is not required as this library
-- just waits until the socket is connected before sending, but it's probably good practice
function socket:onConnected()
    chat.AddText("Connected to server")
    -- Write Echo once every second, 10 times
    timer.Create("SocketWriteTimer", 1, 0, function()
        chat.AddText("Writing: ", "Echo")
        socket:write("Echo")
    end)
    
    timer.Simple(10, function()
        timer.Remove("SocketWriteTimer")
        -- Even if some of the messages have not reached the other side yet, this type of close makes sure
        -- to only close the socket once all queued messages have been received by the peer.
        socket:close()
    end)
end

function socket:onDisconnected()
    chat.AddText("WebSocket disconnected")
end

timer.Simple(1, function()
    socket:close()
    socket:open()
end)



concommand.Add("gws", function(ply)
	-- if ply:IsValid() and not ply:IsAdmin() then return end
	chat.AddText('PhysPult: reopen socket')
    
    socket:close()
    -- socket:open()
	-- socket:write("test")
end)

-- local function clientHanlder(tcpClient)
--     timer.Pause("tcp-server-physpult")

--     if file.Exists("metrostroi_data/physpult.txt", "DATA") then
--         local paramsString = file.Read("metrostroi_data/physpult.txt", "DATA")
--         local params = string.Split(paramsString, " ")
        
--         -- Update connection parameters received from c++ program
--         PhysPult.SocketPort = tonumber(params[1])
--         PhysPult.UpdateInterval = tonumber(params[2])
--         PhysPult.IndicatorsNumber = tonumber(params[3])
--         PhysPult.SwitchesNumber = tonumber(params[4])
--     else
--         chat.AddText("PhysPult: Couldn't load config for current connection! Using default parameters.")
--     end

--     local timerName = "tcp-client-handler"..tcpClient:getsockname()
--     local errorCount = 0

--     -- handShake(tcpClient)

--     -- if(PhysPult.SocketWrtData) then tcpClient:send(PhysPult.SocketWrtData) end
--     tcpClient:settimeout(0.01)

--     timer.Create(timerName, PhysPult.UpdateInterval / 1000, 0, function()
--         tcpClient:send(PhysPult.SocketWrtData.."\0")
--         --chat.AddText("wrt"..PhysPult.SocketWrtData)

--         local data, error = tcpClient:receive(PhysPult.SwitchesNumber + 1)

--         if (data) then
--             errorCount = 0
--             chat.AddText("rec"..data)
--             PhysPult.SocketRecData = data
--         else 
--             errorCount = errorCount + 1
--             chat.AddText("rec failed: "..error)
--             if(errorCount == 3) then 
--                 tcpClient:close()
--                 PhysPult.SocketRecData = nil
--                 chat.AddText("PhysPult: Client disconnected!")
--                 timer.UnPause("tcp-server-physpult")
--                 timer.Remove(timerName)
--             end
--         end
--     end) 
-- end

-- function handShake(tcpClient)
--     local data = tcpClient:receive(150)
--     -- chat.AddText("data: "..data)

--     local keyStart, keyEnd = string.find(data, "Sec-WebSocket-Key: ", 1, true)
--     local WebSocketKey = string.sub(data, keyEnd + 1, keyEnd + 1 + 24)
--     -- chat.AddText("key: "..WebSocketKey)

--     local magicString = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"
    
--     local acceptKey = WebSocketKey..magicString
--     acceptKey = util.SHA1(acceptKey)
    
--     acceptKey = util.Base64Encode(acceptKey, true)
    
--     local responce =
--     "HTTP/1.1 101 Switching Protocols\n\rUpgrade: websocket\n\rConnection: Upgrade\n\rSec-WebSocket-Accept: "
    
--     responce = responce..acceptKey.."\n\r"

--     -- chat.AddText("accept key: "..acceptKey)

--     tcpClient:send(responce.."\0")
-- end

----   API definition   ----

function PhysPult.StartServer(host, port)
    -- local tcpServer = socket.tcp4()

    -- tcpServer:settimeout(0.01)
    -- tcpServer:setoption("reuseaddr", true)
    -- tcpServer:bind(host, port)
    -- tcpServer:listen(2)

    -- Нижнеидушая хрень для чтения из файла, временно
    -- if file.Exists("metrostroi_data/physpult.txt", "DATA") then
    --     local paramsString = file.Read("metrostroi_data/physpult.txt", "DATA")
    --     local params = string.Split(paramsString, " ")
        
    --     PhysPult.SocketPort = tonumber(params[1])
    --     PhysPult.UpdateInterval = tonumber(params[2])
    --     PhysPult.IndicatorsNumber = tonumber(params[3])
    --     PhysPult.SwitchesNumber = tonumber(params[4])
    -- else
    --     chat.AddText("PhysPult: Couldn't load config for current connection! Using default parameters.")
    -- end
    
    -- timer.Create("physpult_files", 1 / PhysPult.UpdateInterval, 0, function()
    --     file.Write("metrostroi_data/physpult_indicators.txt", PhysPult.SocketWrtData)
    --     --chat.AddText("wrt"..PhysPult.SocketWrtData)

    --     local data = file.Read("physpult_switches.txt", "DATA")
    --     PhysPult.SocketRecData = data
    -- end) 
    ---- ВОТ ПО СЮДА

    timer.Create("tcp-server-physpult", 0.1, 0, function()
        -- PhysPult.tcpClient = tcpServer:accept()

        if (PhysPult.tcpClient) then
            chat.AddText("PhysPult: Client connected!")
            -- clientHanlder(PhysPult.tcpClient)
        end
    end)
end