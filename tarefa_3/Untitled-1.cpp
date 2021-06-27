
#include "../includes/tarefa_2_3.hpp"
#include <queue>
#include <fstream>


XBT_LOG_NEW_DEFAULT_CATEGORY(s4u_app_token_ring, ("Messages specific for this s4u example"));

class Simulador
{
	//Declaração das estruturas -> vetores de mensagens	
	msg_array *movimento;
	msg_array *input;
	msg_array *output;
	
public:
	explicit Simulador()
	{			
		 //for -> popula com null - define o espaço com o tamanho em bytes definido
		for(int i = 0; i < NUM_MSG; i++)
		 {
			movimento[i].msg = '\0';
			input[i].msg = '\0';
			output[i].msg = '\0';
			
		}
	}

	void operator()()	//Função operator() + (Args)
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

		if(nome_numerico == 0)		//Ator princial
		{	
			ifstream input_file("movimento.txt");	//Abre o arquivo
			string msg;

			int i = 0;
			while(getline(input_file, msg) && i < NUM_MSG)	//Lê linha por linha
			{
				movimento[i].msg = msg;						//Preenche o vetor movimento
				i++;
			}

			for(i = 0; i < NUM_MSG; i++)					//Envia as msg's a partir do host 0
			{
				output[i].msg = movimento[i].msg;			//Preenche o vetor de saida
				sender(output[i].msg, next_mailbox, nome_numerico);	//Envia a partir do vetor de saida

				input[i].msg = receiver(my_mailbox, nome_numerico, disk);	//Preenche o vetor de entrada
			}
			
			XBT_INFO("Fim!");
		
		}else
		{	
			for(int i = 0; i < NUM_MSG; i++)		//Proximo ator envia as mensagens conforme chegam
			{
				input[i].msg = receiver(my_mailbox, nome_numerico, disk);	//Preenche o vetor de entrada
				output[i].msg = input[i].msg;								//Preenche o vetor de saida

				sender(output[i].msg, next_mailbox, nome_numerico);			//Envia a partir do vetor de saida
			}

			for(int i = 0; i < NUM_MSG; i++)
				cout << output[i].msg << endl;
		}
	}
};

int main(int argc, char **argv)
{	
	sg4::Engine e(&argc, argv);							//Contem as funções principais da simulação
	
	xbt_assert(argc > 2, "Usage: %s platform.xml <file name>\n", argv[0]); 	//Se menor que 3 param, informa
	e.load_platform(argv[1]); 				 						//Carrega o arquivo xml
	
	XBT_INFO("Número de hosts '%zu'", e.get_host_count());
	int id = 0;

	vector<sg4::Host*> list = e.get_all_hosts();	//Lista com os hosts

	for(auto const& host : list)
	{
		sg4::Actor::create((to_string(id)).c_str(), host, Simulador());	//Cria o ator relacionado ao host da platform.xml
		id++;
	}
	
	e.run();	//Roda a simulação
	XBT_INFO("Tempo de simulação: %g", sg4::Engine::get_clock());

	return 0;
}

