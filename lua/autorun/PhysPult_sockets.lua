if(SERVER) then return end

PhysPult = PhysPult or {}

local socket = require("socket.core") or socket

local function clientHanlder(tcpClient)
    timer.Pause("tcp-server-physpult")

    local timerName = "tcp-client-handler"..tcpClient:getsockname()
    --local dataTimeout = 0
    local errorCount = 0

    tcpClient:send(PhysPult.SocketWrtData)

    tcpClient:settimeout(0.01)

    timer.Create(timerName, 1 / PhysPult.UpdateFrequency, 0 --[[dataTimeout / timerInterval]] , function()
        tcpClient:send(PhysPult.SocketWrtData)
        chat.AddText("wrt"..PhysPult.SocketWrtData)

        local data, error = tcpClient:receive(PhysPult.SwitchesNumber + 1)

        if (error == "timeout") then
            errorCount = errorCount + 1
            --chat.AddText("rec failed:"..error)
            if(errorCount == 3) then 
                timer.Remove(timerName)
                tcpClient:close()
                PhysPult.SocketRecData = nil
                chat.AddText("Client disconnected!")
                timer.UnPause("tcp-server-physpult")
            end
        end
    
        if (data) then
            errorCount = 0
            chat.AddText("rec"..data)
            PhysPult.SocketRecData = data
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

    chat.AddText("Server start.")

    timer.Create("tcp-server-physpult", 0.1, 0, function()
        PhysPult.tcpClient = tcpServer:accept()

        if (PhysPult.tcpClient) then
            chat.AddText("Client connected!")
            clientHanlder(PhysPult.tcpClient)
        end
    end)
end