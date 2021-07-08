#include <simgrid/s4u.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <queue>

namespace sg4 = simgrid::s4u;	
using namespace std;

void sender(string msg, sg4::Mailbox* next_mailbox, int nome_numerico);
string receiver(sg4::Mailbox* my_mailbox, int nome_numerico, sg4::Disk* disk);
void task_init(int* nLines, int* msg_size, const char* file_name);