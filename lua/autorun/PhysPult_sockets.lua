if(SERVER) then return end

PhysPult = PhysPult or {}

local socket = require("socket.core") or socket

local function clientHanlder(tcpClient)
    timer.Pause("tcp-server-physpult")

    if file.Exists("metrostroi_data/physpult.txt", "DATA") then
        local paramsString = file.Read("metrostroi_data/physpult.txt", "DATA")
        local params = string.Split(paramsString, " ")
        
        PhysPult.SocketPort = tonumber(params[1])
        PhysPult.UpdateFrequency = tonumber(params[2])
        PhysPult.IndicatorsNumber = tonumber(params[3])
        PhysPult.SwitchesNumber = tonumber(params[4])
    else
        chat.AddText("PhysPult: Couldn't load config for current connection! Using default parameters.")
    end

    local timerName = "tcp-client-handler"..tcpClient:getsockname()
    local errorCount = 0

    tcpClient:send(PhysPult.SocketWrtData) -------
    tcpClient:settimeout(0.01)

    timer.Create(timerName, 1 / PhysPult.UpdateFrequency, 0, function()
        tcpClient:send(PhysPult.SocketWrtData.."\0")
        chat.AddText("wrt"..PhysPult.SocketWrtData)

        local data, error = tcpClient:receive(PhysPult.SwitchesNumber + 1)

        if (data) then
            errorCount = 0
            chat.AddText("rec"..data)
            PhysPult.SocketRecData = data
        else 
            errorCount = errorCount + 1
            chat.AddText("rec failed:"..error)
            if(errorCount == 3) then 
                tcpClient:close()
                PhysPult.SocketRecData = nil
                chat.AddText("PhysPult: Client disconnected!")
                timer.UnPause("tcp-server-physpult")
                timer.Remove(timerName)
            end
        end
    end) 
end

----   API definition   ----

function PhysPult.StartServer(host, port)
    local tcpServer = socket.tcp4()

    tcpServer:settimeout(0.01)
    tcpServer:setoption("reuseaddr", true)
    tcpServer:bind(host, port)
    tcpServer:listen(2)

    timer.Create("tcp-server-physpult", 0.1, 0, function()
        PhysPult.tcpClient = tcpServer:accept()

        if (PhysPult.tcpClient) then
            chat.AddText("PhysPult: Client connected!")
            clientHanlder(PhysPult.tcpClient)
        end
    end)
end