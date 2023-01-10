if(SERVER) then return end

local socket = require("socket.core") or socket

----   API definition   ----

if(not PhysPult) then PhysPult = {} end

local function clientHanlder(tcpClient)
    local timerName = "tcp-client-handler"..tcpClient:getsockname()
    local timerInterval = 1 / PhysPult.UpdateFrequency
    local dataTimeout = 0

    tcpClient:settimeout(0.01)

    timer.Create(timerName, timerInterval, dataTimeout / timerInterval, function()
        tcpClient:send(PhysPult.SocketWrtData)
        chat.AddText("wrt"..PhysPult.SocketWrtData)

        local data, error = tcpClient:receive(PhysPult.SwitchesNumber + 1)

        if (error) then
            chat.AddText("rec failed:"..error)
        end
    
        if (data) then
            chat.AddText("rec"..data)
            PhysPult.SocketRecData = data
        end
        
    end) 
end

function PhysPult.StartServer(host, port)
    local tcpServer = socket.tcp4()

    tcpServer:settimeout(0.01)
    tcpServer:setoption("reuseaddr", true)
    tcpServer:bind(host, port)
    tcpServer:listen(2)

    chat.AddText("Server start.")

    timer.Create("tcp-server-"..port, 1, 0, function()
        PhysPult.tcpClient = tcpServer:accept()

        if (PhysPult.tcpClient) then
            chat.AddText("Client connected!")
            clientHanlder(PhysPult.tcpClient)
        end
    end)
end