#include "../includes/tarefa_2_3.hpp"

XBT_LOG_EXTERNAL_DEFAULT_CATEGORY(s4u_app_token_ring);

void sender(string msg, sg4::Mailbox* next_mailbox, int nome_numerico)
{
	XBT_INFO("Host \"%u\" enviando \"%s\" para o host \"%s\"", nome_numerico, msg.c_str(), next_mailbox->get_cname());
    next_mailbox->put(&msg, msg.size());	//Coloca a msg na proxima mailbox
}

string receiver(sg4::Mailbox* my_mailbox, int nome_numerico, sg4::Disk* disk)
{
    const auto *res = static_cast<string*>(my_mailbox->get());	//Le a mailbox e transforma para string	
	disk->set_data(new string(res->c_str()));			//Armazena a mensagem
	const auto* data = static_cast<string*>(disk->get_data());		//Le a mensagem

	XBT_INFO("Host \"%u\" recebeu \"%s\" de \"%lu\" bytes", nome_numerico, data ? data->c_str() : "No user data", data->size());	//Imprime
	XBT_INFO("Host \"%u\" armazenando em  \"%s\": '%s'",nome_numerico, disk->get_cname(),data ? data->c_str() : "No user data");

	return data->c_str();
}