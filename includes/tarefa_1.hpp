#include <simgrid/s4u.hpp>
#include <algorithm>
#include <iostream>
#include <string>
#include <map>
#include <vector>

namespace sg4 = simgrid::s4u;	//Abreviação
void sender(std::string msg, sg4::Mailbox* next_mailbox, int nome_numerico);
std::string receiver(sg4::Mailbox* my_mailbox, int nome_numerico);
