#include <iostream>
#include <fstream>
#include <dirent.h>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

struct Process {
    int pid;
    int ppid;
    std::string name;
    long int memory_usage;

    Process(int p, int pp, const std::string &n, long int mem)
        : pid(p), ppid(pp), name(n), memory_usage(mem) {}
};

long int getMemoryUsage(int pid) {
    std::ifstream statm("/proc/" + std::to_string(pid) + "/statm");
    long int mem = 0;
    if (statm.is_open()) {
        statm >> mem;
    }
    return mem;
}

std::vector<Process> listProcesses() {
    std::vector<Process> processes;
    DIR *dir = opendir("/proc");
    struct dirent *entry;

    if (dir == nullptr) {
        std::cerr << "Error opening /proc directory." << std::endl;
        return processes;
    }

    while ((entry = readdir(dir)) != nullptr) {
        if (entry->d_type == DT_DIR && std::isdigit(entry->d_name[0])) {
            int pid = std::stoi(entry->d_name);
            std::ifstream statusFile("/proc/" + std::to_string(pid) + "/status");
            std::string line;
            int ppid = -1;
            std::string name;
            while (std::getline(statusFile, line)) {
                if (line.find("PPid") == 0) {
                    std::istringstream(line.substr(5)) >> ppid;
                }
                if (line.find("Name") == 0) {
                    name = line.substr(5);
                }
            }
            long int memory = getMemoryUsage(pid);
            processes.push_back(Process(pid, ppid, name, memory));
        }
    }
    closedir(dir);
    return processes;
}

void killProcess(int pid) {
    if (kill(pid, SIGTERM) == 0) {
        std::cout << "Process " << pid << " terminated." << std::endl;
    } else {
        std::cerr << "Failed to terminate process " << pid << std::endl;
    }
}

int main() {
    std::vector<Process> processes = listProcesses();

    std::cout << "Active Processes: " << std::endl;
    for (const auto &p : processes) {
        std::cout << "PID: " << p.pid << ", PPID: " << p.ppid << ", Name: " << p.name << ", Memory Usage: " << p.memory_usage << std::endl;
    }

    // Sort processes by memory usage in descending order
    std::sort(processes.begin(), processes.end(), [](const Process &a, const Process &b) {
        return a.memory_usage > b.memory_usage;
    });

    std::cout << "\nProcesses sorted by memory usage (descending):" << std::endl;
    for (const auto &p : processes) {
        std::cout << "PID: " << p.pid << ", PPID: " << p.ppid << ", Name: " << p.name << ", Memory Usage: " << p.memory_usage << std::endl;
    }

    // Example: Killing a process (use an actual PID you want to terminate)
    int pidToKill;
    std::cout << "\nEnter PID to kill: ";
    std::cin >> pidToKill;
    killProcess(pidToKill);

    return 0;
}
