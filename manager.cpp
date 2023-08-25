#include <iostream>
#include <vector>
#include <windows.h>
#include <tlhelp32.h>
#include <shellapi.h>

struct Task {
    DWORD pid;
    std::string name;
    HICON icon;
};

std::vector<Task> getRunningTasks() {
    std::vector<Task> tasks;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if(hSnapshot) {
        PROCESSENTRY32 pe32;
        pe32.dwSize = sizeof(PROCESSENTRY32);
        if(Process32First(hSnapshot, &pe32)) {
            do {
                Task task;
                task.pid = pe32.th32ProcessID;
                task.name = pe32.szExeFile;
                SHFILEINFO sfi;
                if(SHGetFileInfo(task.name.c_str(), 0, &sfi, sizeof(sfi), SHGFI_ICON | SHGFI_LARGEICON)) {
                    task.icon = sfi.hIcon;
                }
                tasks.push_back(task);
            } while(Process32Next(hSnapshot, &pe32));
        }
        CloseHandle(hSnapshot);
    }
    return tasks;
}

void killTask(DWORD pid) {
    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
    if(hProcess) {
        TerminateProcess(hProcess, 0);
        CloseHandle(hProcess);
    }
}

int main() {
    std::vector<Task> tasks = getRunningTasks();
    for(const Task& task : tasks) {
        std::cout << "PID: " << task.pid << " Name: " << task.name << std::endl;
        displayIcon(task.icon); 
    }
    DWORD pidToKill;
    std::cout << "Enter the PID of the task to kill: ";
    std::cin >> pidToKill;
    killTask(pidToKill);
    std::cout << "Task with PID " << pidToKill << " has been terminated." << std::endl;
    
    return 0;
}
