#include <iostream>
#include <string>
#include <cstring>
#include "mmu.h"
#include "pagetable.h"

#include <cmath>

void printStartMessage(int page_size);
void createProcess(int text_size, int data_size, Mmu *mmu, PageTable *page_table);
void allocateVariable(uint32_t pid, std::string var_name, DataType type, uint32_t num_elements, Mmu *mmu, PageTable *page_table);
void setVariable(uint32_t pid, std::string var_name, uint32_t offset, void *value, Mmu *mmu, PageTable *page_table, void *memory);
void freeVariable(uint32_t pid, std::string var_name, Mmu *mmu, PageTable *page_table);
void terminateProcess(uint32_t pid, Mmu *mmu, PageTable *page_table);
void splitString(std::string text, char d, std::vector<std::string>& result);


int main(int argc, char **argv)
{
    // Ensure user specified page size as a command line parameter
    if (argc < 2)
    {
        fprintf(stderr, "Error: you must specify the page size\n");
        return 1;
    }

    // Print opening instuction message
    int page_size = std::stoi(argv[1]);
    printStartMessage(page_size);

    // Create physical 'memory'
    uint32_t mem_size = 67108864;

    void *memory = malloc(mem_size); // 64 MB (64 * 1024 * 1024)

    // Create MMU and Page Table
    Mmu *mmu = new Mmu(mem_size);
    PageTable *page_table = new PageTable(page_size);

    // Prompt loop
    std::string command;
    std::cout << "> ";
    std::getline (std::cin, command);
    while (command != "exit") {
        // Handle command
        // TODO: implement this!
        std::vector<std::string> listOfArgs;

        splitString(command, ' ', listOfArgs);

        command = listOfArgs[0];

        if (command == "create"){
        
            createProcess(stoi(listOfArgs[1]), stoi(listOfArgs[2]), mmu, page_table);

        } else if (command == "allocate") {

            DataType type;
            if (listOfArgs[3] == "char"){
                type = DataType::Char;
            } else if(listOfArgs[3] == "short"){
                type = DataType::Short;
            } else if (listOfArgs[3] == "float"){
                type = DataType::Float;
            } else if (listOfArgs[3] == "int"){
                type = DataType::Int;
            } else if (listOfArgs[3] == "double"){
                type = DataType::Double;
            } else if (listOfArgs[3] == "long"){
                type = DataType::Long;
            } else {
                //error
            }
            allocateVariable(stoi(listOfArgs[1]), listOfArgs[2], type, stoi(listOfArgs[4]), mmu, page_table);
        } else if (command == "set") {
            //0 1 2 3 are separate. merge all the other inputs into a length....

            // for (int i = 4; i < listOfArgs.size(); i++){
            //     setVariable(stoi(listOfArgs[1]), listOfArgs[2], 0, &listOfArgs[i], mmu, page_table, memory);
            // }            
            bool variableExists = false;
            for (int i = 0; i < mmu->getAllProcesses().size(); i++){
                if (mmu->getAllProcesses()[i]->pid == stoi(listOfArgs[1])){
                    for (int j = 0; j < mmu->getAllProcesses()[i]->variables.size(); j++){
                        if (mmu->getAllProcesses()[i]->variables[j]->name == listOfArgs[2]){
                            variableExists = true;
                            break;
                        }
                        if (j == mmu->getAllProcesses()[i]->variables.size()-1){
                            std::cout << "error: variable not found.\n";
                        }
                    }
                }
                if (variableExists == true){
                    break;
                }
            }

            bool pidExists = false;
            for (int i = 0; i < mmu->getAllProcesses().size(); i++){
                if (mmu->getAllProcesses()[i]->pid == stoi(listOfArgs[1])){
                    pidExists = true;
                    break;
                }
                if (i == mmu->getAllProcesses().size()-1){
                    std::cout << "error: process not found.\n";
                }
            }

            if (variableExists == true && pidExists == true){
                for (int i = 0; i < mmu->getAllProcesses().size(); i++){
                    if (mmu->getAllProcesses()[i]->pid == stoi(listOfArgs[1])){
                        for (int j = 0; j < mmu->getAllProcesses()[i]->variables.size(); j++){
                            Variable* currentVar = mmu->getAllProcesses()[i]->variables[j];
                            int offset = stoi(listOfArgs[3]);
                            if (currentVar->name == listOfArgs[2]){
                                if (currentVar->type == DataType::Char){
                                    std::string string = "";
                                    for (int i = 4; i < listOfArgs.size(); i++){
                                        if (i == listOfArgs.size()-1){
                                            string = string + listOfArgs[i].c_str();
                                        }
                                        string = string + listOfArgs[i].c_str() + " ";
                                    }
                                    for (int i = 0; i < string.size(); i++) {
                                        char character = string.at(i);
                                        setVariable(stoi(listOfArgs[1]), listOfArgs[2], offset, &character, mmu, page_table, memory);
                                        offset++;
                                    }
                                } else if (currentVar->type == DataType::Short){
                                    short value;
                                    for (int i = 4; i < listOfArgs.size(); i++){
                                        value = (short)stoi(listOfArgs[i]);
                                        setVariable(stoi(listOfArgs[1]), listOfArgs[2], offset*2, &value, mmu, page_table, memory);
                                    }
                                } else if (currentVar->type == DataType::Int){
                                    int value;
                                    for (int i = 4; i < listOfArgs.size(); i++){
                                        value = stoi(listOfArgs[i]);
                                        setVariable(stoi(listOfArgs[1]), listOfArgs[2], offset*4, &value, mmu, page_table, memory);
                                    }                                
                                } else if (currentVar->type == DataType::Float){
                                    float value;
                                    for (int i = 4; i < listOfArgs.size(); i++){
                                        value = stof(listOfArgs[i]);
                                        setVariable(stoi(listOfArgs[1]), listOfArgs[2], offset*4, &value, mmu, page_table, memory);
                                    }                                      
                                } else if (currentVar->type == DataType::Long){
                                    long value;
                                    for (int i = 4; i < listOfArgs.size(); i++){
                                        value = stol(listOfArgs[i]);
                                        setVariable(stoi(listOfArgs[1]), listOfArgs[2], offset*8, &value, mmu, page_table, memory);
                                    }                                   
                                } else if (currentVar->type == DataType::Double){
                                    double value;
                                    for (int i = 4; i < listOfArgs.size(); i++){
                                        value = stod(listOfArgs[i]);
                                        setVariable(stoi(listOfArgs[1]), listOfArgs[2], offset*8, &value, mmu, page_table, memory);
                                    }                                    
                                } else {
                                    //error
                                }
                            }
                        }
                    }
                }
            }

            //setVariable(stoi(listOfArgs[1]), listOfArgs[2], 0, &listOfArgs[4], mmu, page_table, memory);
        } else if (command == "free") {
            freeVariable(stoi(listOfArgs[1]), listOfArgs[2], mmu, page_table);
        } else if (command == "terminate") {
            terminateProcess(stoi(listOfArgs[1]), mmu, page_table);
        } else if (command == "print") {
            if (listOfArgs[1] == "mmu"){
                mmu->print();
            } else if (listOfArgs[1] == "page"){
                page_table->print();
            } else if(listOfArgs[1] == "processes"){
                mmu->printProcesses();
            } else {
                std::vector<std::string> varDetails;
                splitString(listOfArgs[1], ':', varDetails);
                std::vector<Variable*> vars = mmu->getVariablesByPid(stoi(varDetails[0]));
                for (int i = 0; i < vars.size(); i++){
                    if (vars[i]->name == varDetails[1]){
                        if (vars[i]->type == DataType::Char){
                            int valuesAmount = vars[i]->size;
                            char values[valuesAmount];
                            for (int i = 0; i < valuesAmount; i++){
                                memcpy(values+i, page_table->getPhysicalAddress(stoi(varDetails[0]), vars[i]->virtual_address + i) + (char*)memory, 1);
                            }
                            if (valuesAmount > 4){
                                for (int i = 0; i < 4; i++){
                                    std::cout << values[i] << ", ";
                                }
                                std::cout << "... [" << vars[i]->size << " items]";
                            } else {
                                for (int i = 0; i < valuesAmount; i++){
                                    if (i < valuesAmount-1){
                                        std::cout << values[i] << ", ";
                                    } else {
                                        std::cout << values[i];
                                    }
                                }
                            }
                        } else if (vars[i]->type == DataType::Short){

                        } else if (vars[i]->type == DataType::Int){

                        } else if (vars[i]->type == DataType::Float){

                        } else if (vars[i]->type == DataType::Long){

                        } else if (vars[i]->type == DataType::Double){

                        }
                    }
                }
            }
        } else {
            printf("error: command not recognized\n");
        }


        // Get next command
        std::cout << "> ";
        std::getline (std::cin, command);
    }

    // Cean up
    free(memory);
    delete mmu;
    delete page_table;

    return 0;
}

void printStartMessage(int page_size)
{
    std::cout << "Welcome to the Memory Allocation Simulator! Using a page size of " << page_size << " bytes." << std:: endl;
    std::cout << "Commands:" << std:: endl;
    std::cout << "  * create <text_size> <data_size> (initializes a new process)" << std:: endl;
    std::cout << "  * allocate <PID> <var_name> <data_type> <number_of_elements> (allocated memory on the heap)" << std:: endl;
    std::cout << "  * set <PID> <var_name> <offset> <value_0> <value_1> <value_2> ... <value_N> (set the value for a variable)" << std:: endl;
    std::cout << "  * free <PID> <var_name> (deallocate memory on the heap that is associated with <var_name>)" << std:: endl;
    std::cout << "  * terminate <PID> (kill the specified process)" << std:: endl;
    std::cout << "  * print <object> (prints data)" << std:: endl;
    std::cout << "    * If <object> is \"mmu\", print the MMU memory table" << std:: endl;
    std::cout << "    * if <object> is \"page\", print the page table" << std:: endl;
    std::cout << "    * if <object> is \"processes\", print a list of PIDs for processes that are still running" << std:: endl;
    std::cout << "    * if <object> is a \"<PID>:<var_name>\", print the value of the variable for that process" << std:: endl;
    std::cout << std::endl;
}

void createProcess(int text_size, int data_size, Mmu *mmu, PageTable *page_table)
{
    // TODO: implement this!
    //   - create new process in the MMU
    //   - allocate new variables for the <TEXT>, <GLOBALS>, and <STACK>
    //   - print pid
    int pid = mmu->createProcess();

    uint32_t stack_size = 65536;

    allocateVariable(pid, "<TEXT>", DataType::Char, text_size, mmu, page_table);
    allocateVariable(pid, "<GLOBALS>", DataType::Char, data_size, mmu, page_table);
    allocateVariable(pid, "<STACK>", DataType::Char, stack_size, mmu, page_table);


    std::cout << pid << "\n";

}

void allocateVariable(uint32_t pid, std::string var_name, DataType type, uint32_t num_elements, Mmu *mmu, PageTable *page_table)
{
    // TODO: implement this!
    //   - find first free space within a page already allocated to this process that is large enough to fit the new variable
    //   - if no hole is large enough, allocate new page(s)
    //   - insert variable into MMU
    //   - print virtual memory address

    bool variableExists = false;
    for (int i = 0; i < mmu->getAllProcesses().size(); i++){
        if (mmu->getAllProcesses()[i]->pid == pid){
            for (int j = 0; j < mmu->getAllProcesses()[i]->variables.size(); j++){
                if (mmu->getAllProcesses()[i]->variables[j]->name == var_name){
                    std::cout << "error: variable already exists.\n";
                    variableExists = true;
                }
            }
        }
    }

    bool pidExists = false;
    for (int i = 0; i < mmu->getAllProcesses().size(); i++){
        if (mmu->getAllProcesses()[i]->pid == pid){
            pidExists = true;
            break;
        }
        if (i == mmu->getAllProcesses().size()-1){
            std::cout << "error: process not found.\n";
        }
    }




    if (variableExists == false && pidExists == true){
        int dataTypeSize = 0;
        if (type == DataType::Char){
            dataTypeSize = 1;
        } else if (type == DataType::Short){
            dataTypeSize = 2;
        } else if (type == DataType::Float || type == DataType::Int){
            dataTypeSize = 4;
        } else if (type == DataType::Double || type == DataType::Long){
            dataTypeSize = 8;
        }

        int page_size = page_table->getPageSize();
        bool insertedVariable = false;
        for (int i = 0; i < mmu->getAllProcesses().size(); i++){
            if (mmu->getAllProcesses()[i]->pid == pid && insertedVariable == false){
                for (int j = 0; j < mmu->getAllProcesses()[i]->variables.size(); j++){
                    int variableSize = mmu->getAllProcesses()[i]->variables[j]->size;
                    int virtualAddr = mmu->getAllProcesses()[i]->variables[j]->virtual_address;
                    if (mmu->getAllProcesses()[i]->variables[j]->type == DataType::FreeSpace && variableSize >= dataTypeSize*num_elements && insertedVariable == false){
                        if (variableSize == dataTypeSize*num_elements){
                            int freespaceStartPage = floor((double)virtualAddr / (double)page_size);
                            int freespaceEndPage = floor((double)(virtualAddr + (dataTypeSize*num_elements - 1)) / (double)page_size);
                            if (freespaceStartPage == freespaceEndPage){
                                mmu->getAllProcesses()[i]->variables[j]->name = var_name;
                                mmu->getAllProcesses()[i]->variables[j]->size = dataTypeSize*num_elements;
                                mmu->getAllProcesses()[i]->variables[j]->type = type;
                                insertedVariable = true;
                            } else {
                                int originalFreeSize = variableSize;
                                int nextPageVirtualAddr = (freespaceStartPage + 1) * page_size;
                                int remainingBytesCurrPage = nextPageVirtualAddr - virtualAddr;
                                int offset = remainingBytesCurrPage % dataTypeSize;
                                if (offset == 0){
                                    mmu->getAllProcesses()[i]->variables[j]->name = var_name;
                                    mmu->getAllProcesses()[i]->variables[j]->type = type;
                                    mmu->getAllProcesses()[i]->variables[j]->size = dataTypeSize*num_elements;
                                    mmu->addVariableToProcess(pid, "<FREE_SPACE>", DataType::FreeSpace, originalFreeSize - dataTypeSize*num_elements, virtualAddr + dataTypeSize*num_elements);
                                    insertedVariable = true;
                                    if(var_name != "<TEXT>" && var_name != "<GLOBALS>" && var_name != "<STACK>"){
                                        std::cout << mmu->getAllProcesses()[i]->variables[j]->virtual_address << "\n";
                                    }
                                }
                            }
                        } else if (mmu->getAllProcesses()[i]->variables[j]->size > dataTypeSize*num_elements){
                            int freespaceStartPage = floor((double)virtualAddr / (double)page_size);
                            int freespaceEndPage = floor((double)(virtualAddr + (dataTypeSize*num_elements - 1)) / (double)page_size);
                            if (freespaceStartPage == freespaceEndPage){
                                int originalFreeSize = mmu->getAllProcesses()[i]->variables[j]->size;
                                mmu->getAllProcesses()[i]->variables[j]->name = var_name;
                                mmu->getAllProcesses()[i]->variables[j]->size = dataTypeSize*num_elements;
                                mmu->getAllProcesses()[i]->variables[j]->type = type;
                                mmu->addVariableToProcess(pid, "<FREE_SPACE>", DataType::FreeSpace, originalFreeSize - dataTypeSize*num_elements, virtualAddr + dataTypeSize*num_elements);
                                insertedVariable = true;
                                if(var_name != "<TEXT>" && var_name != "<GLOBALS>" && var_name != "<STACK>"){
                                    std::cout << virtualAddr << "\n";
                                }
                            } else {
                                int originalFreeSize = variableSize;
                                int nextPageVirtualAddr = (freespaceStartPage + 1) * page_size;
                                int remainingBytesCurrPage = nextPageVirtualAddr - virtualAddr;
                                int offset = remainingBytesCurrPage % dataTypeSize;
                                if (offset != 0){
                                    mmu->getAllProcesses()[i]->variables[j]->size = offset;
                                    mmu->addVariableToProcess(pid, var_name, type, dataTypeSize*num_elements, virtualAddr + offset);
                                    mmu->addVariableToProcess(pid, "<FREE_SPACE>", DataType::FreeSpace, originalFreeSize - offset - dataTypeSize*num_elements, virtualAddr + offset + dataTypeSize*num_elements);
                                    if(var_name != "<TEXT>" && var_name != "<GLOBALS>" && var_name != "<STACK>"){
                                        std::cout << virtualAddr + offset << "\n";
                                    }                               
                                    
                                } else {
                                    mmu->getAllProcesses()[i]->variables[j]->name = var_name;
                                    mmu->getAllProcesses()[i]->variables[j]->type = type;
                                    mmu->getAllProcesses()[i]->variables[j]->size = dataTypeSize*num_elements;
                                    mmu->addVariableToProcess(pid, "<FREE_SPACE>", DataType::FreeSpace, originalFreeSize - dataTypeSize, virtualAddr + dataTypeSize*num_elements);
                                    if(var_name != "<TEXT>" && var_name != "<GLOBALS>" && var_name != "<STACK>"){
                                        std::cout << mmu->getAllProcesses()[i]->variables[j]->virtual_address << "\n";
                                    }                                  
                                }
                                insertedVariable = true;
                            }
                            if (insertedVariable == true) {
                                double offsetAmountOfBits = log2(page_table->getPageSize());
            
                                int pageNumberBits = 32 - offsetAmountOfBits;

                                int firstPageNumber = (mmu->getAllProcesses()[i]->variables[j]->virtual_address) >> (int)offsetAmountOfBits;
                                int lastPageNumber = (mmu->getAllProcesses()[i]->variables[j]->virtual_address + dataTypeSize*num_elements - 1) >> (int)offsetAmountOfBits;

                                for (int i = firstPageNumber; i <= lastPageNumber; i++){
                                    page_table->addEntry(pid, i);
                                }
                            }
                        }
                    }
                }
            }
        }
    }   

}

void setVariable(uint32_t pid, std::string var_name, uint32_t offset, void *value, Mmu *mmu, PageTable *page_table, void *memory)
{
    // TODO: implement this!
    //   - look up physical address for variable based on its virtual address / offset
    //   - insert `value` into `memory` at physical address
    //   * note: this function only handles a single element (i.e. you'll need to call this within a loop when setting
    //           multiple elements of an array)

    bool setVariable = false;
    int virtualAddr;
    int physicalAddr;
    for (int i = 0; i < mmu->getAllProcesses().size(); i++){
        if (mmu->getAllProcesses()[i]->pid == pid){
            for (int j = 0; j < mmu->getAllProcesses()[i]->variables.size(); j++){
                if (mmu->getAllProcesses()[i]->variables[j]->name == var_name){
                    virtualAddr = mmu->getAllProcesses()[i]->variables[j]->virtual_address + offset;
                    physicalAddr = page_table->getPhysicalAddress(pid, virtualAddr);
                    memcpy(physicalAddr + (char *)memory, value, mmu->getAllProcesses()[i]->variables[j]->size);
                    setVariable = true;
                    break;
                }

            }
        }
        if (setVariable == true){
            break;
        }
    }

}

void freeVariable(uint32_t pid, std::string var_name, Mmu *mmu, PageTable *page_table)
{
    // TODO: implement this!
    //   - remove entry from MMU
    //   - free page if this variable was the only one on a given page

    bool variableExists = false;
    for (int i = 0; i < mmu->getAllProcesses().size(); i++){
        if (mmu->getAllProcesses()[i]->pid == pid){
            for (int j = 0; j < mmu->getAllProcesses()[i]->variables.size(); j++){
                if (mmu->getAllProcesses()[i]->variables[j]->name == var_name){
                    variableExists = true;
                    break;
                }
                if (j == mmu->getAllProcesses()[i]->variables.size()-1){
                    std::cout << "error: variable not found.\n";
                }
            }
        }
        if (variableExists == true){
            break;
        }
    }

    bool pidExists = false;
    for (int i = 0; i < mmu->getAllProcesses().size(); i++){
        if (mmu->getAllProcesses()[i]->pid == pid){
            pidExists = true;
            break;
        }
        if (i == mmu->getAllProcesses().size()-1){
            std::cout << "error: process not found.\n";
        }
    }

    if (variableExists == true && pidExists == true){
        int virtualAddr;
        int sizeOfVariable;
        std::string removedVarName;
        for (int i = 0; i < mmu->getAllProcesses().size(); i++){
            if (mmu->getAllProcesses()[i]->pid == pid){
                for (int j = 0; j < mmu->getAllProcesses()[i]->variables.size(); j++){
                    if (mmu->getAllProcesses()[i]->variables[j]->name == var_name){
                        removedVarName = mmu->getAllProcesses()[i]->variables[j]->name;
                        virtualAddr = mmu->getAllProcesses()[i]->variables[j]->virtual_address;
                        sizeOfVariable = mmu->getAllProcesses()[i]->variables[j]->size;
                        mmu->getAllProcesses()[i]->variables[j]->type = DataType::FreeSpace;
                        mmu->getAllProcesses()[i]->variables[j]->name = "<FREE_SPACE>";
                        if (j > 0){
                            // make sure to check for out of bounds
                            if (mmu->getAllProcesses()[i]->variables[j-1]->type == DataType::FreeSpace){
                                mmu->getAllProcesses()[i]->variables[j-1]->size =  mmu->getAllProcesses()[i]->variables[j-1]->size + mmu->getAllProcesses()[i]->variables[j]->size;
                                mmu->getAllProcesses()[i]->variables.erase(mmu->getAllProcesses()[i]->variables.begin()+j);
                            }
                        }
                        if (j < mmu->getAllProcesses()[i]->variables.size()){
                            // make sure to check for out of bounds
                            if (mmu->getAllProcesses()[i]->variables[j+1]->type == DataType::FreeSpace){
                                mmu->getAllProcesses()[i]->variables[j]->size =  mmu->getAllProcesses()[i]->variables[j]->size + mmu->getAllProcesses()[i]->variables[j+1]->size;
                                mmu->getAllProcesses()[i]->variables.erase(mmu->getAllProcesses()[i]->variables.begin()+j+1);
                            }
                        }
                    }
                }
            }
        }
        int firstPage = virtualAddr / page_table->getPageSize();
        int lastPage = (virtualAddr + sizeOfVariable - 1) / page_table->getPageSize();

        std::vector<Variable*> vars = mmu->getVariablesByPid(pid);

        std::vector<bool> sharedPages;

        for (int i = firstPage; i <= lastPage; i++){
            bool wasShared = false;
            for (int j = 0; j < vars.size(); j++){
                if (vars[j]->name != removedVarName && i == vars[j]->virtual_address / page_table->getPageSize()){
                    wasShared = true;
                    break;
                } else {
                    wasShared = false;
                }
            }
            sharedPages.push_back(wasShared);
        }

        for (int i = 0; i < sharedPages.size(); i++){
            if (sharedPages[i] == false){
                page_table->removePage(pid, i+firstPage);
            }
        }
    }
}

void terminateProcess(uint32_t pid, Mmu *mmu, PageTable *page_table)
{
    // TODO: implement this!
    //   - remove process from MMU
    //   - free all pages associated with given process
    bool pidExists = false;
    for (int i = 0; i < mmu->getAllProcesses().size(); i++){
        if (mmu->getAllProcesses()[i]->pid == pid){
            pidExists = true;
            break;
        }
        if (i == mmu->getAllProcesses().size()-1){
            std::cout << "error: process not found.\n";
        }
    }

    if (pidExists == true){
        for (int i = 0; i < mmu->getAllProcesses().size(); i++){
            if (mmu->getAllProcesses()[i]->pid == pid){
                mmu->terminateProcess(pid);
                page_table->removeAllEntriesByID(pid);
            }
        }
    }
}

void splitString(std::string text, char d, std::vector<std::string>& result)
{
    enum states { NONE, IN_WORD, IN_STRING } state = NONE;

    int i;
    std::string token;
    result.clear();
    for (i = 0; i < text.length(); i++)
    {
        char c = text[i];
        switch (state) {
            case NONE:
                if (c != d)
                {
                    if (c == '\"')
                    {
                        state = IN_STRING;
                        token = "";
                    }
                    else
                    {
                        state = IN_WORD;
                        token = c;
                    }
                }
                break;
            case IN_WORD:
                if (c == d)
                {
                    result.push_back(token);
                    state = NONE;
                }
                else
                {
                    token += c;
                }
                break;
            case IN_STRING:
                if (c == '\"')
                {
                    result.push_back(token);
                    state = NONE;
                }
                else
                {
                    token += c;
                }
                break;
        }
    }
    if (state != NONE)
    {
        result.push_back(token);
    }
}