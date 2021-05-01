#include "mmu.h"
#include "sstream"

Mmu::Mmu(int memory_size)
{
    _next_pid = 1024;
    _max_size = memory_size;
}

Mmu::~Mmu()
{
}

uint32_t Mmu::createProcess()
{
    Process *proc = new Process();
    proc->pid = _next_pid;

    Variable *var = new Variable();
    var->name = "<FREE_SPACE>";
    var->type = DataType::FreeSpace;
    var->virtual_address = 0;
    var->size = _max_size;
    proc->variables.push_back(var);

    _processes.push_back(proc);

    _next_pid++;
    return proc->pid;
}

void Mmu::addVariableToProcess(uint32_t pid, std::string var_name, DataType type, uint32_t size, uint32_t address)
{
    int i;
    Process *proc = NULL;
    for (i = 0; i < _processes.size(); i++)
    {
        if (_processes[i]->pid == pid)
        {
            proc = _processes[i];
        }
    }

    Variable *var = new Variable();
    var->name = var_name;
    var->type = type;
    var->virtual_address = address;
    var->size = size;
    if (proc != NULL)
    {
        proc->variables.push_back(var);
    }
}

std::vector<Process*> Mmu::getAllProcesses(){
    return _processes;
};

std::vector<Variable*> Mmu::getVariablesByPid(int pid){
    Process *proc = NULL;
    for (int i = 0; i < _processes.size(); i++)
    {
        if (_processes[i]->pid == pid)
        {
            proc = _processes[i];
        }
    }
    return proc->variables;

}


void Mmu::printProcesses(){
    for (int i = 0; i < _processes.size(); i++){
        std::cout << _processes[i]->pid << "\n";
    }
}



void Mmu::terminateProcess(int pid){
    for (int i = 0; i < _processes.size(); i++){
        if (_processes[i]->pid == pid){
            _processes.erase(_processes.begin()+i);
        }
    }
}

void Mmu::print()
{
    int i, j;

    std::cout << " PID  | Variable Name | Virtual Addr | Size" << std::endl;
    std::cout << "------+---------------+--------------+------------" << std::endl;

    int pidSpacing = 5;
    int varNameSpacing = 14;
    int virtAddrSpacing = 12;
    int sizeSpacing = 10;

    for (i = 0; i < _processes.size(); i++)
    {
        for (j = 0; j < _processes[i]->variables.size(); j++)
        {
            // TODO: print all variables (excluding <FREE_SPACE> entries)

            if (_processes[i]->variables[j]->type != DataType::FreeSpace){
                int pidLength = std::to_string(_processes[i]->pid).size();
                int sizeLength = std::to_string(_processes[i]->variables[j]->size).size();

                std::cout << " " << _processes[i]->pid << std::string(pidSpacing-pidLength, ' ') << "|";
                std::cout << " " << _processes[i]->variables[j]->name << std::string(varNameSpacing-_processes[i]->variables[j]->name.size(), ' ') << "|";

                std::stringstream ss;
                ss << std::hex << std::uppercase << _processes[i]->variables[j]->virtual_address;
                std::string res (ss.str());
                int zeros = 8 - res.size();
                res = "0x" + std::string(zeros, '0') + res;
                std::cout << " " << std::string(virtAddrSpacing-res.size(), ' ') << res << " |";
                std::cout << " " << std::string(sizeSpacing-sizeLength, ' ') << _processes[i]->variables[j]->size << "\n";
            }
        }
    }
}
