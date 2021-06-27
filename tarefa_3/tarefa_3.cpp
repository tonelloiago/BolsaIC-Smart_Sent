#include "../includes/tarefa_2_3.hpp"

XBT_LOG_NEW_DEFAULT_CATEGORY(s4u_app_token_ring, ("Messages specific for this s4u example"));

class Simulador
{
	//Declaração das estruturas e variaveis
	queue <string> movimento;
	queue <string> input;
	queue <string> output;
	string file_name;
	int nLines;
	int msg_size;

public:
	explicit Simulador(string file, int Lines, int size)
	{	
		//Inicializa as variaveis
		this->file_name = file;		
		this->nLines = Lines;
		this->msg_size = size;
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
        
		//retorna o disco presente no arquivo da plataforma
		vector<sg4::Disk*> const& disk_list = simgrid::s4u::Host::current()->get_disks();
		sg4::Disk* disk = disk_list.front();

		if(nome_numerico == 0)		//Ator princial
		{	
			
			char msg[msg_size];			//Tamanho extraido do arquivo
			ifstream msg_file(file_name);
			
			if(msg_file.is_open())
			{	//Faz a leitura das mensagens do arquivo
				for(int pos = 0; pos < nLines; pos++)
				{
					msg_file.getline(msg, msg_size);
					movimento.push(msg);
				}
				msg_file.close();
			}

			while(!movimento.empty())
			{
				output.push(movimento.front());
				movimento.pop();
				sender(output.front(), next_mailbox, nome_numerico);
				
				input.push(receiver(my_mailbox, nome_numerico, disk));
			}
			XBT_INFO("Fim!");
		
		}else
		{	
			for(int pos = 0; pos < nLines; pos++)
			{
				input.push(receiver(my_mailbox, nome_numerico, disk));
				output.push(input.front());
				input.pop();
				sender(output.front(), next_mailbox, nome_numerico);
				output.pop();
			}
		}
	}
};

int main(int argc, char **argv)
{	
	sg4::Engine e(&argc, argv);							//Contem as funções principais da simulação
	
	xbt_assert(argc > 2, "Usage: %s platform.xml <file name>\n", argv[0]); 	//Se menor que 3 param, informa
	e.load_platform(argv[1]); 		//Carrega o arquivo xml
	
	XBT_INFO("Número de hosts '%zu'", e.get_host_count());
	
	int id = 0;
	int nLines = 0;
	int msg_size = 0;
	//Extrai informações sobre o conteudo do arquivo
	tarefa_init(&nLines, &msg_size, argv[2]);

	vector<sg4::Host*> list = e.get_all_hosts();	//Lista com os hosts

	for(auto const& host : list)
	{
		sg4::Actor::create((to_string(id)).c_str(), host, Simulador(argv[2], nLines, msg_size));	//Cria o ator relacionado ao host da platform.xml
		id++;
	}

	e.run();	//Roda a simulação
	XBT_INFO("Tempo de simulação: %g", sg4::Engine::get_clock());

	return 0;
}

