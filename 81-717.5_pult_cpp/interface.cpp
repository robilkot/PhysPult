#include <iostream>
#include <string>
#include <list>
#include <Windows.h>

using namespace std;

list<int> GetCOMports()
{
    wchar_t lpTargetPath[5000]; // buffer to store the path of the COM PORTS
    list<int> portList;

    cout << "Available ports :\n";
    for (int i = 0; i < 255; i++) // checking ports from COM0 to COM255
    {
        wstring str = L"COM" + to_wstring(i); // converting to COM0, COM1, COM2
        DWORD res = QueryDosDevice(str.c_str(), lpTargetPath, 5000);

        if (res != 0) //QueryDosDevice returns zero if it didn't find an object
        {
            portList.push_back(i);
            string temp(str.begin(),str.end());
            cout << temp << " - " << lpTargetPath << endl;
        }
        if (::GetLastError() == ERROR_INSUFFICIENT_BUFFER) NULL;
    }
    cout << "\n";
    return portList;
}