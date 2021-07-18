#include "../includes/tarefa_2_3.hpp"

XBT_LOG_NEW_DEFAULT_CATEGORY(s4u_app_token_ring, ("Messages specific for this s4u example"));

class Simulador
{
	//Declaração das estruturas e variaveis
	simgrid::s4u::MutexPtr 	mutex;
	queue <string> 			movt;	//Queue movimento
	queue <string> 			input;
	queue <string> 			output;
	string 					file_name;
	int 					nLines;
	int						msg_size;
	

public:
	explicit Simulador(string file, int Lines, int size, simgrid::s4u::MutexPtr mutexptr)
	{	
		//Inicializa as variaveis
		this->file_name =	file;		
		this->nLines 	= 	Lines;
		this->msg_size 	= 	size;
		this->mutex 	= 	mutexptr;
	}	

	void operator()()	//Função operator() + (Args)
	{
		vector<sg4::Disk*> const& disk_list = simgrid::s4u::Host::current()->get_disks(); //Retornas os discos
		//Declaração das Mailboxes
		sg4::Mailbox* 	my_mailbox;		
		sg4::Mailbox*	next_mailbox;
		sg4::Disk* 		disk = disk_list.front();	//Lista os discos
		string 			msgStr;						//String auxiliar para as funções receiver
		int 			rank;

		try					 
		{
			//Tenta ler o nome do ator, que nesse caso deve ser numerico e converte para int					
			rank = stoi(sg4::this_actor::get_name()); //this_actor atua sobre o ator atual
		}
		catch(const invalid_argument& ia)	//Trata a exceção de argumento invalido			
		{
			//Transforma em string e imprime uma mensagem com o nome argumento invalido
			throw invalid_argument(string("Atores devem ter um nome numerico, não ") + ia.what());
		}

		//Acessa a mailbox com o rank convertido para string
		my_mailbox = sg4::Mailbox::by_name(to_string(rank));

		//get_instance -> singleton -> garante a existencia de apenas uma instancia dessa classe
		if(rank + 1 == sg4::Engine::get_instance()->get_host_count())
			next_mailbox = sg4::Mailbox::by_name("0");			//Se chegou no final, proxima mailbox é a inicial	
		else
			next_mailbox = sg4::Mailbox::by_name(to_string(rank + 1));
        
		/*Ator princial
			Mutex controla o acesso à seção crítica em trechos de código que acessam memória compartilhada.
			Nesse caso, queues de entrada e saída de mensagens.
		*/
		if(rank == 0)		
		{	//Tamanho extraido do arquivo
			char msg[msg_size];			
			ifstream msg_file(file_name);
			
			if(msg_file.is_open())
			{	//Faz a leitura das mensagens do arquivo
				for(int countLines = 0; countLines < nLines; countLines++)
				{
					msg_file.getline(msg, msg_size);
					movt.push(msg);
				}
				msg_file.close();
			
				//Envia mensagens lidas do arquivo de entrada
				while(!movt.empty())		
				{	
					mutex->lock();
					sender(movt.front(), next_mailbox, rank);
					movt.pop();
					mutex->unlock();
				}

				//Ator principal recebe as mensagens e adiciona à fila de entrada
				while(input.size() < nLines)	{
					
					msgStr = receiver(my_mailbox, rank, disk);
					mutex->lock();
					input.push(msgStr);
					mutex->unlock();
				}
				XBT_INFO("Done!");
			}
		}else								
		{	
			//Ator recebe as mensagens, adiciona à fila de entrada e à fila de saída
			for(int countMsg = 0; countMsg < nLines; countMsg++)	
			{
				msgStr = receiver(my_mailbox, rank, disk);
				//Preenche as filas de entrada e saida
				mutex->lock();
				input.push(msgStr);
				output.push(msgStr);
				mutex->unlock();
			}
			//Ator envia mensagens a partir da fila de saída. Só envia após receber todas as mensagens
			while(!output.empty())
			{
				//Esvazia fila de saida
				mutex->lock();
				sender(output.front(), next_mailbox, rank);
				output.pop();
				mutex->unlock();
			}
		}
	}
};

int main(int argc, char **argv)
{	//Contem as funções principais da simulação
	sg4::Engine e(&argc, argv);							
	
	//Informa caso os parametros sejam invalidos
	xbt_assert(argc > 2, "Usage: %s platform.xml <file name>\n", argv[0]); 	
	//Carrega o arquivo xml da plataforma
	e.load_platform(argv[1]); 		
	
	XBT_INFO("Number of hosts '%zu'", e.get_host_count());
	
	int id 			= 0;
	int nLines 		= 0;
	int msg_size 	= 0;

	//Extrai informações sobre o conteudo do arquivo
	task_init(&nLines, &msg_size, argv[2]);
	//Lista com os hosts
	vector<sg4::Host*> list = e.get_all_hosts();	
	simgrid::s4u::MutexPtr mutex = simgrid::s4u::Mutex::create();

	for(auto const& host : list)
	{
		//Cria o ator relacionado ao host da platform.xml
		sg4::Actor::create((to_string(id)).c_str(), host, Simulador(argv[2], nLines, (msg_size + 1), mutex));	
		id++;
	}

	//Roda a simulação
	e.run();	
	XBT_INFO("Simulation time: %g", sg4::Engine::get_clock());

	return 0;
}

