#include "../includes/tarefa_1.hpp"

XBT_LOG_EXTERNAL_DEFAULT_CATEGORY(s4u_app_token_ring);

void sender(std::string msg, sg4::Mailbox* next_mailbox, int nome_numerico)
{
    next_mailbox->put(&msg, msg.size());
	XBT_INFO("Host \"%u\" enviando \"%s\" para o host \"%s\"", nome_numerico, msg.c_str(), next_mailbox->get_cname());
}

std::string receiver(sg4::Mailbox* my_mailbox, int nome_numerico)
{
	const auto *res = static_cast<std::string*>(my_mailbox->get());	//Le a mailbox e transforma para string
	XBT_INFO("Host \"%u\" recebeu \"%s\" de \"%lu\" bytes", nome_numerico, res->c_str(), res->size());	//Imprime

	return res->c_str();
}