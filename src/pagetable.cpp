#include "pagetable.h"
#include <cmath>

PageTable::PageTable(int page_size)
{
    _page_size = page_size;
}

PageTable::~PageTable()
{
}

std::vector<std::string> PageTable::sortedKeys()
{
    std::vector<std::string> keys;

    std::map<std::string, int>::iterator it;
    for (it = _table.begin(); it != _table.end(); it++)
    {
        keys.push_back(it->first);
    }

    std::sort(keys.begin(), keys.end(), PageTableKeyComparator());

    return keys;
}

void PageTable::addEntry(uint32_t pid, int page_number)
{
    // Combination of pid and page number act as the key to look up frame number
    std::string entry = std::to_string(pid) + "|" + std::to_string(page_number);


    if (_table.count(entry) == 0){
        int frame = 0; 
        // Find free frame
        // TODO: implement this!

        std::vector<std::string> keys = sortedKeys();

        if (keys.size() == 0){
            _table[entry] = frame;
        } else {
            for (int i = 0; i < keys.size(); i++){  
                std::map<std::string, int>::iterator it;
                for (it = _table.begin(); it != _table.end(); it++){
                    //check if keys 1-X map to frame anywhere in the table,
                    //if 1 or more does, increment the frame and try again.
                    if (_table[keys[i]] == frame){
                        frame++;
                        i = 0;
                        break;
                    }
                }
                if (i == keys.size() - 1) {
                    _table[entry] = frame;
                }
            }
        }
    } 
}

void PageTable::removeAllEntriesByID(uint32_t pid)
{
    std::vector<std::string> keys = sortedKeys();

    for (int i = 0; i < keys.size(); i++){
        std::string locatedPid = keys[i].substr(0,keys[i].find_first_of('|'));
        if (pid == stoi(locatedPid)){
            _table.erase(keys[i]);
        }
    }
}

int PageTable::getPhysicalAddress(uint32_t pid, uint32_t virtual_address)
{
    // Convert virtual address to page_number and page_offset
    // TODO: implement this!
    int page_number = 0;
    int page_offset = 0;

    double offsetAmountOfBits = log2(_page_size);
    int pageNumberBits = 32 - offsetAmountOfBits;
    page_number = virtual_address >> (int)offsetAmountOfBits;
    page_offset = virtual_address << pageNumberBits;
    page_offset = page_offset >> pageNumberBits;


    // Combination of pid and page number act as the key to look up frame number
    std::string entry = std::to_string(pid) + "|" + std::to_string(page_number);
    
    // If entry exists, look up frame number and convert virtual to physical address
    int address = -1;
    int frame;
    if (_table.count(entry) > 0)
    {
        // TODO: implement this!
        frame = _table[entry];
        address = frame * _page_size + page_offset;
    }

    return address;
}

// bool PageTable::isOtherVarsOnPage(int pid, int page_number){

//     for (int i = 0; i < pidList.size(); i++){
//         std::string entry = std::to_string(pidList[i]) + "|" + std::to_string(page_number);
//         if (_table.count(entry) > 0){
//             return true;
//         }
//     }
//     return false;
// }


std::vector<int> PageTable::getProccesesList(){
    std::vector<int> pids;
    std::vector<std::string> keys = sortedKeys();
    for (int i = 0; i < keys.size(); i++) {
        int pidFromKey = stoi(keys[i].substr(0,keys[i].find_first_of('|')));
        if (!(std::find(pids.begin(), pids.end(), pidFromKey) != pids.end())){
            pids.push_back(pidFromKey);
        }
    }
    return pids;
}

void PageTable::removePage(uint32_t pid, int page_number){
    std::string entry = std::to_string(pid) + "|" + std::to_string(page_number);
    if(_table.count(entry) > 0){
        _table.erase(entry);
    }
}

int PageTable::getPageSize(){
    return _page_size;
}


void PageTable::print()
{
    int i;

    std::cout << " PID  | Page Number | Frame Number" << std::endl;
    std::cout << "------+-------------+--------------" << std::endl;

    int pidSpacing = 5;
    int pageNumberSpacing = 12;
    int frameNumberSpacing = 13;


    std::vector<std::string> keys = sortedKeys();

    for (i = 0; i < keys.size(); i++)
    {
        // TODO: print all pages
        std::string pid = keys[i].substr(0,keys[i].find_first_of('|'));
        std::string pageNumber = keys[i].substr(keys[i].find_first_of('|')+1, keys[i].length()-1);
        int frameNumberInt = (int)_table[keys[i]];
        std::string frameNumberString = std::to_string(frameNumberInt);
        std::cout << " " << pid << std::string(pidSpacing - pid.size(), ' ') << "|" << std::string(pageNumberSpacing - pageNumber.size(), ' ') << pageNumber << " |" << std::string(frameNumberSpacing - frameNumberString.size(), ' ') << _table[keys[i]] << "\n";

    }
}
