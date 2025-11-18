# Projeto CRUD (Trabalho)

Trabalho do curso de Bacharelado em Engenharia de Software.

Nome do estudante: Luiz Gustavo Silva Zechi

# Tema 4 — Sistema de Contratação de Serviço de Limpeza

## Descrição Geral

Este projeto consiste no desenvolvimento de um sistema em linguagem C para gerenciar informações de uma prestadora de serviços de limpeza (faxinas).
O sistema deverá armazenar e manipular dados de faxineiros, clientes e serviços, garantindo integridade das chaves e persistência em arquivos binários.

Além das operações básicas (CRUD), o sistema deve gerar relatórios em arquivos texto com base nas informações cadastradas.

- **Observação**: Uma contextualização detalhada será adicionada posteriormente.

## Entidades e Atributos

### Faxineiro
| Atributo           | Descrição                 | Chave |
| ------------------ | ------------------------- | ----- |
| CPF                | Cadastro de Pessoa Física | ✔️    |
| RG                 | Registro Geral            |       |
| Nome               | Nome completo             |       |
| Sexo               | M/F/Outro                 |       |
| Data de nascimento | DD/MM/AAAA                |       |
| Telefones          | Lista de telefones        |       |

### Cliente
| Atributo           | Descrição                 | Chave |
| ------------------ | ------------------------- | ----- |
| CPF                | Cadastro de Pessoa Física | ✔️    |
| Nome               | Nome completo             |       |
| Data de nascimento | DD/MM/AAAA                |       |
| Endereço           | Logradouro                |       |
| CEP                | Código Postal             |       |
| Cidade             | Cidade                    |       |
| E-mails            | Lista de e-mails          |       |
| Telefones          | Lista de telefones        |       |

### Serviço
| Atributo      | Descrição                     | Chave |
| ------------- | ----------------------------- | ----- |
| CPF Faxineiro | Identificação do profissional | ✔️    |
| CPF Cliente   | Identificação do contratante  | ✔️    |
| Data          | Data da prestação do serviço  | ✔️    |
| Valor         | Valor cobrado                 |       |

## Regras Importantes

- Os atributos marcados como chave não podem se repetir.

- Não é permitido cadastrar um registro duplicado baseado em sua chave.

- Os dados devem ser armazenados temporariamente em estruturas dinâmicas (vetores e registros alocados dinamicamente).

- A persistência deve ser feita em arquivos binários específicos por entidade.

- Os relatórios devem ser gravados em arquivos texto.

## Funcionalidades

O sistema apresentará o seguinte Menu Principal:

1. Submenu de Faxineiros

2. Submenu de Clientes

3. Submenu de Serviços

4. Submenu de Relatórios

5. Sair

## Operações de Cada Submenu (CRUD)

Cada submenu (Faxineiros, Clientes, Serviços) deverá oferecer:

- Listar todos

- Listar um

- Incluir (sem repetição da chave)

- Alterar

- Excluir (mediante confirmação)

## Relatórios

O submenu de relatórios deverá fornecer:

### a) Clientes atendidos por um faxineiro entre datas X e Y

Exibir:

- Nome do cliente

- E-mails

- Telefones

Critérios:

- CPF do(a) faxineiro(a) informado pelo usuário

- Intervalo de datas X e Y também fornecidas pelo usuário

### b) Serviços realizados em uma data específica

Exibir:

- Dados completos do serviço

- Nome do(a) faxineiro(a)

- Nome do(a) cliente

A data será fornecida pelo usuário.

### c) Serviços realizados por determinado(a) faxineiro(a)

Exibir:

- Dados completos de cada serviço

O CPF do(a) faxineiro(a) será fornecido pelo usuário.

## Armazenamento dos Dados

O sistema deve utilizar arquivos binários separados:

- `dados_faxineiros.dat`

- `dados_clientes.dat`

- `dados_servicos.dat`

Os relatórios deverão ser gerados em arquivos texto, como:

- `relatorio_faxineiro.txt`

- `relatorio_data.txt`

etc.

## Estrutura de Dados Recomendada

- Estruturas (`struct`) para cada entidade

- Vetores dinâmicos (via `malloc`, `realloc`)

- Arquivos binários para persistência

- Funções separadas para cada operação (organização modular)
