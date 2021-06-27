#include "../includes/tarefa_2_3.hpp"


XBT_LOG_NEW_DEFAULT_CATEGORY(s4u_app_token_ring, ("Messages specific for this s4u example"));

class Simulador
{
public:
	explicit Simulador(/* args */) = default;

	void operator()() const		//Função operator() + (Args)
	{
		sg4::Mailbox* my_mailbox;		//Declaração das Mailboxes
		sg4::Mailbox* next_mailbox;
		int nome_numerico;

		try					//Tenta ler o nome do ator, que nesse caso deve ser numerico e converte para int 
		{					
			nome_numerico = stoi(sg4::this_actor::get_name()); //this_actor atua sobre o ator atual
		}
		catch(const invalid_argument& ia)			//Trata a exceção de argumento invalido
		{
			throw invalid_argument(string("Atores devem ter um nome numerico, não ") + ia.what());
			//Transforma em string e imprime uma mensagem com o nome argumento invalido
		}

		//Acessa a mailbox com o nome_numerico convertido para string
		my_mailbox = sg4::Mailbox::by_name(to_string(nome_numerico));

		//get_instance -> singleton -> garante a existencia de apenas uma instancia dessa classe
		if(nome_numerico + 1 == sg4::Engine::get_instance()->get_host_count())
			next_mailbox = sg4::Mailbox::by_name("0");			//Se chegou no final, proxima mailbox é a inicial	
		else
			next_mailbox = sg4::Mailbox::by_name(to_string(nome_numerico + 1));

        vector<sg4::Disk*> const& disk_list = simgrid::s4u::Host::current()->get_disks();
		sg4::Disk* disk = disk_list.front();

		if(nome_numerico == 0)		//root actor
		{	
            string msg = "Hello, SimGrid!";			//Cria uma mensagem para ser enviada pelo ring
			
			sender(msg, next_mailbox, nome_numerico);
			receiver(my_mailbox, nome_numerico, disk);

			XBT_INFO("Fim!");
		
		}else
		{	
			string msg;
			msg = receiver(my_mailbox, nome_numerico, disk);
			sender(msg, next_mailbox, nome_numerico);
		}
	}
};

int main(int argc, char **argv)
{
	sg4::Engine e(&argc, argv);							//Contem as funções principais da simulação
	
	xbt_assert(argc > 1, "Usage: %s hosts_with_disks.xml\n", argv[0]); 	//Informa os parametros de entrada caso tenha algum problema
	e.load_platform(argv[1]); 				 						//Carrega o arquivo platform.xml
	
	XBT_INFO("Número de hosts '%zu'", e.get_host_count());
	
	int id = 0;

	vector<sg4::Host*> list = e.get_all_hosts();	//Lista com os hosts
	
	for(auto const& host : list)
	{
		sg4::Actor::create((to_string(id)).c_str(), host, Simulador());	//Cria o ator relacionado ao host da platform.xml
		id++;
	}
	
	e.run();	//Roda a simulação
	XBT_INFO("Tempo de simulação: %g", simgrid::s4u::Engine::get_clock());

	return 0;
}

