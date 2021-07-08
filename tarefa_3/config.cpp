#include "../includes/tarefa_2_3.hpp"

XBT_LOG_EXTERNAL_DEFAULT_CATEGORY(s4u_app_token_ring);

//Extrai informações do arquivo de entrada
void task_init(int* nLines, int* msg_size, const char* file_name)
{
	char c;
	int tam_msg = 0;
	FILE* file;
	file = fopen(file_name, "r");

	for(c = getc(file); c != EOF; c = getc(file))
	{
		if(c == '\n')
		{
			(*nLines)++;

			if(tam_msg > *msg_size)			//Verifica se o tamanho da mensagem é maior
				*msg_size = tam_msg;
			tam_msg = 0;					//Reinicia o contador

		}else
			tam_msg++;
	}
	*msg_size += 1; 						//Coluna do arquivo inicia em 1
	std::fclose(file);
}