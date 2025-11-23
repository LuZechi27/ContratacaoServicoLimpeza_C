//======================================================================

#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include <stdbool.h>

// ================== Macros ====================================================

#define MAX_CPF               32                            // Tamanho maximo do CPF.
#define MAX_RG                32                            // Tamanho maximo do RG.
#define MAX_NOME              64                            // Tamanho maximo do nome.
#define MAX_DATA              16                            // Tamanho maximo da string de data.
#define MAX_QT_TELEFONES      8                             // Quantidade maxima de telefones.
#define MAX_TAM_TELEFONE      32                            // Tamanho maximo dos telefones.
#define MAX_ENDERECO          128                           // Tamanho maximo do endereco.
#define MAX_CEP               16                            // Tamanho maximo do CEP.
#define MAX_CIDADE            128                           // Tamanho maximo do nome da cidade.
#define MAX_QT_EMAILS         8                             // Quantidade maxima de emails.
#define MAX_TAM_EMAILS        128                           // Tamanho maximo dos emails.
#define CAPACIDADE_FAX_VAZIO  64                            // Capacidade do vetor dos faxineiros quando vazio.
#define CAPACIDADE_CLI_VAZIO  64                            // Capacidade do vetor dos clientes quando vazio.
#define CAPACIDADE_SERV_VAZIO 32                            // Capacidade do vetor dos servicos quando vazio.
#define MAX_ID_SERV           (2 * MAX_CPF + MAX_DATA)      // Capacidade do identificador de servicos.

// =================== Structs ==============================================

/* Tipo de data (dd/mm/aaaa) convertido.
 * dia
 * mes
 * ano
 */
typedef struct
{
      int dia;                // Entre 1 e 31, dependendo do mes.
      int mes;                // Entre 1 e 12.
      int ano;                // Pode ser bissexto.
      bool ano_bissexto;      // Se (ano % 400 == 0) ou se (ano % 4 == 0 && ano % 100 != 0).
} data;

/* CPF Unico
 * RG
 * Nome
 * Sexo
 * Data nascimento (struct data)
 * Quantidade de telefones (int)
 * Telefones (vetor)
 */
typedef struct
{
      char cpf[MAX_CPF];                  // 999.999.999-99
      char rg[MAX_RG];                    // 99.999.999-9
      char nome[MAX_NOME];
      char sexo;
      data data_nascimento;               // dd/mm/aaaa
      int quantidade_telefones;
      char telefones[MAX_QT_TELEFONES][MAX_TAM_TELEFONE];
} faxineiro;

/* CPF Unico
 * Nome
 * Data nascimento (struct data)
 * Endereco
 * CEP
 * Cidade
 * Quantidade de Emails (int)
 * Emails (vetor)
 * Quantidade de telefones (int)
 * Telefones (vetor)
 */
typedef struct
{
      char cpf[MAX_CPF];
      char nome[MAX_NOME];
      data data_nascimento;
      char endereco[MAX_ENDERECO];
      char cep[MAX_CEP];
      char cidade[MAX_CIDADE];
      int quantidade_emails;
      char emails[MAX_QT_EMAILS][MAX_TAM_EMAILS];
      int quantidade_telefones;
      char telefones[MAX_QT_TELEFONES][MAX_TAM_TELEFONE];         // max = +55 (99) 99999-9999
} cliente;

/* ID (CPF fax + CPF cli + data(string))
 * CPF Unico (faxineiro)
 * CPF Unico (cliente)
 * Data (struct data)
 * Valor (float)
 */
typedef struct
{
      char id[MAX_ID_SERV];
      char cpf_faxineiro[MAX_CPF];
      char cpf_cliente[MAX_CPF];
      data data;
      float valor;
} servico;

// ====================== MISC =========================================================

/* Usado para descobrir se o arquivo esta vazio.
 * Se o arquivo não existe, ou se ocorrer um erro na hora de abrir,
 * trata como vazio e retorna true.
 * Se o arquivo não está vazio, retorna false.
 */
bool arquivo_vazio(const char* nome_arquivo)
{
      FILE *arquivo;
      int tamanho_arquivo;

      if ((arquivo = fopen(nome_arquivo, "rb+")) == NULL)
      {
            perror("Erro ao abrir o arquivo");
            return true;
      }
      fseek(arquivo, 0, SEEK_END);
      tamanho_arquivo = ftell(arquivo);
      fclose(arquivo);

      if (tamanho_arquivo == 0)
            return true;

      return false;
}

/* Usada para converter uma string de data em uma struct data.
 * Retorna true se a data e valida e false caso contrario.
 */
bool converter_data(char* string_data, data* data_convertida)
{
      if (strlen(string_data) != 10)
      {
            return false;
      }
      int dia;
      int mes;
      int ano;

      int pos;

      pos = strcspn(string_data, "/");
      string_data[pos] = '\0';
      dia = atoi(string_data);
      string_data += pos + 1;

      pos = strcspn(string_data, "/");
      string_data[pos] = '\0';
      mes = atoi(string_data);
      string_data += pos + 1;

      ano = atoi(string_data);

      data_convertida->ano = ano;
      data_convertida->dia = dia;
      data_convertida->mes = mes;

      int dias_mes;

      if ((ano % 400 == 0) || ((ano % 4 == 0) && (ano % 100 != 0)))
            data_convertida->ano_bissexto = true;
      else
            data_convertida->ano_bissexto = false;

      if (mes >= 1 && mes <= 12)
      {
            if (mes == 2)
            {
                  if (data_convertida->ano_bissexto)
                        dias_mes = 29;
                  else
                        dias_mes = 28;
            }
            else if (mes == 1 || mes == 3 || mes == 5 || mes == 7 || mes == 8 || mes == 10 || mes == 12)
                  dias_mes = 31;
            else
                  dias_mes = 30;

            if (dia < 1 || dia > dias_mes)
                  return false;
            else
                  return true;
      }
      else 
            return false;
}

void converte_data_para_string(data data_a_converter, char *data_convertida)
{
      char dia[4];
      char mes[4];
      char ano[5];

      snprintf(dia, sizeof(dia), "%02d", data_a_converter.dia);
      dia[2] = '/';
      dia[3] = '\0';
      strcpy(data_convertida, dia);

      snprintf(mes, sizeof(mes), "%02d", data_a_converter.mes);
      mes[2] = '/';
      mes[3] = '\0';
      strcat(data_convertida, mes);

      snprintf(ano, sizeof(ano), "%04d", data_a_converter.ano);
      strcat(data_convertida, ano);
}

bool data_menor_ou_igual(data data1, data data2)
{
      if (data1.ano != data2.ano)
            return data1.ano < data2.ano;
      if (data1.mes != data2.mes)
            return data1.mes < data2.mes;
      return  data1.dia <= data2.dia;
}

bool data_esta_no_intervalo(data data_comparada, data data_inicio, data data_final)
{     
      return data_menor_ou_igual(data_inicio, data_comparada) && data_menor_ou_igual(data_comparada, data_final);
}

//=================================================================================//
//
//================= Funcoes de Busca ==================================================

// Usado para encontrar um faxineiro no vetor. Retorna -1 se o faxineiro não está no vetor.
int buscar_faxineiro(faxineiro *faxineiros, int inicio, int fim, char* cpf_procurado)
{
      if (inicio > fim)
            return -1;

      int meio = inicio + (fim - inicio) / 2;
      int comparacao = strcmp(faxineiros[meio].cpf, cpf_procurado);

      if (comparacao == 0)
            return meio;
      else if (comparacao < 0)
            return buscar_faxineiro(faxineiros, meio + 1, fim, cpf_procurado);
      else
            return buscar_faxineiro(faxineiros, inicio, meio - 1, cpf_procurado);
}

/* Usado para encontrar o local no vetor para inserir um faxineiro.
 * Retorna -1 caso o faxineiro já existe no vetor.
 */
int busca_para_inserir_faxineiro(faxineiro *faxineiros, int inicio, int fim, char* cpf_procurado)
{
      if (inicio > fim)
            return inicio;

      int meio = inicio + (fim - inicio) / 2;
      int comparacao = strcmp(faxineiros[meio].cpf, cpf_procurado);

      if (comparacao == 0)
            return -1;
      else if (comparacao < 0)
            return busca_para_inserir_faxineiro(faxineiros, meio + 1, fim, cpf_procurado);
      else
            return busca_para_inserir_faxineiro(faxineiros, inicio, meio - 1, cpf_procurado);
}

// Usado para encontrar um cliente no vetor. Retorna -1 se o cliente não está no vetor.
int buscar_cliente(cliente* clientes, int inicio, int fim, char* cpf_procurado)
{
      if (inicio > fim)
            return -1;

      int meio = inicio + (fim - inicio) / 2;
      int comparacao = strcmp(clientes[meio].cpf, cpf_procurado);

      if (comparacao == 0)
            return meio;
      else if (comparacao < 0)
            return buscar_cliente(clientes, meio + 1, fim, cpf_procurado);
      else
            return buscar_cliente(clientes, inicio, meio - 1, cpf_procurado);
}

/* Usado para encontrar o local no vetor para inserir um cliente.
 * Retorna -1 caso o cliente já existe no vetor.
 */
int busca_para_inserir_cliente(cliente* clientes, int inicio, int fim, char* cpf_procurado)
{
      if (inicio > fim)
            return inicio;

      int meio = inicio + (fim - inicio) / 2;
      int comparacao = strcmp(clientes[meio].cpf, cpf_procurado);

      if (comparacao == 0)
            return -1;
      else if (comparacao < 0)
            return busca_para_inserir_cliente(clientes, meio + 1, fim, cpf_procurado);
      else
            return busca_para_inserir_cliente(clientes, inicio, meio - 1, cpf_procurado);
}

/* Usado para encontrar o identificador de ordenacao de um servico.
 * Retorna a string do identificador.
 * NAO ESQUECER DE DAR FREE() NA STRING RESULTANTO DEPOIS DED USAR.
 */ 
char* buscar_id_servico(char* cpf_fax, char* cpf_cli, char* string_data)
{
      char* identificador;
      identificador = (char*)calloc(MAX_ID_SERV, sizeof(char));

      strcpy(identificador, cpf_fax);
      strcat(identificador, cpf_cli);
      strcat(identificador, string_data);

      return identificador;
}

// Usado para encontrar um cliente no vetor. Retorna -1 se o cliente não está no vetor.
int buscar_servico(servico* servicos, int inicio, int fim, char* identificador)
{
      if (inicio > fim)
            return -1;

      int meio = inicio + (fim - inicio) / 2;
      int comparacao = strcmp(servicos[meio].id, identificador);

      if (comparacao == 0)
            return meio;
      else if (comparacao < 0)
            return buscar_servico(servicos, meio + 1, fim, identificador);
      else 
            return buscar_servico(servicos, inicio, meio - 1, identificador);

}
/* Usado para encontrar o local no vetor para inserir um cliente.
 * Retorna -1 caso o cliente já existe no vetor.
 */
int busca_para_inserir_servico(servico* servicos, int inicio, int fim, char* identificador)
{
      if (inicio > fim)
            return inicio;

      int meio = inicio + (fim - inicio) / 2;
      int comparacao = strcmp(servicos[meio].id, identificador);

      if (comparacao == 0)
            return -1;
      else if (comparacao < 0)
            return busca_para_inserir_servico(servicos, meio + 1, fim, identificador);
      else 
            return busca_para_inserir_servico(servicos, inicio, meio - 1, identificador);
}

//=================================================== Funcoes de Busca ==============//
//
// ================= Menus e Submenus ==================================================

// Usada para imprimir o Menu Principal e retorna a opcao escolhida pelo usuario.
int print_menu()
{
      int opt;
      printf("\n1. Submenu de Faxineiros\n");
      printf("2. Submenu de Clientes\n");
      printf("3. Submenu de Serviços\n");
      printf("4. Submenu Relatórios\n");
      printf("5. Sair\n");
      printf(": ");
      scanf("%d", &opt);

      return opt;
}

// Usada para imprimir o Submenu (Variavel) e retorna a opcao escolhida pelo usuario.
int print_submenu(char *tipo_submenu)
{
      int opt; // Listar todos, Listar um, Incluir (sem repeticao), Alterar e Excluir
      printf("\n1. Listar todos os %ss\n", tipo_submenu);
      printf("2. Listar um %s\n", tipo_submenu);
      printf("3. Incluir um %s\n", tipo_submenu);
      printf("4. Alterar um %s\n", tipo_submenu);
      printf("5. Excluir um %s\n", tipo_submenu);
      printf("6. Voltar\n");
      printf(": ");
      scanf("%d", &opt);

      return opt;
}

// Usada para imprimir o Submenu dos relatorios e retorna a opcao escolhida pelo usuario.
int print_submenu_relatorios()
{
      int opt;
      printf("\n1. Listar clientes de um faxineiro entre datas X até Y\n");
      printf("2. Listar serviços de uma data específica\n");
      printf("3. Listar todos os serviços de um faxineiro (pelo CPF)\n");
      printf("4. Voltar\n");
      printf(": ");
      scanf("%d", &opt);

      return opt;
}
// ================================================== Menus e Submenus =================//
//
// ======================= Carregar e Salvar =============================================

// Usado para carregar os dados dos faxineiros salvos no arquivo
faxineiro* carregar_faxineiros(const char* nome_arquivo, int* retorno_tamanho_vetor)
{
      FILE* arquivo;
      faxineiro* vetor_faxineiros;
      int tamanho_vetor;
      long tamanho_arquivo;

      arquivo = fopen(nome_arquivo, "rb+");
      if (arquivo == NULL)
      {
            perror("Erro ao abrir o arquivo");
            exit(EXIT_FAILURE);
      }

      fseek(arquivo, 0, SEEK_END);
      tamanho_arquivo = ftell(arquivo);
      tamanho_vetor = (int)tamanho_arquivo / sizeof(faxineiro);
      rewind(arquivo);

      vetor_faxineiros = (faxineiro *)calloc(tamanho_vetor * 2, sizeof(faxineiro));
      if (vetor_faxineiros == NULL)
      {
            perror("Erro ao disponibilizar memória para os dados");
            fclose(arquivo);
            exit(EXIT_FAILURE);
      }
      if (fread(vetor_faxineiros, sizeof(faxineiro), tamanho_vetor, arquivo) != tamanho_vetor)
      {
            perror("Erro ao carregar os dados");
            fclose(arquivo);
            free(vetor_faxineiros);
            exit(EXIT_FAILURE);
      }
      fclose(arquivo);
      *retorno_tamanho_vetor = tamanho_vetor;
      return vetor_faxineiros;
}

// Usado para carregar os dados dos clientes salvos no arquivo
cliente* carregar_clientes(const char* nome_arquivo, int* retorno_tamanho_vetor)
{
      FILE* arquivo;
      cliente* vetor_clientes;
      int tamanho_vetor;
      long tamanho_arquivo;

      arquivo = fopen(nome_arquivo, "rb+");
      if (arquivo == NULL)
      {
            perror("Erro ao abrir o arquivo");
            exit(EXIT_FAILURE);
      }

      fseek(arquivo, 0, SEEK_END);
      tamanho_arquivo = ftell(arquivo);
      tamanho_vetor = (int)tamanho_arquivo / sizeof(cliente);
      rewind(arquivo);

      vetor_clientes = (cliente *)calloc(tamanho_vetor * 2, sizeof(cliente));
      if (vetor_clientes == NULL)
      {
            perror("Erro ao disponibilizar memória para os dados");
            fclose(arquivo);
            exit(EXIT_FAILURE);
      }
      if (fread(vetor_clientes, sizeof(cliente), tamanho_vetor, arquivo) != tamanho_vetor)
      {
            perror("Erro ao carregar os dados");
            fclose(arquivo);
            free(vetor_clientes);
            exit(EXIT_FAILURE);
      }
      fclose(arquivo);
      *retorno_tamanho_vetor = tamanho_vetor;
      return vetor_clientes;
}

// Usado para carregar os dados dos servicos salvos no arquivo
servico* carregar_servicos(const char* nome_arquivo, int* retorno_tamanho_vetor)
{
      FILE* arquivo;
      servico* vetor_servicos;
      int tamanho_vetor;
      long tamanho_arquivo;

      arquivo = fopen(nome_arquivo, "rb+");
      if (arquivo == NULL)
      {
            perror("Erro ao abrir o arquivo");
            exit(EXIT_FAILURE);
      }
      fseek(arquivo, 0, SEEK_END);
      tamanho_arquivo = ftell(arquivo);
      tamanho_vetor = (int)tamanho_arquivo / sizeof(servico);
      rewind(arquivo);

      vetor_servicos = (servico *)calloc(tamanho_vetor * 2, sizeof(servico));
      if (vetor_servicos == NULL)
      {
            perror("Erro ao disponibilizar memória para os dados");
            fclose(arquivo);
            exit(EXIT_FAILURE);
      }
      if (fread(vetor_servicos, sizeof(servico), tamanho_vetor, arquivo) != tamanho_vetor)
      {
            perror("Erro ao carregar os dados");
            fclose(arquivo);
            free(vetor_servicos);
            exit(EXIT_FAILURE);
      }
      fclose(arquivo);
      *retorno_tamanho_vetor = tamanho_vetor;
      return vetor_servicos;
}

// Usada para salvar os dados dos faxineiros no arquivo.
void salvar_faxineiros(faxineiro* vetor_faxineiros, const char* nome_arquivo, int tamanho_vetor)
{
      FILE* arquivo;

      arquivo = fopen(nome_arquivo, "wb+");
      if (arquivo == NULL)
      {
            perror("Erro ao abrir o arquivo");
            exit(EXIT_FAILURE);
      }
      if ((fwrite(vetor_faxineiros, sizeof(faxineiro), tamanho_vetor, arquivo)) != tamanho_vetor)
      {
            perror("Erro ao salvar os dados");
            fclose(arquivo);
            exit(EXIT_FAILURE);
      }
      fclose(arquivo);
}

// Usada para salvar os dados dos clientes no arquivo.
void salvar_clientes(cliente* vetor_clientes, const char* nome_arquivo, int tamanho_vetor)
{
      FILE* arquivo;

      arquivo = fopen(nome_arquivo, "wb+");
      if (arquivo == NULL)
      {
            perror("Erro ao abrir o arquivo");
            exit(EXIT_FAILURE);
      }
      if ((fwrite(vetor_clientes, sizeof(cliente), tamanho_vetor, arquivo)) != tamanho_vetor)
      {
            perror("Erro ao salvar os dados");
            fclose(arquivo);
            exit(EXIT_FAILURE);
      }
      fclose(arquivo);
}

// Usada para salvar os dados dos servicos no arquivo.
void salvar_servicos(servico* vetor_servicos, const char* nome_arquivo, int tamanho_vetor)
{
      FILE* arquivo;

      arquivo = fopen(nome_arquivo, "wb+");
      if (arquivo == NULL)
      {
            perror("Erro ao abrir o arquivo");
            exit(EXIT_FAILURE);
      }
      if ((fwrite(vetor_servicos, sizeof(servico), tamanho_vetor, arquivo)) != tamanho_vetor)
      {
            perror("Erro ao salvar os dados");
            fclose(arquivo);
            exit(EXIT_FAILURE);
      }
      fclose(arquivo);
}
// ======================================================================= Carregar e Salvar ===== //
//
// ===================== Funcoes de Faxineiros ====================================================

// Usado para listar todos os faxineiros no vetor.
void listar_todos_faxineiros(faxineiro *faxineiros, int tamanho_vetor)
{
      int i, j;
      printf("\nTodos os faxineiros:\n");
      for (i = 0; i < tamanho_vetor; i++)
      {
            printf("\nNome: %s\n", faxineiros[i].nome);
            printf("CPF : %s\n", faxineiros[i].cpf);
            printf("RG : %s\n", faxineiros[i].rg);
            printf("Sexo : %c\n", faxineiros[i].sexo);
            printf("Data de nascimento : %02d/%02d/%04d\n",
                  faxineiros[i].data_nascimento.dia,
                  faxineiros[i].data_nascimento.mes,
                  faxineiros[i].data_nascimento.ano);
            printf("Telefones : ");
            for (j = 0; j < faxineiros[i].quantidade_telefones; j++)
                  printf("%s | ", faxineiros[i].telefones[j]);
            printf("\n");
      }
      puts("\n");
}

// Usado para listar um unico faxineiro do vetor. Requer CPF para busca.
void listar_um_faxineiro(faxineiro *faxineiros, int tamanho, char *cpf)
{
      int indice, i;

      indice = buscar_faxineiro(faxineiros, 0, tamanho - 1, cpf);

      if (indice < 0)
      {
            printf("\nO faxineiro não existe / não está cadastrado!\n");
      }
      else
      {
            printf("\nNome: %s\n", faxineiros[indice].nome);
            printf("CPF : %s\n", faxineiros[indice].cpf);
            printf("RG : %s\n", faxineiros[indice].rg);
            printf("Sexo : %c\n", faxineiros[indice].sexo);
            printf("Data de nascimento : %02d/%02d/%04d\n",
                  faxineiros[indice].data_nascimento.dia,
                  faxineiros[indice].data_nascimento.mes,
                  faxineiros[indice].data_nascimento.ano);
            printf("Telefones : ");
            for (i = 0; i < faxineiros[indice].quantidade_telefones; i++)
                  printf("%s | ", faxineiros[indice].telefones[i]);
            printf("\n");
      }
}

/* Usado para adicionar os dados no struct faxineiros.
 * Pode ser usado tanto para adicionar um novo faxineiro, tanto para substituir
 * os dados de um faxineiro já existente.
 */
void adicionar_dados_faxineiro(faxineiro *faxineiros, int indice, char *cpf_a_registrar)
{
      int i;
      char terminador_string;

      strcpy(faxineiros[indice].cpf, cpf_a_registrar);

      printf("Informe o RG: ");
      fgets(faxineiros[indice].rg, sizeof(faxineiros[indice].rg), stdin);
      faxineiros[indice].rg[strcspn(faxineiros[indice].rg, "\n")] = '\0';

      printf("Informe o Nome (Tamanho máximo = %d): ", MAX_NOME - 2);
      fgets(faxineiros[indice].nome, sizeof(faxineiros[indice].nome), stdin);
      faxineiros[indice].nome[strcspn(faxineiros[indice].nome, "\n")] = '\0';

      printf("Informe o Sexo (M ou F): ");
      faxineiros[indice].sexo = fgetc(stdin);
      while ((terminador_string = getchar()) != '\n' && terminador_string != EOF);

      char data_informada[MAX_DATA];
      bool valida;
      data data_nasc;

      do
      {
            printf("Informe a data de nascimento (dd/mm/aaaa): ");
            fgets(data_informada, sizeof(data_informada), stdin);
            data_informada[strcspn(data_informada, "\n")] = '\0';

            valida = converter_data(data_informada, &data_nasc);

            if (!valida)
                  printf("Data inválida!\n");
      } while (!valida);

      faxineiros[indice].data_nascimento = data_nasc;

      printf("Informe a quantidade de telefones (Máximo = %d): ", MAX_QT_TELEFONES);
      if (scanf("%d", &faxineiros[indice].quantidade_telefones) != 1)
            faxineiros[indice].quantidade_telefones = 0;

      while ((terminador_string = getchar()) != '\n' && terminador_string != EOF);

      if (faxineiros[indice].quantidade_telefones < 0)
            faxineiros[indice].quantidade_telefones = 0;
      if (faxineiros[indice].quantidade_telefones > MAX_QT_TELEFONES)
            faxineiros[indice].quantidade_telefones = MAX_QT_TELEFONES;

      for (i = 0; i < faxineiros[indice].quantidade_telefones; i++)
      {
            printf("Telefone %d: ", i + 1);
            fgets(faxineiros[indice].telefones[i], sizeof(faxineiros[indice].telefones[i]), stdin);
            faxineiros[indice].telefones[i][strcspn(faxineiros[indice].telefones[i], "\n")] = '\0';
      }
}

/* Usado para incluir um novo faxineiro já ordenado no vetor.
 * Retorna true se a inclusão for bem sucedida, e false caso contrário.
 * Essa funcao ja realoca a memoria quando necessario.
 */
bool incluir_um_faxineiro(faxineiro **faxineiros, int *tamanho, int *capacidade)
{
      cliente* vetor_clientes;
      int tamanho_clientes;
      const char nome_arquivo_clientes[] = "dados_clientes.dat";
      char cpf_a_registrar[MAX_CPF];
      int indice_inserir;
      int i;
      int cpf_no_local;
      int cpf_entrando;
      int ultimo_cpf;
      char terminador;

      printf("\nInforme o CPF a ser cadastrado: ");
      while ((terminador = getchar()) != '\n' && terminador != EOF);
      fgets(cpf_a_registrar, sizeof(cpf_a_registrar), stdin);
      cpf_a_registrar[strcspn(cpf_a_registrar, "\n")] = '\0';

      if (!arquivo_vazio(nome_arquivo_clientes))
      {
            vetor_clientes = carregar_clientes(nome_arquivo_clientes, &tamanho_clientes);

            if (buscar_cliente(vetor_clientes, 0, tamanho_clientes - 1, cpf_a_registrar) != -1)
            {
                  printf("\nCPF já está cadastrado.\n");
                  free(vetor_clientes);
                  return false;
            }
            free(vetor_clientes);
      }

      if (*tamanho == 0)
      {
            adicionar_dados_faxineiro(*faxineiros, 0, cpf_a_registrar);
            (*tamanho)++;
            return true;
      }
      else
      {
            // Para legibilidade do código
            ultimo_cpf = strcmp((*faxineiros)[(*tamanho) - 1].cpf, cpf_a_registrar);
            cpf_entrando = 0;
            if (ultimo_cpf < cpf_entrando)
            {
                  adicionar_dados_faxineiro(*faxineiros, *tamanho, cpf_a_registrar);
                  (*tamanho)++;
                  return true;
            }
            if ((indice_inserir = busca_para_inserir_faxineiro(*faxineiros, 0, (*tamanho) - 1, cpf_a_registrar)) == -1)
            {
                  printf("\nCPF já está cadastrado.\n");
                  return false;
            }
            else
            {
                  if ((*tamanho - 1) >= *capacidade)
                  {
                        (*capacidade) *= 2;
                        faxineiro *temporario = (faxineiro *)realloc(*faxineiros, (*capacidade) * sizeof(faxineiro));
                        if (temporario == NULL)
                        {
                              perror("Erro ao aumnetar a memoria");
                              free(*faxineiros);
                              exit(EXIT_FAILURE);
                        }
                        else
                              *faxineiros = temporario;
                  }
                  // Para legibilidade do código
                  cpf_no_local = strcmp((*faxineiros)[indice_inserir].cpf, cpf_a_registrar);
                  cpf_entrando = 0;
                  if (cpf_no_local > cpf_entrando)
                  {
                        for (i = (*tamanho); i > indice_inserir; i--)
                              (*faxineiros)[i] = (*faxineiros)[i - 1];

                        adicionar_dados_faxineiro(*faxineiros, indice_inserir, cpf_a_registrar);
                        (*tamanho)++;
                        return true;
                  }
                  else
                  {
                              for (i = *tamanho; i > indice_inserir + 1; i--)
                                    (*faxineiros)[i] = (*faxineiros)[i - 1];

                              adicionar_dados_faxineiro(*faxineiros, indice_inserir + 1, cpf_a_registrar);
                              (*tamanho)++;
                              return true;
                  }
            }
      }
}

/* Usado para alterar os dados de um faxineiro.
 * Retorna true se a alteracao for bem sucedida, e false caso contrário.
 */
bool alterar_faxineiro(faxineiro* vetor_faxineiros, char* cpf, int tamanho)
{
      int indice;
      char confirmar;
      char terminador;

      indice = buscar_faxineiro(vetor_faxineiros, 0, tamanho - 1, cpf);
      if (indice == -1)
      {
            printf("\nO faxineiro não existe\n");
            return false;
      }
      else
      {
            do 
            {
                  listar_um_faxineiro(vetor_faxineiros, tamanho, cpf);
                  printf("\nTem certeza que quer alterar esse faxineiro? ( s / n ): ");
                  confirmar = fgetc(stdin);
                  while ((terminador = getchar()) != '\n' && terminador != EOF);

                  if (confirmar == 's')
                  {
                        adicionar_dados_faxineiro(vetor_faxineiros, indice, cpf);
                        return true;
                  }
                  else if (confirmar == 'n')
                  {
                        printf("\nAlteração cancelada.\n");
                        return false;
                  }
                  else
                        printf("\nOpção inválida.\n");
                       
            } while (confirmar != 'n');
      }
      return false;
}

/* Usado para excluir um faxineiro do vetor.
 * Retorna true se a exclusao for bem sucedida, e false caso contrário.
 */
bool excluir_faxineiro(faxineiro* vetor_faxineiros, char* cpf, int* tamanho)
{
      int indice;
      int i;
      char confirmar;
      char terminador;

      indice = buscar_faxineiro(vetor_faxineiros, 0, (*tamanho) - 1, cpf);
      if (indice == -1)
      {
            printf("\nO faxineiro não existe.\n");
            return false;
      }
      else 
      {
            do 
            {
                  listar_um_faxineiro(vetor_faxineiros, *tamanho, cpf);
                  printf("\nTem certeza que quer excluir esse faxineiro? ( s / n ): ");
                  confirmar = fgetc(stdin);
                  while ((terminador = getchar()) != '\n' && terminador != EOF);

                  if (confirmar == 's')
                  {
                        for (i = indice; i < (*tamanho) - 1; i++)
                        {
                              vetor_faxineiros[indice] = vetor_faxineiros[indice + 1];
                        }
                        (*tamanho)--;
                        return true;
                  }
                  else if (confirmar == 'n')
                  {
                        printf("\nExclusão cancelada.\n");
                        return false;
                  }
                  else
                        printf("\nOpção inválida.\n");
                       
            } while (confirmar != 'n');
      }
      return false;
}
// =========================================================== Funcoes de Faxineiros ==============//
//
// ===================== Funcoes de Clientes ====================================================

// Usado para listar todos os clientes no vetor.
void listar_todos_clientes(cliente* clientes, int tamanho_vetor)
{
      int i, j;
      printf("\nTodos os clientes:\n");
      for (i = 0; i < tamanho_vetor; i++)
      {
            printf("\nNome: %s\n", clientes[i].nome);
            printf("CPF : %s\n", clientes[i].cpf);
            printf("Data de nascimento : %02d/%02d/%04d\n",
                  clientes[i].data_nascimento.dia,
                  clientes[i].data_nascimento.mes,
                  clientes[i].data_nascimento.ano);
            printf("Cidade: %s\n", clientes[i].cidade);
            printf("Endereço: %s\n", clientes[i].endereco);
            printf("CEP: %s\n", clientes[i].cep);
            printf("Emails:\n");
            for (j = 0; j < clientes[i].quantidade_emails; j++)
                  printf("\t%s\n", clientes[i].emails[j]);
            printf("Telefones: ");
            for (j = 0; j < clientes[i].quantidade_telefones; j++)
                  printf("%s | ", clientes[i].telefones[j]);
            printf("\n");
      }
      puts("\n");
}

// Usado para listar um unico cliente do vetor. Requer CPF para busca.
void listar_um_cliente(cliente* clientes, int tamanho, char *cpf)
{
      int indice, i;

      indice = buscar_cliente(clientes, 0, tamanho - 1, cpf);

      if (indice < 0)
      {
            printf("\nO cliente não existe / não está cadastrado!\n");
      }
      else
      {
            printf("\nNome: %s\n", clientes[indice].nome);
            printf("CPF : %s\n", clientes[indice].cpf);
            printf("Data de nascimento : %02d/%02d/%04d\n",
                  clientes[indice].data_nascimento.dia,
                  clientes[indice].data_nascimento.mes,
                  clientes[indice].data_nascimento.ano);
            printf("Cidade: %s\n", clientes[indice].cidade);
            printf("Endereço: %s\n", clientes[indice].endereco);
            printf("CEP: %s\n", clientes[indice].cep);
            printf("Emails:\n");
            for (i = 0; i < clientes[indice].quantidade_emails; i++)
                  printf("\t%s\n", clientes[indice].emails[i]);
            printf("Telefones: ");
            for (i = 0; i < clientes[indice].quantidade_telefones; i++)
                  printf("%s | ", clientes[indice].telefones[i]);
            printf("\n");
      }
}

/* Usado para adicionar os dados no struct cliente.
 * Pode ser usado tanto para adicionar um novo cliente, tanto para substituir
 * os dados de um cliente já existente.
 */
void adicionar_dados_cliente(cliente* clientes, int indice, char* cpf_a_registrar)
{
      int i;
      char terminador_string;

      strcpy(clientes[indice].cpf, cpf_a_registrar);

      printf("Informe o Nome (Tamanho máximo = %d): ", MAX_NOME - 2);
      fgets(clientes[indice].nome, sizeof(clientes[indice].nome), stdin);
      clientes[indice].nome[strcspn(clientes[indice].nome, "\n")] = '\0';

      char data_informada[MAX_DATA];
      bool valida;
      data data_nasc;

      do
      {
            printf("Informe a data de nascimento (dd/mm/aaaa): ");
            fgets(data_informada, sizeof(data_informada), stdin);
            data_informada[strcspn(data_informada, "\n")] = '\0';

            valida = converter_data(data_informada, &data_nasc);

            if (!valida)
                  printf("Data inválida!\n");
      } while (!valida);

      clientes[indice].data_nascimento = data_nasc;

      printf("Informe o endereço: ");
      fgets(clientes[indice].endereco, sizeof(clientes[indice].endereco), stdin);
      clientes[indice].endereco[strcspn(clientes[indice].endereco, "\n")] = '\0';

      printf("Informe o CEP: ");
      fgets(clientes[indice].cep, sizeof(clientes[indice].cep), stdin);
      clientes[indice].cep[strcspn(clientes[indice].cep, "\n")] = '\0';

      printf("Informe a cidade: ");
      fgets(clientes[indice].cidade, sizeof(clientes[indice].cidade), stdin);
      clientes[indice].cidade[strcspn(clientes[indice].cidade, "\n")] = '\0';

      printf("Informe a quantidade de Emails (Máximo = %d): ", MAX_QT_EMAILS);
      if (scanf("%d", &clientes[indice].quantidade_emails) != 1)
            clientes[indice].quantidade_emails = 0;

      while ((terminador_string = getchar()) != '\n' && terminador_string != EOF);

      if (clientes[indice].quantidade_emails < 0)
            clientes[indice].quantidade_emails = 0;
      if (clientes[indice].quantidade_emails > MAX_QT_EMAILS)
            clientes[indice].quantidade_emails = MAX_QT_EMAILS;

      for (i = 0; i < clientes[indice].quantidade_emails; i++)
      {
            printf("Email %d: ", i + 1);
            fgets(clientes[indice].emails[i], sizeof(clientes[indice].emails[i]), stdin);
            clientes[indice].emails[i][strcspn(clientes[indice].emails[i], "\n")] = '\0';
      }

      printf("Informe a quantidade de telefones (Máximo = %d): ", MAX_QT_TELEFONES);
      if (scanf("%d", &clientes[indice].quantidade_telefones) != 1)
            clientes[indice].quantidade_telefones = 0;

      while ((terminador_string = getchar()) != '\n' && terminador_string != EOF);

      if (clientes[indice].quantidade_telefones < 0)
            clientes[indice].quantidade_telefones = 0;
      if (clientes[indice].quantidade_telefones > MAX_QT_TELEFONES)
            clientes[indice].quantidade_telefones = MAX_QT_TELEFONES;

      for (i = 0; i < clientes[indice].quantidade_telefones; i++)
      {
            printf("Telefone %d: ", i + 1);
            fgets(clientes[indice].telefones[i], sizeof(clientes[indice].telefones[i]), stdin);
            clientes[indice].telefones[i][strcspn(clientes[indice].telefones[i], "\n")] = '\0';
      }
}

/* Usado para incluir um novo cliente já ordenado no vetor.
 * Retorna true se a inclusão for bem sucedida, e false caso contrário.
 * Essa funcao ja realoca a memoria quando necessario.
 */
bool incluir_um_cliente(cliente **clientes, int *tamanho, int *capacidade)
{
      faxineiro* vetor_faxineiros;
      int tamanho_faxineiros;
      const char nome_arquivo_faxineiros[] = "dados_faxineiros.dat";
      char cpf_a_registrar[MAX_CPF];
      int indice_inserir;
      int i;
      int cpf_no_local;
      int cpf_entrando;
      int ultimo_cpf;
      char terminador;

      printf("\nInforme o CPF a ser cadastrado: ");
      while ((terminador = getchar()) != '\n' && terminador != EOF);
      fgets(cpf_a_registrar, sizeof(cpf_a_registrar), stdin);
      cpf_a_registrar[strcspn(cpf_a_registrar, "\n")] = '\0';

      if (!arquivo_vazio(nome_arquivo_faxineiros))
      {
            vetor_faxineiros = carregar_faxineiros(nome_arquivo_faxineiros, &tamanho_faxineiros);

            if (buscar_faxineiro(vetor_faxineiros, 0, tamanho_faxineiros - 1, cpf_a_registrar) != -1)
            {
                  printf("\nCPF já está cadastrado.\n");
                  free(vetor_faxineiros);
                  return false;
            }
            free(vetor_faxineiros);
      }

      if (*tamanho == 0)
      {
            adicionar_dados_cliente(*clientes, 0, cpf_a_registrar);
            (*tamanho)++;
            return true;
      }
      else
      {
            // Para legibilidade do código
            ultimo_cpf = strcmp((*clientes)[(*tamanho) - 1].cpf, cpf_a_registrar);
            cpf_entrando = 0;
            if (ultimo_cpf < cpf_entrando)
            {
                  adicionar_dados_cliente(*clientes, *tamanho, cpf_a_registrar);
                  (*tamanho)++;
                  return true;
            }
            if ((indice_inserir = busca_para_inserir_cliente(*clientes, 0, (*tamanho) - 1, cpf_a_registrar)) == -1)
            {
                  printf("\nCPF já está cadastrado.\n");
                  return false;
            }
            else
            {
                  if ((*tamanho) - 1 >= *capacidade)
                  {
                        (*capacidade) *= 2;
                        cliente *temporario = (cliente *)realloc(*clientes, (*capacidade) * sizeof(cliente));
                        if (temporario == NULL)
                        {
                              perror("Erro ao aumnetar a memoria");
                              free(*clientes);
                              exit(EXIT_FAILURE);
                        }
                        else
                              *clientes = temporario;
                  }
                  // Para legibilidade do código
                  cpf_no_local = strcmp((*clientes)[indice_inserir].cpf, cpf_a_registrar);
                  cpf_entrando = 0;
                  if (cpf_no_local > cpf_entrando)
                  {
                        for (i = (*tamanho); i > indice_inserir; i--)
                              (*clientes)[i] = (*clientes)[i - 1];

                        adicionar_dados_cliente(*clientes, indice_inserir, cpf_a_registrar);
                        (*tamanho)++;
                        return true;
                  }
                  else
                  {
                              for (i = *tamanho; i > indice_inserir + 1; i--)
                                    (*clientes)[i] = (*clientes)[i - 1];

                              adicionar_dados_cliente(*clientes, indice_inserir + 1, cpf_a_registrar);
                              (*tamanho)++;
                              return true;
                  }
            }
      }
}

/* Usado para alterar os dados de um cliente.
 * Retorna true se a alteracao for bem sucedida, e false caso contrário.
 */
bool alterar_cliente(cliente* vetor_clientes, char* cpf, int tamanho)
{
      int indice;
      char confirmar;
      char terminador;

      indice = buscar_cliente(vetor_clientes, 0, tamanho - 1, cpf);
      if (indice == -1)
      {
            printf("\nO cliente não existe\n");
            return false;
      }
      else
      {
            do 
            {
                  listar_um_cliente(vetor_clientes, tamanho, cpf);
                  printf("\nTem certeza que quer alterar esse cliente? ( s / n ): ");
                  confirmar = fgetc(stdin);
                  while ((terminador = getchar()) != '\n' && terminador != EOF);

                  if (confirmar == 's')
                  {
                        adicionar_dados_cliente(vetor_clientes, indice, cpf);
                        return true;
                  }
                  else if (confirmar == 'n')
                  {
                        printf("\nAlteração cancelada.\n");
                        return false;
                  }
                  else
                        printf("\nOpção inválida.\n");
                       
            } while (confirmar != 'n');
      }
      return false;
}

/* Usado para excluir um cliente do vetor.
 * Retorna true se a exclusao for bem sucedida, e false caso contrário.
 */
bool excluir_cliente(cliente* vetor_clientes, char* cpf, int* tamanho)
{
      int indice;
      int i;
      char confirmar;
      char terminador;

      indice = buscar_cliente(vetor_clientes, 0, (*tamanho) - 1, cpf);
      if (indice == -1)
      {
            printf("\nO cliente não existe.\n");
            return false;
      }
      else 
      {
            do 
            {
                  listar_um_cliente(vetor_clientes, *tamanho, cpf);
                  printf("\nTem certeza que quer excluir esse cliente? ( s / n ): ");
                  confirmar = fgetc(stdin);
                  while ((terminador = getchar()) != '\n' && terminador != EOF);

                  if (confirmar == 's')
                  {
                        for (i = indice; i < (*tamanho) - 1; i++)
                        {
                              vetor_clientes[indice] = vetor_clientes[indice + 1];
                        }
                        (*tamanho)--;
                        return true;
                  }
                  else if (confirmar == 'n')
                  {
                        printf("\nExclusão cancelada.\n");
                        return false;
                  }
                  else
                        printf("\nOpção inválida.\n");
                       
            } while (confirmar != 'n');
      }
      return false;
}
// =========================================================== Funcoes de Clientes ==============//
//
// ===================== Funcoes de Servicos ====================================================

/* Usado para listar todos os servicos no vetor.
 * Tamanhos é um vetor com os tamanhos dos vetores.
 * Ordem dos tamanhos: [tamanho_servico, tamanho_faxineiro, tamanho_cliente].
 */
void listar_todos_servicos(servico* servicos, faxineiro* faxineiros, cliente* clientes, int* tamanhos)
{
      int i, j;
      printf("\nTodos os servicos:\n");
      for (i = 0; i < tamanhos[0]; i++)
      {
            int indice_faxineiro = buscar_faxineiro(faxineiros, 0, tamanhos[1] - 1, servicos[i].cpf_faxineiro);
            int indice_cliente = buscar_cliente(clientes, 0, tamanhos[2] - 1, servicos[i].cpf_cliente);

            if (indice_faxineiro == -1)
            {
                  printf("\nFaxineiro Removido!\n");
            }
            else
            {
                  printf("Nome do faxineiro(a): %s\n", faxineiros[indice_faxineiro].nome);
                  printf("\nCPF do faxineiro: %s\n", servicos[i].cpf_faxineiro);
            }

            if (indice_cliente == -1)
            {
                  printf("Cliente Removido!\n");
            }
            else 
            {
                  printf("Nome do cliente: %s\n", clientes[indice_cliente].nome);
                  printf("CPF do cliente: %s\n", servicos[i].cpf_cliente);
            }
            
            printf("Data do serviço: %02d/%02d/%04d\n",
                  servicos[i].data.dia,
                  servicos[i].data.mes,
                  servicos[i].data.ano);
            printf("Valor do servico: %.2f\n", servicos[i].valor);
      }
      puts("\n");
}

/* Usado para listar um unico servico no vetor.
 * Tamanhos é um vetor com os tamanhos dos vetores.
 * Ordem dos tamanhos: [tamanho_servico, tamanho_faxineiro, tamanho_cliente].
 * Requer identificador para busca.
 */
void listar_um_servico(servico* servicos, faxineiro* faxineiros, cliente* clientes,
                        int* tamanhos, char* identificador)
{
      int indice, i;

      indice = buscar_servico(servicos, 0, tamanhos[0] - 1, identificador);

      if (indice < 0)
      {
            printf("\nO servico não existe / não está cadastrado!\n");
      }
      else
      {
            int indice_faxineiro = buscar_faxineiro(faxineiros, 0, tamanhos[1] - 1, servicos[indice].cpf_faxineiro);
            int indice_cliente = buscar_cliente(clientes, 0, tamanhos[2] - 1, servicos[indice].cpf_cliente);

            if (indice_faxineiro == -1)
            {
                  printf("\nFaxineiro Removido!\n");
            }
            else
            {
                  printf("Nome do faxineiro(a): %s\n", faxineiros[indice_faxineiro].nome);
                  printf("\nCPF do faxineiro: %s\n", servicos[indice].cpf_faxineiro);
            }

            if (indice_cliente == -1)
            {
                  printf("Cliente Removido!\n");
            }
            else 
            {
                  printf("Nome do cliente: %s\n", clientes[indice_cliente].nome);
                  printf("CPF do cliente: %s\n", servicos[indice].cpf_cliente);
            }

            printf("Data do serviço: %02d/%02d/%04d\n",
                  servicos[indice].data.dia,
                  servicos[indice].data.mes,
                  servicos[indice].data.ano);
            printf("Valor do servico: %.2f\n", servicos[indice].valor);
      }
      puts("\n");
}

/* Usado para adicionar os dados no struct cliente.
 * Pode ser usado tanto para adicionar um novo cliente, tanto para substituir
 * os dados de um cliente já existente.
 */
void adicionar_dados_servico(servico* servicos, char* identificador, int indice,
                              char* cpf_faxineiro, char* cpf_cliente, data data_)
{
      char terminador;

      strcpy(servicos[indice].id, identificador);
      strcpy(servicos[indice].cpf_faxineiro, cpf_faxineiro);
      strcpy(servicos[indice].cpf_cliente, cpf_cliente);

      servicos[indice].data = data_;

      printf("Informe o valor do serviço: ");
      scanf("%f", &servicos[indice].valor);

      while ((terminador = getchar()) != '\n' && terminador != EOF);
}

/* Usado para incluir um novo servico já ordenado no vetor.
 * Retorna true se a inclusão for bem sucedida, e false caso contrário.
 * Essa funcao ja realoca a memoria quando necessario.
 */
bool incluir_um_servico(servico* servicos, char* identificador, char* cpf_faxineiro, char* cpf_cliente,
                        data data_, int* tamanho, int* capacidade)
{
      int indice_inserir;
      int i;
      int cpf_no_local;
      int cpf_entrando;
      int ultimo_cpf;
      char terminador;


      if (*tamanho == 0)
      {
            adicionar_dados_servico(servicos, identificador, 0, cpf_faxineiro, cpf_cliente, data_);
            (*tamanho)++;
            return true;
      }
      else
      {
            // Para legibilidade do código
            ultimo_cpf = strcmp(servicos[(*tamanho) - 1].id, identificador);
            cpf_entrando = 0;
            if (ultimo_cpf < cpf_entrando)
            {
                  adicionar_dados_servico(servicos, identificador, *tamanho, cpf_faxineiro, cpf_cliente, data_);
                  (*tamanho)++;
                  return true;
            }
            if ((indice_inserir = busca_para_inserir_servico(servicos, 0, (*tamanho) - 1, identificador)) == -1)
            {
                  printf("\nServiço já está cadastrado.\n");
                  return false;
            }
            else
            {
                  if ((*tamanho) - 1 >= *capacidade)
                  {
                        (*capacidade) *= 2;
                        servico *temporario = (servico *)realloc(servicos, (*capacidade) * sizeof(servico));
                        if (temporario == NULL)
                        {
                              perror("Erro ao aumnetar a memoria");
                              free(servicos);
                              exit(EXIT_FAILURE);
                        }
                        else
                              servicos = temporario;
                  }
                  // Para legibilidade do código
                  cpf_no_local = strcmp(servicos[indice_inserir].id, identificador);
                  cpf_entrando = 0;
                  if (cpf_no_local > cpf_entrando)
                  {
                        for (i = (*tamanho); i > indice_inserir; i--)
                              servicos[i] = servicos[i - 1];

                        adicionar_dados_servico(servicos, identificador, indice_inserir, cpf_faxineiro, cpf_cliente, data_);
                        (*tamanho)++;
                        return true;
                  }
                  else
                  {
                              for (i = *tamanho; i > indice_inserir + 1; i--)
                                    servicos[i] = servicos[i - 1];

                              adicionar_dados_servico(servicos, identificador, indice_inserir + 1, cpf_faxineiro, cpf_cliente, data_);
                              (*tamanho)++;
                              return true;
                  }
            }
      }
}

/* Usado para alterar os dados de um servico.
 * Retorna true se a alteracao for bem sucedida, e false caso contrário.
 */
bool alterar_servico(servico* servicos, faxineiro* faxineiros, cliente* clientes, char* identificador, int* tamanhos)
{
      int indice;
      char confirmar;
      char terminador;

      indice = buscar_servico(servicos, 0, tamanhos[0] - 1, identificador);
      if (indice == -1)
      {
            printf("\nO servico não existe\n");
            return false;
      }
      else
      {
            do 
            {
                  listar_um_servico(servicos, faxineiros, clientes, tamanhos, identificador);
                  printf("\nTem certeza que quer alterar esse servico? ( s / n ): ");
                  confirmar = fgetc(stdin);
                  while ((terminador = getchar()) != '\n' && terminador != EOF);

                  if (confirmar == 's')
                  {
                        adicionar_dados_servico(servicos, identificador, indice, servicos[indice].cpf_faxineiro,
                              servicos[indice].cpf_cliente, servicos[indice].data);
                        return true;
                  }
                  else if (confirmar == 'n')
                  {
                        printf("\nAlteração cancelada.\n");
                        return false;
                  }
                  else
                        printf("\nOpção inválida.\n");
                       
            } while (confirmar != 'n');
      }
      return false;
}

/* Usado para excluir um servico do vetor.
 * Retorna true se a exclusao for bem sucedida, e false caso contrário.
 */
bool excluir_servico(servico* servicos, faxineiro* faxineiros, cliente* clientes, char* identificador, int* tamanhos)
{
      int indice;
      int i;
      char confirmar;
      char terminador;

      indice = buscar_servico(servicos, 0, tamanhos[0] - 1, identificador);
      if (indice == -1)
      {
            printf("\nO servico não existe.\n");
            return false;
      }
      else 
      {
            do 
            {
                  listar_um_servico(servicos, faxineiros, clientes, tamanhos, identificador);
                  printf("\nTem certeza que quer excluir esse servico? ( s / n ): ");
                  confirmar = fgetc(stdin);
                  while ((terminador = getchar()) != '\n' && terminador != EOF);

                  if (confirmar == 's')
                  {
                        for (i = indice; i < tamanhos[0] - 1; i++)
                        {
                              servicos[indice] = servicos[indice + 1];
                        }
                        tamanhos[0]--;
                        return true;
                  }
                  else if (confirmar == 'n')
                  {
                        printf("\nExclusão cancelada.\n");
                        return false;
                  }
                  else
                        printf("\nOpção inválida.\n");
                       
            } while (confirmar != 'n');
      }
      return false;
}
// =========================================================== Funcoes de Servicos ==============//
//
// ===================== Funcoes de Relatorios ====================================================

/* Usada para mostrar o relatorio dos clientes atendidos 
 * por um faxineiro X entre as datas Y e Z.
 */
bool atendidos_entre_datas(cliente *vetor_clientes, servico *vetor_servicos, int *tamanhos,
      char *cpf_informado, char *data_inicial, char *data_final, const char *nome_arquivo)
{
      FILE *arquivo;
      servico *servicos_do_faxineiro;
      cliente *clientes_ja_escolhidos;

      int tamanho_serv_faxineiro = 0;
      int tamanho_cliente_escolhido = 0;
      int eh_do_faxineiro_informado;
      int esta_entre_datas;
      int i, j;

      char data_inicial_converter[MAX_DATA];
      char data_final_converter[MAX_DATA];

      data inicial;
      data final;

      strcpy(data_inicial_converter, data_inicial);
      strcpy(data_final_converter, data_final);

      converter_data(data_inicial_converter, &inicial);
      converter_data(data_final_converter, &final);

      servicos_do_faxineiro = (servico *)calloc(tamanhos[0], sizeof(servico));
      if (servicos_do_faxineiro == NULL)
      {
            perror("Erro de memória");
            exit(EXIT_FAILURE);
      }
      clientes_ja_escolhidos = (cliente *)calloc(tamanhos[2], sizeof(cliente));
      if (clientes_ja_escolhidos == NULL)
      {
            perror("Erro de memória");
            exit(EXIT_FAILURE);
      }

      for (i = 0; i < tamanhos[0]; i++)
      {
            eh_do_faxineiro_informado = (strcmp(vetor_servicos[i].cpf_faxineiro, cpf_informado) == 0);
            esta_entre_datas = data_esta_no_intervalo(vetor_servicos[i].data, inicial, final);

            if (eh_do_faxineiro_informado && esta_entre_datas)
            {
                  servicos_do_faxineiro[tamanho_serv_faxineiro] = vetor_servicos[i];
                  tamanho_serv_faxineiro++;
            }
      }

      int indice;
      bool ja_foi_escolhido;

      if (tamanho_serv_faxineiro > 0)
      {
            indice = buscar_cliente(vetor_clientes, 0, tamanhos[2] - 1, servicos_do_faxineiro[0].cpf_cliente);
            if (indice == -1)
            {
                  printf("\nErro: cliente foi removido.\n");
                  return false;
            }

            clientes_ja_escolhidos[0] = vetor_clientes[indice];
            tamanho_cliente_escolhido++;

            for (i = 1; i < tamanho_serv_faxineiro; i++)
            {
                  indice = buscar_cliente(vetor_clientes, 0, tamanhos[2] - 1, servicos_do_faxineiro[i].cpf_cliente);
                  if (indice == -1)
                  {
                        printf("\nErro: cliente foi removido.\n");
                        return false;
                  }

                  ja_foi_escolhido = false;
                  for (j = 0; j < tamanho_cliente_escolhido; j++)
                  {
                        if (strcmp(vetor_clientes[indice].cpf, clientes_ja_escolhidos[j].cpf) == 0)
                        {
                              ja_foi_escolhido = true;
                              break;
                        }
                  }

                  if (!ja_foi_escolhido)
                  {
                        clientes_ja_escolhidos[tamanho_cliente_escolhido] = vetor_clientes[indice];
                        tamanho_cliente_escolhido++;
                  }
            }

            arquivo = fopen(nome_arquivo, "a+");
            if (arquivo == NULL)
            {
                  perror("Erro ao abrir o arquivo");
                  free(servicos_do_faxineiro);
                  free(clientes_ja_escolhidos);
                  return false;
            }

            fputs("\nClientes atendidos pelo CPF ", arquivo);
            fputs(cpf_informado, arquivo);
            fputs(" entre as datas ", arquivo);
            fputs(data_inicial, arquivo);
            fputs(" e ", arquivo);
            fputs(data_final, arquivo);
            fputs("\n", arquivo);

            printf("\nClientes atendidos pelo CPF %s entre as datas %s e %s\n",
                  cpf_informado, data_inicial, data_final);

            for (i = 0; i < tamanho_cliente_escolhido; i++)
            {
                  fputs("\nNome : ", arquivo);
                  fputs(clientes_ja_escolhidos[i].nome, arquivo);
                  fputs("\n", arquivo);
                  fputs("Emails : ", arquivo);

                  printf("\nNome : %s\n", clientes_ja_escolhidos[i].nome);
                  printf("Emails : ");

                  for (j = 0; j < clientes_ja_escolhidos[i].quantidade_emails; j++)
                  {
                        fputs(clientes_ja_escolhidos[i].emails[j], arquivo);
                        fputs(" , ", arquivo);

                        printf("%s , ", clientes_ja_escolhidos[i].emails[j]);
                  }
                  fputs("\n", arquivo);
                  printf("\n");

                  fputs("Telefones : ", arquivo);
                  printf("Telefones : ");

                  for (j = 0; j < clientes_ja_escolhidos[i].quantidade_telefones; j++)
                  {
                        fputs(clientes_ja_escolhidos[i].telefones[j], arquivo);
                        fputs(" , ", arquivo);

                        printf("%s , ", clientes_ja_escolhidos[i].telefones[j]);
                  }
                  fputs("\n", arquivo);
                  printf("\n");
            }
            fclose(arquivo);
      }
      else
      {
            printf("\nNão há nenhum serviço do faxineiro informado entre as datas informadas.\n");
      }
      free(servicos_do_faxineiro);
      free(clientes_ja_escolhidos);
      
      return true;
}

bool servicos_data_especifica(servico *servicos, faxineiro *faxineiros, cliente *clientes,
      int *tamanhos, char *data_especifica_string, const char *nome_arquivo)
{
      FILE *arquivo;
      servico *servicos_data;
      int tamanho_serv_data = 0;

      char data_string[2 * MAX_DATA];
      char string_valor[16];
      int indice;
      int i;

      servicos_data = (servico *)calloc(tamanhos[0], sizeof(servico));
      if (servicos_data == NULL)
      {
            perror("Erro de memória");
            exit(EXIT_FAILURE);
      }

      for (i = 0; i < tamanhos[0]; i++)
      {
            converte_data_para_string(servicos[i].data, data_string);

            if (strcmp(data_string, data_especifica_string) == 0)
            {
                  servicos_data[tamanho_serv_data] = servicos[i];
                  tamanho_serv_data++;
            }
      }

      if (tamanho_serv_data > 0)
      {
            arquivo = fopen(nome_arquivo, "a+");
            if (arquivo == NULL)
            {
                  perror("Erro ao abrir o arquivo");
                  free(servicos_data);
                  return false;
            }

            fputs("\nServicos contratados na data ", arquivo);
            fputs(data_especifica_string, arquivo);
            fputs("\n",arquivo);

            printf("\nServicos contratados na data %s\n", data_especifica_string);

            for (i = 0; i < tamanho_serv_data; i++)
            {
                  indice = buscar_faxineiro(faxineiros, 0, tamanhos[1] - 1, servicos_data[i].cpf_faxineiro);
                  
                  fputs("\nServiço:\n\tFaxineiro: ", arquivo);
                  printf("\nServiço:\n\tFaxineiro: ");
                  
                  if (indice == -1)
                  {
                        printf("Faxineiro Removido!\n");
                        fputs("Faxineiro removido!\n", arquivo);
                  }
                  else
                  {
                        fputs(faxineiros[indice].nome, arquivo);
                        fputs(" (CPF ", arquivo);
                        fputs(faxineiros[indice].cpf, arquivo);
                        fputs(")\n", arquivo);

                        printf("%s (CPF %s)\n", faxineiros[indice].nome, faxineiros[indice].cpf);
                  }

                  indice = buscar_cliente(clientes, 0, tamanhos[2] - 1, servicos_data[i].cpf_cliente);

                  fputs("\tCliente: ", arquivo);
                  printf("\tCliente: ");

                  if (indice == -1)
                  {
                        printf("Cliente Removido!\n");
                        fputs("Cliente removido!\n", arquivo);
                  }
                  else
                  {
                        fputs(clientes[indice].nome, arquivo);
                        fputs(" (CPF ", arquivo);
                        fputs(clientes[indice].cpf, arquivo);
                        fputs(")\n", arquivo);

                        printf("%s (CPF %s)\n", clientes[indice].nome, clientes[indice].cpf);
                  }

                  snprintf(string_valor, sizeof(string_valor), "\tValor: %.2f\n", servicos_data[i].valor);
                  fputs(string_valor, arquivo);
                  
                  printf("\tValor: %.2f\n", servicos_data[i].valor);
            }
            fclose(arquivo);
      }
      else
      {
            printf("\nNão há nenhum serviço na data informada.\n");
      }
      free(servicos_data);
      return true;
}

bool servicos_por_faxineiro(servico *servicos, cliente *clientes, char *cpf_faxineiro, int *tamanhos, const char *nome_arquivo)
{
      FILE *arquivo;
      servico *servicos_do_faxineiro;

      int tamanho_serv_faxineiro = 0;
      int indice;
      int i;

      char data_string[MAX_DATA];
      char string_valor[16];

      servicos_do_faxineiro = (servico *)calloc(tamanhos[0], sizeof(servico));
      if (servicos_do_faxineiro == NULL)
      {
            perror("Erro de memória");
            exit(EXIT_FAILURE);
      }

      for (i = 0; i < tamanhos[0]; i++)
      {
            if (strcmp(servicos[i].cpf_faxineiro, cpf_faxineiro) == 0)
            {
                  servicos_do_faxineiro[tamanho_serv_faxineiro] = servicos[i];
                  tamanho_serv_faxineiro++;
            }
      }

      if (tamanho_serv_faxineiro > 0)
      {
            arquivo = fopen(nome_arquivo, "a+");
            if (arquivo == NULL)
            {
                  perror("Erro ao abrir o arquivo");
                  free(servicos_do_faxineiro);
                  return false;
            }

            fputs("\nServicos realizados por CPF: ", arquivo);
            fputs(cpf_faxineiro, arquivo);
            fputs("\n",arquivo);

            printf("\nServicos realizados por CPF: %s\n", cpf_faxineiro);

            for (i = 0; i < tamanho_serv_faxineiro; i++)
            {
                  converte_data_para_string(servicos_do_faxineiro[i].data, data_string);

                  fputs("\nServiço realizado em ", arquivo);
                  fputs(data_string, arquivo);
                  fputs("\n", arquivo);

                  printf("\nServiço realizado em %s\n", data_string);

                  indice = buscar_cliente(clientes, 0, tamanhos[2] - 1, servicos_do_faxineiro[i].cpf_cliente);

                  fputs("\tCliente: ", arquivo);
                  printf("\tCliente: ");

                  if (indice == -1)
                  {
                        printf("Cliente Removido!\n");
                        fputs("Cliente removido!\n", arquivo);
                  }
                  else
                  {
                        fputs(clientes[indice].nome, arquivo);
                        fputs(" (CPF ", arquivo);
                        fputs(clientes[indice].cpf, arquivo);
                        fputs(")\n", arquivo);

                        printf("%s (CPF %s)\n", clientes[indice].nome, clientes[indice].cpf);
                  }

                  snprintf(string_valor, sizeof(string_valor), "\tValor: %.2f\n", servicos_do_faxineiro[i].valor);
                  fputs(string_valor, arquivo);
                  
                  printf("\tValor: %.2f\n", servicos_do_faxineiro[i].valor);
            }
            fclose(arquivo);
      }
      else
      {
            printf("\nNão há nenhum serviço realizado pelo faxineiro informado.\n");
      }

      free(servicos_do_faxineiro);
      return true;
}

// =========================================================== Funcoes de Relatorios ==============//
//
// ============================= Mains =============================================================

/* Usado para gerenciar o submenu de faxineiros.
 * Os dados sao locais apenas, e podem ser salvos em arquivos para evitar perdas.
 */
void main_faxineiros()
{
      faxineiro* vetor_faxineiros;

      int opcao;
      int tamanho_vetor;
      int capacidade;

      char cpf[MAX_CPF];
      char continuar;
      char terminador;

      const char nome_arquivo[] = "dados_faxineiros.dat";

      if (arquivo_vazio(nome_arquivo))
      {
            vetor_faxineiros = (faxineiro *)calloc(CAPACIDADE_FAX_VAZIO, sizeof(faxineiro));
            tamanho_vetor = 0;
            capacidade = CAPACIDADE_FAX_VAZIO;
      }
      else
      {
            vetor_faxineiros = carregar_faxineiros(nome_arquivo, &tamanho_vetor);
            capacidade = tamanho_vetor * 2;
      }
      do
      {
            opcao = print_submenu("faxineiro");
            switch (opcao)
            {
            case 1:
                  // listar todos
                  if (tamanho_vetor == 0)
                        printf("\nOps, não tem nada aqui!\n");

                  else
                        listar_todos_faxineiros(vetor_faxineiros, tamanho_vetor);

                  printf("\nEnter para continuar...");
                  while ((terminador = getchar()) != '\n' && terminador != EOF);
                  continuar = fgetc(stdin);
                  break;
            case 2:
                  // listar um
                  if (tamanho_vetor == 0)
                        printf("\nOps, não tem nada aqui!\n");
                  else
                  {
                        printf("\nInforme o cpf do faxineiro a ser listado: ");
                        while ((terminador = getchar()) != '\n' && terminador != EOF);
                        fgets(cpf, sizeof(cpf), stdin);
                        cpf[strcspn(cpf, "\n")] = '\0';
                        listar_um_faxineiro(vetor_faxineiros, tamanho_vetor, cpf);
                  }
                  printf("\nEnter para continuar...");
                  continuar = fgetc(stdin);
                  break;
            case 3:
                  // incluir um (sem repeticao)
                  if (incluir_um_faxineiro(&vetor_faxineiros, &tamanho_vetor, &capacidade))
                        printf("\nFaxineiro incluído com sucesso!\n");
                  else
                        printf("\nNão foi possível incluir o faxineiro.\n");

                  printf("\nEnter para continuar...");
                  continuar = fgetc(stdin);
                  break;
            case 4:
                  // alterar um faxineiro
                  if (tamanho_vetor == 0)
                        printf("\nOps, não tem nada aqui!\n");
                  else
                  {
                        printf("\nInforme o cpf do faxineiro a ser alterado: ");
                        while ((terminador = getchar()) != '\n' && terminador != EOF);
                        fgets(cpf, sizeof(cpf), stdin);
                        cpf[strcspn(cpf, "\n")] = '\0';
                        if (alterar_faxineiro(vetor_faxineiros, cpf, tamanho_vetor))
                              printf("\nFaxineiro alterado com sucesso!\n");
                        else 
                              printf("\nNão foi possível alterar o faxineiro.\n");
                  }
                  printf("\nEnter para continuar...");
                  continuar = fgetc(stdin);
                  break;
            case 5:
                  // excluir um faxineiro
                  if (tamanho_vetor == 0)
                        printf("\nOps, não tem nada aqui!\n");
                  else
                  {
                        printf("\nInforme o cpf do faxineiro a ser excluído: ");
                        while ((terminador = getchar()) != '\n' && terminador != EOF);
                        fgets(cpf, sizeof(cpf), stdin);
                        cpf[strcspn(cpf, "\n")] = '\0';
                        if (excluir_faxineiro(vetor_faxineiros, cpf, &tamanho_vetor))
                              printf("\nFaxineiro excluído com sucesso!\n");
                        else 
                              printf("\nNão foi possível excluir o faxineiro.\n");
                  }
                  printf("\nEnter para continuar...");
                  continuar = fgetc(stdin);
                  break;
            case 6:
                  printf("\nVoltando...\n");
                  break;
            default:
                  printf("\nOpção inválida\n");
                  break;
            }

      } while (opcao != 6);

      char salvar;
      do
      {
            while ((terminador = getchar()) != '\n' && terminador != EOF);
            printf("\nSalvar dados?: (s / n): ");
            salvar = fgetc(stdin);
            switch (salvar)
            {
            case 's':
                  // salva
                  printf("Salvando...\n");
                  salvar_faxineiros(vetor_faxineiros, nome_arquivo, tamanho_vetor);
                  break;
            case 'n':
                  printf("Saindo sem salvar...\n");
                  break;
            default:
                  printf("Opção inválida.\n");
                  break;
            }
      } while (!(salvar == 'n' || salvar == 's'));

      if (vetor_faxineiros != NULL)
      {
            free(vetor_faxineiros);
            vetor_faxineiros = NULL;
      }
}

/* Usado para gerenciar o submenu de clientes.
 * Os dados sao locais apenas, e podem ser salvos em arquivos para evitar perdas.
 */
void main_clientes()
{
      cliente* vetor_clientes;

      int opcao;
      int tamanho_vetor;
      int capacidade;

      char cpf[MAX_CPF];
      char continuar;
      char terminador;

      const char nome_arquivo[] = "dados_clientes.dat";

      if (arquivo_vazio(nome_arquivo))
      {
            vetor_clientes = (cliente *)calloc(CAPACIDADE_CLI_VAZIO, sizeof(cliente));
            tamanho_vetor = 0;
            capacidade = CAPACIDADE_CLI_VAZIO;
      }           
      else
      {
            vetor_clientes = carregar_clientes(nome_arquivo, &tamanho_vetor);
            capacidade = tamanho_vetor * 2;
      }
      do
      {
            opcao = print_submenu("cliente");
            switch (opcao)
            {
            case 1:
                  // listar todos
                  if (tamanho_vetor == 0)
                        printf("\nOps, não tem nada aqui!\n");

                  else
                        listar_todos_clientes(vetor_clientes, tamanho_vetor);

                  printf("\nEnter para continuar...");
                  while ((terminador = getchar()) != '\n' && terminador != EOF);
                  continuar = fgetc(stdin);
                  break;
            case 2:
                  // listar um
                  if (tamanho_vetor == 0)
                        printf("\nOps, não tem nada aqui!\n");
                  else
                  {
                        printf("\nInforme o cpf do cliente a ser listado: ");
                        while ((terminador = getchar()) != '\n' && terminador != EOF);
                        fgets(cpf, sizeof(cpf), stdin);
                        cpf[strcspn(cpf, "\n")] = '\0';
                        listar_um_cliente(vetor_clientes, tamanho_vetor, cpf);
                  }
                  printf("\nEnter para continuar...");
                  continuar = fgetc(stdin);
                  break;
            case 3:
                  // incluir um (sem repeticao)
                  if (incluir_um_cliente(&vetor_clientes, &tamanho_vetor, &capacidade))
                        printf("\ncliente incluído com sucesso!\n");
                  else
                        printf("\nNão foi possível incluir o cliente.\n");

                  printf("\nEnter para continuar...");
                  continuar = fgetc(stdin);
                  break;
            case 4:
                  // alterar um cliente
                  if (tamanho_vetor == 0)
                        printf("\nOps, não tem nada aqui!\n");
                  else
                  {
                        printf("\nInforme o cpf do cliente a ser alterado: ");
                        while ((terminador = getchar()) != '\n' && terminador != EOF);
                        fgets(cpf, sizeof(cpf), stdin);
                        cpf[strcspn(cpf, "\n")] = '\0';
                        if (alterar_cliente(vetor_clientes, cpf, tamanho_vetor))
                              printf("\ncliente alterado com sucesso!\n");
                        else 
                              printf("\nNão foi possível alterar o cliente.\n");
                  }
                  printf("\nEnter para continuar...");
                  continuar = fgetc(stdin);
                  break;
            case 5:
                  // excluir um cliente
                  if (tamanho_vetor == 0)
                        printf("\nOps, não tem nada aqui!\n");
                  else
                  {
                        printf("\nInforme o cpf do cliente a ser excluído: ");
                        while ((terminador = getchar()) != '\n' && terminador != EOF);
                        fgets(cpf, sizeof(cpf), stdin);
                        cpf[strcspn(cpf, "\n")] = '\0';
                        if (excluir_cliente(vetor_clientes, cpf, &tamanho_vetor))
                              printf("\ncliente excluído com sucesso!\n");
                        else 
                              printf("\nNão foi possível excluir o cliente.\n");
                  }
                  printf("\nEnter para continuar...");
                  continuar = fgetc(stdin);
                  break;
            case 6:
                  printf("\nVoltando...\n");
                  break;
            default:
                  printf("\nOpção inválida\n");
                  break;
            }

      } while (opcao != 6);

      char salvar;
      do
      {
            while ((terminador = getchar()) != '\n' && terminador != EOF);
            printf("\nSalvar dados?: (s / n): ");
            salvar = fgetc(stdin);
            switch (salvar)
            {
            case 's':
                  // salva
                  printf("Salvando...\n");
                  salvar_clientes(vetor_clientes, nome_arquivo, tamanho_vetor);
                  break;
            case 'n':
                  printf("Saindo sem salvar...\n");
                  break;
            default:
                  printf("Opção inválida.\n");
                  break;
            }
      } while (!(salvar == 'n' || salvar == 's'));

      if (vetor_clientes != NULL)
      {
            free(vetor_clientes);
            vetor_clientes = NULL;
      }
}

/* Usado para gerenciar o submenu de servicos.
 * Os dados sao locais apenas, e podem ser salvos em arquivos para evitar perdas.
 */
void main_servicos()
{
      servico* vetor_servicos;
      faxineiro* vetor_faxineiros;
      cliente* vetor_clientes;
      
      int opcao;

      int tamanho_servico;
      int tamanho_cliente;
      int tamanho_faxineiro;

      int capacidade_servicos;
      int capacidade_faxineiros;
      int capacidade_clientes;

      char* identificador;
      char cpf_faxineiro[MAX_CPF];
      char cpf_cliente[MAX_CPF];
      char continuar;
      char terminador;

      char data_servico[MAX_DATA];
      char data_informada[MAX_DATA];
      bool valida;
      data data_convertida;

      const char nome_arquivo_servicos[] = "dados_servicos.dat";
      const char nome_arquivo_faxineiros[] = "dados_faxineiros.dat";
      const char nome_arquivo_clientes[] = "dados_clientes.dat";

      // arquivo servicos
      if (arquivo_vazio(nome_arquivo_servicos))
      {
            vetor_servicos = (servico *)calloc(CAPACIDADE_SERV_VAZIO, sizeof(servico));
            tamanho_servico = 0;
            capacidade_servicos = CAPACIDADE_SERV_VAZIO;
      }           
      else
      {
            vetor_servicos = carregar_servicos(nome_arquivo_servicos, &tamanho_servico);
            capacidade_servicos = tamanho_servico * 2;
      }

      // arquivo faxineiros
      if (arquivo_vazio(nome_arquivo_faxineiros))
      {
            vetor_faxineiros = (faxineiro *)calloc(CAPACIDADE_FAX_VAZIO, sizeof(faxineiro));
            tamanho_faxineiro = 0;
            capacidade_faxineiros = CAPACIDADE_FAX_VAZIO;
      }           
      else
      {
            vetor_faxineiros = carregar_faxineiros(nome_arquivo_faxineiros, &tamanho_faxineiro);
            capacidade_faxineiros = tamanho_faxineiro * 2;
      }

      // arquivo clientes
      if (arquivo_vazio(nome_arquivo_clientes))
      {
            vetor_clientes = (cliente *)calloc(CAPACIDADE_CLI_VAZIO, sizeof(cliente));
            tamanho_cliente = 0;
            capacidade_clientes = CAPACIDADE_CLI_VAZIO;
      }           
      else
      {
            vetor_clientes = carregar_clientes(nome_arquivo_clientes, &tamanho_cliente);
            capacidade_clientes = tamanho_cliente * 2;
      }

      // [tamanho_servico, tamanho_faxineiro, tamanho_cliente]
      int tamanhos[3] = {tamanho_servico, tamanho_faxineiro, tamanho_cliente};

      do
      {
            opcao = print_submenu("servico");
            switch (opcao)
            {
            case 1:
                  // listar todos

                  if (tamanho_servico == 0)
                        printf("\nOps, não tem nada aqui!\n");

                  else
                        listar_todos_servicos(vetor_servicos, vetor_faxineiros, vetor_clientes, tamanhos);

                  printf("\nEnter para continuar...");
                  while ((terminador = getchar()) != '\n' && terminador != EOF);
                  continuar = fgetc(stdin);
                  break;
            case 2:
                  // listar um

                  if (tamanho_servico == 0)
                        printf("\nOps, não tem nada aqui!\n");
                  else
                  {
                        while ((terminador = getchar()) != '\n' && terminador != EOF);

                        printf("\nInforme o cpf do faxineiro do serviço: ");
                        fgets(cpf_faxineiro, sizeof(cpf_faxineiro), stdin);
                        cpf_faxineiro[strcspn(cpf_faxineiro, "\n")] = '\0';
                        if (buscar_faxineiro(vetor_faxineiros, 0, tamanhos[1] - 1, cpf_faxineiro) == -1)
                        {
                              printf("\nO faxineiro não existe!\n");
                              break;
                        }

                        printf("Informe o cpf do cliente do serviço: ");
                        fgets(cpf_cliente, sizeof(cpf_cliente), stdin);
                        cpf_cliente[strcspn(cpf_cliente, "\n")] = '\0';
                        if (buscar_cliente(vetor_clientes, 0, tamanhos[1] - 1, cpf_cliente) == -1)
                        {
                              printf("\nO cliente não existe!\n");
                              break;
                        }

                        do
                        {
                              printf("Informe a data do serviço (dd/mm/aaaa): ");
                              fgets(data_informada, sizeof(data_informada), stdin);
                              data_informada[strcspn(data_informada, "\n")] = '\0';

                              strcpy(data_servico, data_informada);

                              valida = converter_data(data_informada, &data_convertida);

                              if (!valida)
                                    printf("Data inválida!\n");
                        } while (!valida);

                        identificador = buscar_id_servico(cpf_faxineiro, cpf_cliente, data_servico);

                        listar_um_servico(vetor_servicos, vetor_faxineiros, vetor_clientes, tamanhos, identificador);
                        free(identificador);
                  }
                  printf("\nEnter para continuar...");
                  continuar = fgetc(stdin);
                  break;
            case 3:
                  // incluir um (sem repeticao)

                  while ((terminador = getchar()) != '\n' && terminador != EOF);

                  printf("\nInforme o cpf do faxineiro do serviço: ");
                  fgets(cpf_faxineiro, sizeof(cpf_faxineiro), stdin);
                  cpf_faxineiro[strcspn(cpf_faxineiro, "\n")] = '\0';
                  if (buscar_faxineiro(vetor_faxineiros, 0, tamanhos[1] - 1, cpf_faxineiro) == -1)
                  {
                        printf("\nO faxineiro não existe!\n");
                        break;
                  }

                  printf("Informe o cpf do cliente do serviço: ");
                  fgets(cpf_cliente, sizeof(cpf_cliente), stdin);
                  cpf_cliente[strcspn(cpf_cliente, "\n")] = '\0';
                  if (buscar_cliente(vetor_clientes, 0, tamanhos[1] - 1, cpf_cliente) == -1)
                  {
                        printf("\nO cliente não existe!\n");
                        break;
                  }

                  do
                  {
                        printf("Informe a data do serviço (dd/mm/aaaa): ");
                        fgets(data_informada, sizeof(data_informada), stdin);
                        data_informada[strcspn(data_informada, "\n")] = '\0';

                        strcpy(data_servico, data_informada);

                        valida = converter_data(data_informada, &data_convertida);

                        if (!valida)
                              printf("Data inválida!\n");
                  } while (!valida);

                  identificador = buscar_id_servico(cpf_faxineiro, cpf_cliente, data_servico);

                  if (incluir_um_servico(vetor_servicos, identificador, cpf_faxineiro, cpf_cliente,
                                          data_convertida, &tamanho_servico, &capacidade_servicos))
                        printf("\nservico incluído com sucesso!\n");
                  else
                        printf("\nNão foi possível incluir o servico.\n");

                  free(identificador);
                  tamanhos[0] = tamanho_servico;

                  printf("\nEnter para continuar...");
                  continuar = fgetc(stdin);
                  break;
            case 4:
                  // alterar um servico

                  if (tamanho_servico == 0)
                        printf("\nOps, não tem nada aqui!\n");
                  else
                  {
                        while ((terminador = getchar()) != '\n' && terminador != EOF);

                        printf("\nInforme o cpf do faxineiro do serviço: ");
                        fgets(cpf_faxineiro, sizeof(cpf_faxineiro), stdin);
                        cpf_faxineiro[strcspn(cpf_faxineiro, "\n")] = '\0';
                        if (buscar_faxineiro(vetor_faxineiros, 0, tamanhos[1] - 1, cpf_faxineiro) == -1)
                        {
                              printf("\nO faxineiro não existe!\n");
                              break;
                        }

                        printf("Informe o cpf do cliente do serviço: ");
                        fgets(cpf_cliente, sizeof(cpf_cliente), stdin);
                        cpf_cliente[strcspn(cpf_cliente, "\n")] = '\0';
                        if (buscar_cliente(vetor_clientes, 0, tamanhos[1] - 1, cpf_cliente) == -1)
                        {
                              printf("\nO cliente não existe!\n");
                              break;
                        }

                        do
                        {
                              printf("Informe a data do serviço (dd/mm/aaaa): ");
                              fgets(data_informada, sizeof(data_informada), stdin);
                              data_informada[strcspn(data_informada, "\n")] = '\0';

                              strcpy(data_servico, data_informada);

                              valida = converter_data(data_informada, &data_convertida);

                              if (!valida)
                                    printf("Data inválida!\n");
                        } while (!valida);

                        identificador = buscar_id_servico(cpf_faxineiro, cpf_cliente, data_servico);

                        if (alterar_servico(vetor_servicos, vetor_faxineiros, vetor_clientes, identificador, tamanhos))
                              printf("\nservico alterado com sucesso!\n");
                        else 
                              printf("\nNão foi possível alterar o servico.\n");
                  }
                  printf("\nEnter para continuar...");
                  continuar = fgetc(stdin);
                  break;
            case 5:
                  // excluir um servico

                  if (tamanho_servico == 0)
                        printf("\nOps, não tem nada aqui!\n");
                  else
                  {
                        while ((terminador = getchar()) != '\n' && terminador != EOF);

                        printf("\nInforme o cpf do faxineiro do serviço: ");
                        fgets(cpf_faxineiro, sizeof(cpf_faxineiro), stdin);
                        cpf_faxineiro[strcspn(cpf_faxineiro, "\n")] = '\0';
                        if (buscar_faxineiro(vetor_faxineiros, 0, tamanhos[1] - 1, cpf_faxineiro) == -1)
                        {
                              printf("\nO faxineiro não existe!\n");
                              break;
                        }

                        printf("Informe o cpf do cliente do serviço: ");
                        fgets(cpf_cliente, sizeof(cpf_cliente), stdin);
                        cpf_cliente[strcspn(cpf_cliente, "\n")] = '\0';
                        if (buscar_cliente(vetor_clientes, 0, tamanhos[1] - 1, cpf_cliente) == -1)
                        {
                              printf("\nO cliente não existe!\n");
                              break;
                        }

                        do
                        {
                              printf("Informe a data do serviço (dd/mm/aaaa): ");
                              fgets(data_informada, sizeof(data_informada), stdin);
                              data_informada[strcspn(data_informada, "\n")] = '\0';

                              strcpy(data_servico, data_informada);

                              valida = converter_data(data_informada, &data_convertida);

                              if (!valida)
                                    printf("Data inválida!\n");
                        } while (!valida);

                        identificador = buscar_id_servico(cpf_faxineiro, cpf_cliente, data_servico);

                        if (excluir_servico(vetor_servicos, vetor_faxineiros, vetor_clientes, identificador, tamanhos))
                              printf("\nservico excluído com sucesso!\n");
                        else 
                              printf("\nNão foi possível excluir o servico.\n");
                  }
                  printf("\nEnter para continuar...");
                  continuar = fgetc(stdin);
                  break;
            case 6:
                  printf("\nVoltando...\n");
                  break;
            default:
                  printf("\nOpção inválida\n");
                  break;
            }

      } while (opcao != 6);

      char salvar;
      do
      {
            while ((terminador = getchar()) != '\n' && terminador != EOF);
            printf("\nSalvar dados?: (s / n): ");
            salvar = fgetc(stdin);
            switch (salvar)
            {
            case 's':
                  // salva
                  printf("Salvando...\n");
                  salvar_servicos(vetor_servicos, nome_arquivo_servicos, tamanho_servico);
                  break;
            case 'n':
                  printf("Saindo sem salvar...\n");
                  break;
            default:
                  printf("Opção inválida.\n");
                  break;
            }
      } while (!(salvar == 'n' || salvar == 's'));

      if (vetor_servicos != NULL)
      {
            free(vetor_servicos);
            vetor_servicos = NULL;
      }
      if (vetor_faxineiros != NULL)
      {
            free(vetor_faxineiros);
            vetor_faxineiros = NULL;
      }
      if (vetor_clientes != NULL)
      {
            free(vetor_clientes);
            vetor_clientes = NULL;
      }
}

/* Usado para gerenciar o submenu de relatorios.
 * Os dados sao locais apenas, e podem ser salvos em arquivos para evitar perdas.
 */
void main_relatorios()
{
      faxineiro *vetor_faxineiros;
      cliente *vetor_clientes;
      servico *vetor_servicos;

      int opcao;

      int tamanho_servico;
      int tamanho_cliente;
      int tamanho_faxineiro;

      int capacidade_servicos;
      int capacidade_faxineiros;
      int capacidade_clientes;

      char* identificador;
      char cpf_faxineiro[MAX_CPF];
      char cpf_cliente[MAX_CPF];
      char continuar;
      char terminador;

      char data_servico[MAX_DATA];
      char data_informada[MAX_DATA];
      char data_inicial[MAX_DATA];
      char data_final[MAX_DATA];
      bool valida;
      data data_convertida_inicial;
      data data_convertida_final;
      data data_convertida;

      const char nome_arquivo_servicos[] = "dados_servicos.dat";
      const char nome_arquivo_faxineiros[] = "dados_faxineiros.dat";
      const char nome_arquivo_clientes[] = "dados_clientes.dat";

      const char arquivo_relatorio1[] = "relatorio1.txt";
      const char arquivo_relatorio2[] = "relatorio2.txt";
      const char arquivo_relatorio3[] = "relatorio3.txt";

      // arquivo servicos
      if (arquivo_vazio(nome_arquivo_servicos))
      {
            vetor_servicos = (servico *)calloc(CAPACIDADE_SERV_VAZIO, sizeof(servico));
            tamanho_servico = 0;
            capacidade_servicos = CAPACIDADE_SERV_VAZIO;
      }           
      else
      {
            vetor_servicos = carregar_servicos(nome_arquivo_servicos, &tamanho_servico);
            capacidade_servicos = tamanho_servico * 2;
      }

      // arquivo faxineiros
      if (arquivo_vazio(nome_arquivo_faxineiros))
      {
            vetor_faxineiros = (faxineiro *)calloc(CAPACIDADE_FAX_VAZIO, sizeof(faxineiro));
            tamanho_faxineiro = 0;
            capacidade_faxineiros = CAPACIDADE_FAX_VAZIO;
      }           
      else
      {
            vetor_faxineiros = carregar_faxineiros(nome_arquivo_faxineiros, &tamanho_faxineiro);
            capacidade_faxineiros = tamanho_faxineiro * 2;
      }

      // arquivo clientes
      if (arquivo_vazio(nome_arquivo_clientes))
      {
            vetor_clientes = (cliente *)calloc(CAPACIDADE_CLI_VAZIO, sizeof(cliente));
            tamanho_cliente = 0;
            capacidade_clientes = CAPACIDADE_CLI_VAZIO;
      }           
      else
      {
            vetor_clientes = carregar_clientes(nome_arquivo_clientes, &tamanho_cliente);
            capacidade_clientes = tamanho_cliente * 2;
      }

      // [tamanho_servico, tamanho_faxineiro, tamanho_cliente]
      int tamanhos[3] = {tamanho_servico, tamanho_faxineiro, tamanho_cliente};

       do
      {
            opcao = print_submenu_relatorios();
            switch (opcao)
            {
            case 1:
                  // 1. Listar clientes de um faxineiro entre datas X até Y

                  if (tamanho_servico == 0)
                        printf("\nOps, não tem nada aqui!\n");

                  else
                  {
                        while ((terminador = getchar()) != '\n' && terminador != EOF);

                        printf("\nInforme o cpf do faxineiro do serviço: ");
                        fgets(cpf_faxineiro, sizeof(cpf_faxineiro), stdin);
                        cpf_faxineiro[strcspn(cpf_faxineiro, "\n")] = '\0';
                        if (buscar_faxineiro(vetor_faxineiros, 0, tamanhos[1] - 1, cpf_faxineiro) == -1)
                        {
                              printf("\nO faxineiro não existe!\n");
                              break;
                        }

                        do
                        {
                              printf("Informe a data de inicio (dd/mm/aaaa): ");
                              fgets(data_informada, sizeof(data_informada), stdin);
                              data_informada[strcspn(data_informada, "\n")] = '\0';

                              strcpy(data_inicial, data_informada);

                              valida = converter_data(data_informada, &data_convertida_inicial);

                              if (!valida)
                                    printf("Data inválida!\n");
                        } while (!valida);

                        do
                        {
                              printf("Informe a data final (dd/mm/aaaa): ");
                              fgets(data_informada, sizeof(data_informada), stdin);
                              data_informada[strcspn(data_informada, "\n")] = '\0';

                              strcpy(data_final, data_informada);

                              valida = converter_data(data_informada, &data_convertida_final);

                              if (valida)
                              {
                                    if (!data_menor_ou_igual(data_convertida_inicial, data_convertida_final))
                                          valida = false;
                              }

                              if (!valida)
                                    printf("Data inválida, certifique-se de que a data final é maior que a inicial.\n");
                        } while (!valida);

                        if (!atendidos_entre_datas(vetor_clientes, vetor_servicos, tamanhos, cpf_faxineiro,
                              data_inicial, data_final, arquivo_relatorio1))
                        {
                              printf("\nOcorreu um erro\n");
                        }
                  }
                  printf("\nEnter para continuar...");
                  continuar = fgetc(stdin);
                  break;
            case 2:
                  // 2. Listar serviços de uma data específica

                  if (tamanho_servico == 0)
                        printf("\nOps, não tem nada aqui!\n");
                  else
                  {
                        while ((terminador = getchar()) != '\n' && terminador != EOF);

                        do
                        {
                              printf("Informe a data dos serviços (dd/mm/aaaa): ");
                              fgets(data_informada, sizeof(data_informada), stdin);
                              data_informada[strcspn(data_informada, "\n")] = '\0';

                              strcpy(data_servico, data_informada);

                              valida = converter_data(data_informada, &data_convertida);

                              if (!valida)
                                    printf("Data inválida!\n");
                        } while (!valida);

                        if (!servicos_data_especifica(vetor_servicos, vetor_faxineiros, vetor_clientes, tamanhos,
                              data_servico, arquivo_relatorio2))
                        {
                              printf("\nOcorreu um erro.\n");
                        }
                  }
                  printf("\nEnter para continuar...");
                  continuar = fgetc(stdin);
                  break;
            case 3:
                  // 3. Listar todos os serviços de um faxineiro (pelo CPF)
                  if (tamanho_servico == 0)
                        printf("\nOps, não tem nada aqui!\n");
                  else
                  {
                        while ((terminador = getchar()) != '\n' && terminador != EOF);

                        printf("\nInforme o cpf do faxineiro do serviço: ");
                        fgets(cpf_faxineiro, sizeof(cpf_faxineiro), stdin);
                        cpf_faxineiro[strcspn(cpf_faxineiro, "\n")] = '\0';
                        if (buscar_faxineiro(vetor_faxineiros, 0, tamanhos[1] - 1, cpf_faxineiro) == -1)
                        {
                              printf("\nO faxineiro não existe!\n");
                              break;
                        }

                        if (!servicos_por_faxineiro(vetor_servicos, vetor_clientes, cpf_faxineiro, tamanhos, arquivo_relatorio3))
                        {
                              printf("\nOcorreu um erro.\n");
                        }
                  }
                  printf("\nEnter para continuar...");
                  continuar = fgetc(stdin);
                  break;
            case 4:
                  printf("\nVoltando...\n");
                  break;
            default:
                  printf("\nOpção inválida\n");
                  break;
            }

      } while (opcao != 4);

      if (vetor_servicos != NULL)
      {
            free(vetor_servicos);
            vetor_servicos = NULL;
      }
      if (vetor_faxineiros != NULL)
      {
            free(vetor_faxineiros);
            vetor_faxineiros = NULL;
      }
      if (vetor_clientes != NULL)
      {
            free(vetor_clientes);
            vetor_clientes = NULL;
      }
}

// Pragrama principal do CRUD
int main()
{
      setlocale(LC_ALL, "pt_BR.UTF-8");         // setlocale(LC_ALL, "Portuguese") não funciona
      int opcao;

      do // Inicio do ciclo =================================================================================
      {
            opcao = print_menu();
            switch (opcao)
            {
            case 1:
                  // Submenu de Faxineiros
                  printf("\n===== Submenu de Faxineiros =====\n");
                  main_faxineiros();
                  break;
            case 2:
                  // Submenu de Clientes
                  printf("\n===== Submenu de Clientes =====\n");
                  main_clientes();
                  break;
            case 3:
                  // Submenu de Serviços
                  printf("\n===== Submenu de Serviços =====\n");
                  main_servicos();
                  break;
            case 4:
                  // Submenu Relatórios
                  printf("\n===== Submenu Relatórios =====\n");
                  main_relatorios();
                  break;
            case 5:
                  printf("\nSaindo...\n");
                  break;
            default:
                  printf("\nOpção inválida!\n");
                  break;
            }
      } while (opcao != 5);

      return 0;
}
