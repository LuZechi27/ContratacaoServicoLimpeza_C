//======================================================================
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include <stdbool.h>

// ======================================================================
#define MAX_CPF              20
#define MAX_RG               20
#define MAX_NOME             51
#define MAX_DATA             12
#define MAX_QT_TELEFONES     10
#define MAX_TAM_TELEFONE     21
#define MAX_ENDERECO         101
#define MAX_CEP              11
#define MAX_CIDADE           101
#define MAX_QT_EMAILS        10
#define MAX_TAM_EMAILS       101
#define CAPACIDADE_FAX_VAZIO 50

// =================== Structs ==============================================
typedef struct
{
      char cpf[MAX_CPF];            // 999.999.999-99\0\0
      char rg[MAX_RG];              // 99.999.999-9\0\0
      char nome[MAX_NOME];
      char sexo;
      char data_nascimento[MAX_DATA];
      int quantidade_telefones;
      char telefones[MAX_QT_TELEFONES][MAX_TAM_TELEFONE];
} faxineiro;

typedef struct
{
      char cpf[MAX_CPF];
      char nome[MAX_NOME];
      char data_nascimento[MAX_DATA];
      char endereço[MAX_ENDERECO];
      char cep[MAX_CEP];
      char cidade[MAX_CIDADE];
      char emails[MAX_QT_EMAILS][MAX_TAM_EMAILS];
      int quantidade_telefones;
      char telefones[MAX_QT_TELEFONES][MAX_TAM_TELEFONE];         // max = +55 (99) 99999-9999
} cliente;

typedef struct
{
      unsigned long int cpf_faxineiro;
      unsigned long int cpf_cliente;
      char data[MAX_DATA];
      float valor;
} servico;

// =====================================================================================
bool arquivo_vazio(char *nome_arquivo)
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
// ================= Menus e Submenus ==================================================
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

int print_submenu(char *tipo_submenu)
{
      int opt; // Listar todos, Listar um, Incluir (sem repetição), Alterar e Excluir
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
faxineiro *carregar_faxineiros(char *nome_arquivo, int *retorno_tamanho_vetor)
{
      FILE *arquivo;
      faxineiro *vetor_faxineiros;
      int tamanho_vetor;

      arquivo = fopen(nome_arquivo, "rb+");
      if (arquivo == NULL)
      {
            perror("Erro ao abrir o arquivo");
            exit(EXIT_FAILURE);
      }

      if (fread(&tamanho_vetor, sizeof(int), 1, arquivo) != 1)
      {
            perror("Erro ao ler o tamanho do vetor");
            fclose(arquivo);
            exit(EXIT_FAILURE);
      }

      vetor_faxineiros = malloc(tamanho_vetor * sizeof(faxineiro));
      if (vetor_faxineiros == NULL)
      {
            perror("Erro ao alocar o vetor");
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

      if (ferror(arquivo))
      {
            perror("Erro de leitura");
            fclose(arquivo);
            free(vetor_faxineiros);
            exit(EXIT_FAILURE);
      }

      fclose(arquivo);
      *retorno_tamanho_vetor = tamanho_vetor;
      return vetor_faxineiros;
}

cliente *carregar_clientes(char *nome_arquivo, int *retorno_tamanho_vetor)
{
      FILE *arquivo;
      cliente *vetor_clientes;
      int tamanho_vetor;

      arquivo = fopen(nome_arquivo, "rb+");
      if (arquivo == NULL)
      {
            perror("Erro ao abrir o arquivo");
            exit(EXIT_FAILURE);
      }

      if (fread(&tamanho_vetor, sizeof(int), 1, arquivo) != 1)
      {
            perror("Erro ao ler o tamanho do vetor");
            fclose(arquivo);
            exit(EXIT_FAILURE);
      }

      vetor_clientes = malloc(tamanho_vetor * sizeof(cliente));
      if (vetor_clientes == NULL)
      {
            perror("Erro ao alocar o vetor");
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

      if (ferror(arquivo))
      {
            perror("Erro de leitura");
            fclose(arquivo);
            free(vetor_clientes);
            exit(EXIT_FAILURE);
      }

      fclose(arquivo);
      *retorno_tamanho_vetor = tamanho_vetor;
      return vetor_clientes;
}

servico *carregar_servicos(char *nome_arquivo, int *retorno_tamanho_vetor)
{
      FILE *arquivo;
      servico *vetor_servicos;
      int tamanho_vetor;

      arquivo = fopen(nome_arquivo, "rb+");
      if (arquivo == NULL)
      {
            perror("Erro ao abrir o arquivo");
            exit(EXIT_FAILURE);
      }

      if (fread(&tamanho_vetor, sizeof(int), 1, arquivo) != 1)
      {
            perror("Erro ao ler o tamanho do vetor");
            fclose(arquivo);
            exit(EXIT_FAILURE);
      }

      vetor_servicos = malloc(tamanho_vetor * sizeof(servico));
      if (vetor_servicos == NULL)
      {
            perror("Erro ao alocar o vetor");
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

      if (ferror(arquivo))
      {
            perror("Erro de leitura");
            fclose(arquivo);
            free(vetor_servicos);
            exit(EXIT_FAILURE);
      }

      fclose(arquivo);
      *retorno_tamanho_vetor = tamanho_vetor;
      return vetor_servicos;
}

void salvar_faxineiros(faxineiro *vetor_faxineiros, int tamanho_vetor)
{
      FILE *arquivo;

      arquivo = fopen("dados_faxineiros.bin", "wb+");
      if (arquivo == NULL)
      {
            perror("Erro ao abrir o arquivo");
            fclose(arquivo);
            exit(EXIT_FAILURE);
      }
      if ((fwrite(&tamanho_vetor, sizeof(int), 1, arquivo)) != 1)
      {
            perror("Erro ao salvar a quantidade de faxineiros");
            fclose(arquivo);
            exit(EXIT_FAILURE);
      }
      if ((fwrite(vetor_faxineiros, sizeof(faxineiro), tamanho_vetor, arquivo)) != tamanho_vetor)
      {
            perror("Erro ao salvar os dados");
            fclose(arquivo);
            exit(EXIT_FAILURE);
      }
      if (ferror(arquivo))
      {
            perror("Erro ao salvar");
            fclose(arquivo);
            exit(EXIT_FAILURE);
      }
      fclose(arquivo);
}
// ======================================================================= Carregar e Salvar ===== //
//
// ===================== Funcoes de Faxineiros ====================================================
int buscar_faxineiro(faxineiro *faxineiros, int inicio, int fim, char *cpf_procurado)
{
      int meio = (inicio + fim) / 2;
      int comparacao = strcmp(faxineiros[meio].cpf, cpf_procurado);
      if (inicio > fim)
            return -1;
      if (comparacao == 0)
            return meio;
      if (comparacao < 0)
            return buscar_faxineiro(faxineiros, meio + 1, fim, cpf_procurado);
      if (comparacao > 0)
            return buscar_faxineiro(faxineiros, inicio, meio - 1, cpf_procurado);
      return -1;
}

int busca_para_inserir_faxineiro(faxineiro *faxineiros, int inicio, int fim, char *cpf_procurado)
{
      int meio = (inicio + fim) / 2;
      int comparacao = strcmp(faxineiros[meio].cpf, cpf_procurado);
      if (comparacao == 0)
            return -1;
      if ((fim - inicio) <= 1)
            return inicio;
      if (comparacao < 0)
            return buscar_faxineiro(faxineiros, meio + 1, fim, cpf_procurado);
      if (comparacao > 0)
            return buscar_faxineiro(faxineiros, inicio, meio - 1, cpf_procurado);
}

void listar_todos_faxineiros(faxineiro *faxineiros, int tamanho_vetor)
{
      int i, j;
      printf("\nTodos os faxineiros:");
      for (i = 0; i < tamanho_vetor; i++)
      {
            printf("\nNome: %s\n", faxineiros[i].nome);
            printf("CPF : %s\n", faxineiros[i].cpf);
            printf("RG : %s\n", faxineiros[i].rg);
            printf("Sexo : %c\n", faxineiros[i].sexo);
            printf("Data de nascimento : %s\n", faxineiros[i].data_nascimento);
            printf("Telefones : ");
            for (j = 0; j < faxineiros[i].quantidade_telefones; j++)
                  printf("%s | ", faxineiros[i].telefones[j]);
            printf("\n");
      }
      puts("\n");
}

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
            printf("Data de nascimento : %s\n", faxineiros[indice].data_nascimento);
            printf("Telefones : ");
            for (i = 0; i < faxineiros[indice].quantidade_telefones; i++)
                  printf("%s | ", faxineiros[indice].telefones[i]);
            printf("\n");
      }
}

void adicionar_faxineiro(faxineiro *faxineiros, int indice, char *cpf_a_registrar)
{
      int i;
      char terminador_string;

      strcpy(faxineiros[indice].cpf, cpf_a_registrar);

      terminador_string = fgetc(stdin); // para tirar o '\n' do buffer para usar fgets()

      printf("Informe o RG: ");
      fgets(faxineiros[indice].rg, sizeof(faxineiros[indice].rg), stdin);
      faxineiros[indice].rg[strcspn(faxineiros[indice].rg, "\n")] = '\0';

      printf("Informe o Nome (Tamanho máximo = 50): ");
      fgets(faxineiros[indice].nome, sizeof(faxineiros[indice].nome), stdin);
      faxineiros[indice].nome[strcspn(faxineiros[indice].nome, "\n")] = '\0';

      printf("Informe o Sexo (M ou F): ");
      faxineiros[indice].sexo = fgetc(stdin);
      terminador_string = fgetc(stdin);

      printf("Informe a data de nascimento (dd/mm/aaaa): ");
      fgets(faxineiros[indice].data_nascimento, sizeof(faxineiros[indice].data_nascimento), stdin);
      faxineiros[indice].data_nascimento[strcspn(faxineiros[indice].data_nascimento, "\n")] = '\0';

      printf("Informe a quantidade de telefones (Tamanho máximo = 10): ");
      scanf("%d", &faxineiros[indice].quantidade_telefones);
      terminador_string = fgetc(stdin);

      for (i = 0; i < faxineiros[indice].quantidade_telefones; i++)
      {
            printf("Telefone %d: ", i + 1);
            fgets(faxineiros[indice].telefones[i], sizeof(faxineiros[indice].telefones[i]), stdin);
            faxineiros[indice].telefones[i][strcspn(faxineiros[indice].telefones[i], "\n")] = '\0';
      }
}

bool incluir_um_faxineiro(faxineiro **faxineiros, int *tamanho, int *capacidade)
{
      char cpf_a_registrar[MAX_CPF];
      int indice_inserir, i;
      char terminador_string;

      printf("\nInforme o CPF a ser cadastrado: ");
      terminador_string = fgetc(stdin);
      fgets(cpf_a_registrar, sizeof(cpf_a_registrar), stdin);
      cpf_a_registrar[strcspn(cpf_a_registrar, "\n")] = '\0';

      if (*tamanho == 0)
      {
            adicionar_faxineiro(*faxineiros, 0, cpf_a_registrar);
            (*tamanho)++;
            return true;
      }
      else
      {
            if ((indice_inserir = busca_para_inserir_faxineiro(*faxineiros, 0, *tamanho - 1, cpf_a_registrar)) == -1)
            {
                  printf("\nCPF já está cadastrado.\n");
                  return false;
            }
            else
            {
                  if (*capacidade <= *tamanho)
                  {
                        (*capacidade) *= 2;
                        *faxineiros = (faxineiro *)realloc(*faxineiros, (*capacidade) * sizeof(faxineiro));
                  }
                  if ((*faxineiros)[indice_inserir].cpf > cpf_a_registrar)
                  {
                        for (i = *tamanho; i > indice_inserir; i--)
                              (*faxineiros)[i] = (*faxineiros)[i - 1];

                        adicionar_faxineiro(*faxineiros, indice_inserir, cpf_a_registrar);
                        (*tamanho)++;
                        return true;
                  }
                  else
                  {
                        // if (indice_inserir == *tamanho - 1)
                        // {
                        //       adicionar_faxineiro(*faxineiros, *tamanho, cpf_a_registrar);
                        //       (*tamanho)++;
                        //       return true;
                        // }
                        // else 
                        // {
                              for (i = *tamanho; i > indice_inserir + 1; i--)
                                    (*faxineiros)[i] = (*faxineiros)[i - 1];

                              adicionar_faxineiro(*faxineiros, indice_inserir + 1, cpf_a_registrar);
                              (*tamanho)++;
                              return true;
                        // }
                  }
            }
      }
}
// =========================================================== Funcoes de Faxineiros ==============//
//
// ============================= Mains =============================================================
void main_faxineiros()
{
      faxineiro *vetor_faxineiros;
      int opcao, tamanho_vetor, capacidade;
      char cpf[MAX_CPF];
      char continuar, terminador;
      char nome_arquivo[] = "dados_faxineiros.bin";

      if (arquivo_vazio(nome_arquivo))
      {
            vetor_faxineiros = (faxineiro *)malloc(CAPACIDADE_FAX_VAZIO * sizeof(faxineiro));
            tamanho_vetor = 0;
            capacidade = CAPACIDADE_FAX_VAZIO;
      }
      else
      {
            vetor_faxineiros = carregar_faxineiros(nome_arquivo, &tamanho_vetor);
            capacidade = tamanho_vetor;
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
                  terminador = fgetc(stdin);
                  continuar = fgetc(stdin);
                  break;
            case 2:
                  // listar um
                  if (tamanho_vetor == 0)
                        printf("\nOps, não tem nada aqui!\n");
                  else
                  {
                        printf("\nInforme o cpf do faxineiro a ser listado: ");
                        terminador = fgetc(stdin);
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
                        printf("\nNão foi possível incluir o Faxineiro.\n");

                  printf("\nEnter para continuar...");
                  terminador = fgetc(stdin);
                  break;
            case 4:
                  // alterar um

                  break;
            case 5:
                  // excluir um

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
            printf("\nSalvar dados?: (s / n): ");
            terminador = fgetc(stdin);
            scanf("%c", &salvar);
            switch (salvar)
            {
            case 's':
                  // salva
                  printf("Salvando...\n");
                  salvar_faxineiros(vetor_faxineiros, tamanho_vetor);
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
            free(vetor_faxineiros);
}

int main()
{
      setlocale(LC_ALL, "Portuguese");
      cliente *vetor_clientes;
      servico *vetor_servicos;
      int opcao, tamanho_vetor_clientes, tamanho_vetor_servicos;

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
                  break;
            case 3:
                  // Submenu de Serviços
                  printf("\n===== Submenu de Serviços =====\n");
                  break;
            case 4:
                  // Submenu Relatórios
                  printf("\n===== Submenu Relatórios =====\n");
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
