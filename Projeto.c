#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

// Cria um dado personalizado para guardar os dados do investidor
typedef struct {
    char nome[255];
    char cpf[12]; 
    char senha[7];   
    float reais;     
    float bitcoin;   
    float ethereum;  
    float ripple;    
} Investidor;        // Apelido para chamar o typedef no resto do codigo

// Cotações globais das criptomoedas
float cotacaoBTC = 350000.00f; 
float cotacaoETH = 16108.00f;  
float cotacaoXRP = 2.57f;      


void salvarDadosInvestidor(Investidor *cli) {  
    FILE *arquivo = fopen("investidores.txt", "r");
    if (arquivo == NULL) return;                  // Sai se o arquivo não existir

    FILE *temp = fopen("temp.txt", "w");          // Cria um arquivo temporario
    if (temp == NULL) {                           // Se o arquivo temp der erro
        fclose(arquivo);                          // Fecha o arquivo que estava aberto
        return;                                   // Para a função
    }
    Investidor aux;         // Uma variável temporária para guardar os dados lidos em cada linha
    char nomeArquivo[50];   // Vetor para guardar o nome como esta gravado no arquivo 

    // guarda cada informação do arquivo
    while (fscanf(arquivo, "%s %s %s %f %f %f %f", 
                  nomeArquivo, aux.cpf, aux.senha, 
                  &aux.reais, &aux.bitcoin, &aux.ethereum, &aux.ripple) == 7) {
        
        
        if (strcmp(aux.cpf, cli->cpf) == 0) {   // Compara o CPF lido do arquivo (aux.cpf) com o CPF do cliente logado 
            aux.reais = cli->reais;         // Atualiza o saldo de Reais
            aux.bitcoin = cli->bitcoin;     // Atualiza o saldo de Bitcoin
            aux.ethereum = cli->ethereum;   // Atualiza o saldo de Ethereum
            aux.ripple = cli->ripple;       // Atualiza o saldo de Ripple
        }
        
        fprintf(temp, "%s %s %s %.2f %.4f %.4f %.2f\n", // Escreve os dados atualizados em um arquivo temporário
                nomeArquivo, aux.cpf, aux.senha, 
                aux.reais, aux.bitcoin, aux.ethereum, aux.ripple);
    }

    // Fecha os dois arquivos
    fclose(arquivo);
    fclose(temp);

    
    remove("investidores.txt");     // Apaga o arquivo antigo com os dados anteriores
    rename("temp.txt", "investidores.txt"); // Renomeia o arquivo temp para investidores
}

void registrarExtrato(Investidor *cli, char operacao, float valor, const char *moeda, float cotacaoUsada, float taxaReal) {
    char nomeArquivo[50];
    sprintf(nomeArquivo, "extrato_%s.txt", cli->cpf); // Cria o nome do arquivo usando o CPF do cliente
    FILE *extrato = fopen(nomeArquivo, "a");          
    if (extrato == NULL) return;                      // Se houver falha ao abrir/criar o arquivo, encerra a função

    time_t t = time(NULL);                            // Obtém o tempo atual do sistema em segundos
    struct tm tm = *localtime(&t);                    // Converte o tempo do sistema para (ano, mês, dia, hora, minuto)

    // Escreve a linha no arquivo de extrato com a data, hora, tipo de operação, valor, moeda, cotação, taxas e saldos atuais
    fprintf(extrato, "%02d-%02d-%04d %02d:%02d %c %.2f %s CT: %.2f TX: %.2f REAL: %.2f BTC: %.4f ETH: %.4f XRP: %.2f\n",
            tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min,
            operacao, valor, moeda, cotacaoUsada, taxaReal,
            cli->reais, cli->bitcoin, cli->ethereum, cli->ripple);
    
    fclose(extrato); // Fecha o arquivo do extrato para salvar as alterações
}

void exibirMenuPrincipal() {
    printf("\n=== BEM-VINDO A EXCHANGE DE CRIPTOMOEDAS ===\n");
    printf("1. Criar nova conta\n");
    printf("2. Realizar login\n");
    printf("3. Sair\n");
    printf("Escolha uma opcao: ");
}

void exibirMenuLogado() {
    printf("\n=== MENU DA EXCHANGE ===\n");
    printf("1. Consultar saldo\n");
    printf("2. Consultar extrato\n");
    printf("3. Depositar reais\n");
    printf("4. Sacar reais\n");
    printf("5. Comprar criptomoedas\n");
    printf("6. Vender criptomoedas\n");
    printf("7. Atualizar cotacao\n");
    printf("8. Terminar sessao (Logout)\n");
    printf("Escolha uma opcao: ");
}

void criarConta() {
    Investidor novo;          // Declara uma variável para guardar um novo investidor
    char nomeTemporario[255];  // Vetor para guardar o nome completo
    
    printf("\n--- CRIAR NOVA CONTA ---\n");
    printf("Informe o seu Nome completo: ");
    scanf(" %[^\n]", nomeTemporario); // Lê o nome completo incluindo os espaços
    printf("Informe o seu CPF (somente numeros, 11 digitos): ");
    scanf("%s", novo.cpf);           // Lê o CPF como string
    printf("Crie uma senha (6 digitos): ");
    scanf("%s", novo.senha);         // Lê a senha como string

    // Contas novas começam com saldo zero em todos os aspectos
    novo.reais = 0.0f;
    novo.bitcoin = 0.0f;
    novo.ethereum = 0.0f;
    novo.ripple = 0.0f;

    // Substitui os espaços do nome por _ pra nao dar erro 
    for (int i = 0; nomeTemporario[i] != '\0'; i++) {
        if (nomeTemporario[i] == ' ') {
            nomeTemporario[i] = '_';
        }
    }

    FILE *arquivo = fopen("investidores.txt", "a");  // Abre o arquivo pra adicionar informações

    // Escreve os dados do novo investidor numa nova linha do arquivo
    fprintf(arquivo, "%s %s %s %.2f %.4f %.4f %.2f\n", 
            nomeTemporario, novo.cpf, novo.senha, 
            novo.reais, novo.bitcoin, novo.ethereum, novo.ripple);
    fclose(arquivo); // Fecha o arquivo para salvar
    printf("\nConta criada com sucesso! Ja pode realizar o login.\n");
}

// Função para validar o login do investidor
int realizarLogin(Investidor *cli) {
    char cpfDigitado[12];     // Guardar temporariamente
    char senhaDigitada[7];   
    char nomeArquivo[50];     // Ler o nome do arquivo
    int loginSucesso = 0;     // Variável para ver se o login está correto

    printf("\n--- REALIZAR LOGIN ---\n");
    printf("Informe o seu CPF: ");
    scanf("%s", cpfDigitado);
    printf("Informe a senha (6 digitos): ");
    scanf("%s", senhaDigitada);

    FILE *arquivo = fopen("investidores.txt", "r");
    if (arquivo == NULL) {
        printf("\nNenhum investidor cadastrado.\n");
        return 0; // Retorna 0 se o arquivo não existir
    }

    // Lê as linhas do arquivo até encontrar os 7 campos correspondentes
    while (fscanf(arquivo, "%s %s %s %f %f %f %f", 
                  nomeArquivo, cli->cpf, cli->senha, 
                  &cli->reais, &cli->bitcoin, &cli->ethereum, &cli->ripple) == 7) {
        // Volta os _ do nome gravado no arquivo de volta para espaços
        for (int i = 0; nomeArquivo[i] != '\0'; i++) {
            if (nomeArquivo[i] == '_') nomeArquivo[i] = ' ';
        }
        strcpy(cli->nome, nomeArquivo); // Copia o nome do arquivo para o nome
        // Compara se o CPF e a senha digitados são iguais com os do cadastro
        if (strcmp(cpfDigitado, cli->cpf) == 0 && strcmp(senhaDigitada, cli->senha) == 0) {
            loginSucesso = 1; // Marca o login como bem-sucedido
            break;            // Interrompe o loop pois o utilizador foi encontrado
        }
    }
    fclose(arquivo); // Fecha o arquivo depois de acabar a busca

    // Mensagem final de sucesso ou erro
    if (loginSucesso) {
        printf("\nLogin realizado com sucesso! Bem-vindo(a), %s.\n", cli->nome);
        return 1;
    } else {
        printf("\nErro: CPF ou senha incorretos!\n");
        return 0;
    }
}

// Consultar os saldos e dados do investidor
void consultarSaldo(Investidor *cli) {
    char senhaDigitada[7]; // Armazenar a senha informada pelo usuario
    printf("\n--- CONSULTAR SALDO ---\n");
    printf("Por favor, confirme a sua senha (6 digitos): ");
    scanf("%s", senhaDigitada); // Lê a senha digitada

    // Compara a senha digitada com a senha real do cliente logado
    if (strcmp(senhaDigitada, cli->senha) == 0) {
        printf("\n=== SEUS SALDOS ===\n");
        printf("Nome: %s\n", cli->nome);       // Mostra o nome do cliente
        printf("CPF: %s\n", cli->cpf);         // Mostra o CPF
        printf("Reais: R$ %.2f\n", cli->reais); // Mostra o saldo em Reais
        printf("Bitcoin: %.4f BTC\n", cli->bitcoin);   // Mostra o saldo de Bitcoin 
        printf("Ethereum: %.4f ETH\n", cli->ethereum); // Mostra o saldo de Ethereum 
        printf("Ripple: %.2f XRP\n", cli->ripple);     // Mostra o saldo de Ripple 
        printf("===================\n");
    } else {
        printf("\nSenha incorreta!\n");
    }
}

// Consultar o extrato do investidor
void consultarExtrato(Investidor *cli) {
    char senhaDigitada[7];  // Guardar a senha de 6 dígitos digitada na confirmação
    char linha[200];        // Armazenar e escrever cada linha do extrato lida no arquivo
    char nomeArquivo[50];   // Montar o nome do arquivo do extrato usando o CPF
    printf("\n--- CONSULTAR EXTRATO ---\n");
    printf("Por favor, confirme a sua senha (6 digitos): ");
    scanf("%s", senhaDigitada); // Lê a senha digitada
    // Validação da senha
    if (strcmp(senhaDigitada, cli->senha) != 0) {
        printf("\nSenha incorreta!\n");
        return;
    }

    // Gera o nome do arquivo de extrato usando o CPF do cusuario
    sprintf(nomeArquivo, "extrato_%s.txt", cli->cpf);
    FILE *extrato = fopen(nomeArquivo, "r");
    // Se o arquivo não existir, o usuario ainda não fez nenhuma transação
    if (extrato == NULL) {
        printf("\nNao existem operacoes registradas no seu extrato ainda.\n");
        return;
    }
    printf("\nNome: %s\n", cli->nome); // Mostra o nome do investidor logado
    printf("CPF: %s\n", cli->cpf);     // Mostra o CPF do investidor logado

    while (fgets(linha, sizeof(linha), extrato) != NULL) {  // Loop que lê o arquivo de extrato até atingir o fim
        printf("%s", linha); // Escreve o conteúdo de cada linha do arquivo de extrato
    }
    fclose(extrato); // Fecha o arquivo de extrato depois de ler
}

// Depositar Reais na conta
void depositarReais(Investidor *cli) {
    float valor; // Guardar o valor que o usuario quer depositar
    printf("\n--- DEPOSITAR REAIS ---\n");
    printf("Valor do deposito (R$): ");
    scanf("%f", &valor); // Lê o valor digitado
    if (valor <= 0) {   // Garantir valores positivos no deposito
        printf("\nValor invalido.\n");
        return; // Sai da função se o valor for negativo
    }
    cli->reais += valor;                // Adiciona o valor depositado ao saldo usuario
    salvarDadosInvestidor(cli);         // Atualiza os dados do investidor no arquivo
    registrarExtrato(cli, '+', valor, "REAL", 0.0f, 0.0f); // Escreve a transacao de depósito no extrato
    printf("\nDeposito realizado! Novo saldo: R$ %.2f\n", cli->reais);
}

// Saque de reais da conta
void sacarReais(Investidor *cli) {
    float valor; // Guarda o valor que o usuario quer sacar
    printf("\n--- SACAR REAIS ---\n");
    printf("Valor do saque (R$): ");
    scanf("%f", &valor); // Lê o valor do saque
    if (valor <= 0) {   // Ve se o valor é positivo
        printf("\nErro: Valor invalido.\n");
        return; // Sai da função
    }
    if (valor > cli->reais) {   // Ve se usuario possui saldo para fazer o saque
        printf("\nSaldo insuficiente! Saldo atual: R$ %.2f\n", cli->reais);
        return; // Sai da função sem mudar o saldo
    }
    cli->reais -= valor;                   // Tira o valor sacado do saldo atual
    salvarDadosInvestidor(cli);            // Atualiza os dados do usuario no arquivo 
    registrarExtrato(cli, '-', valor, "REAL", 0.0f, 0.0f); // Escreve o saque no extrato
    printf("\nSaque realizado! Novo saldo: R$ %.2f\n", cli->reais);
}

// Comprar criptomoedas
void comprarCriptomoedas(Investidor *cli) {
    char senhaDigitada[7];       // Guardar a senha de 6 dígitos digitada na confirmação
    printf("\n--- COMPRAR CRIPTOMOEDAS ---\n");
    printf("Confirme sua senha: ");
    scanf("%s", senhaDigitada); // Lê a senha digitada
    if (strcmp(senhaDigitada, cli->senha) != 0) {   // Se a senha estiver errada cancela a operação
        printf("\nSenha incorreta!\n");
        return; // Finaliza a funcao
    }
    int opcao;                               // Guarda a opcao escolhida
    float valorReais;                        // Guarda o valor que quer investir
    float taxaBTC = 0.02f;                   // Taxas cobradas
    float taxaETH = 0.01f;                   
    float taxaXRP = 0.01f;                   
    printf("\n=== COTACOES ATUAIS ===\n");   // Menu com as cotações atuais e as taxas de transação
    printf("1. Bitcoin  (CT: R$ %.2f | Taxa: 2%%)\n", cotacaoBTC);
    printf("2. Ethereum (CT: R$ %.2f | Taxa: 1%%)\n", cotacaoETH);
    printf("3. Ripple   (CT: R$ %.2f | Taxa: 1%%)\n", cotacaoXRP);
    printf("Escolha a moeda: ");
    scanf("%d", &opcao);
    if (opcao < 1 || opcao > 3) {   // Ve se a opção está dentro de 1 a 3
        printf("\nOpcao invalida!\n");
        return; // Finaliza a funcao
    }

    printf("Valor em Reais (R$) a investir: ");
    scanf("%f", &valorReais);   // Lê o valor que o usuario quer comprar as criptos
    if (valorReais <= 0 || valorReais > cli->reais) {   // Ve se o usuario tem saldo e se o valor é positivo
        printf("\nValor invalido ou saldo insuficiente (R$ %.2f).\n", cli->reais);
        return; // Finaliza a funcao
    }
    float cotacao = 0, taxa = 0; // Variáveis para guardar a cotação e a taxa da moeda 
    char moeda[10];              // Armazenar o símbolo da moeda BTC, ETH, XRP
    if (opcao == 1) { cotacao = cotacaoBTC; taxa = taxaBTC; strcpy(moeda, "BTC"); } // Define a cotação, a taxa e a moeda escolhida
    else if (opcao == 2) { cotacao = cotacaoETH; taxa = taxaETH; strcpy(moeda, "ETH"); }
    else if (opcao == 3) { cotacao = cotacaoXRP; taxa = taxaXRP; strcpy(moeda, "XRP"); }

    float valorTaxa = valorReais * taxa;            // Calcula o valor da taxa cobrada 
    float valorLiquido = valorReais - valorTaxa;    // Calcula o valor depois de subtrair a taxa do valor total
    float qtdComprada = valorLiquido / cotacao;     // Calcula a quantidade de criptomoeda comprada
    
    cli->reais -= valorReais;   // Desconta o valor em reais do saldo e adiciona a criptomoeda comprada ao saldo
    if (opcao == 1) cli->bitcoin += qtdComprada;
    else if (opcao == 2) cli->ethereum += qtdComprada;
    else if (opcao == 3) cli->ripple += qtdComprada; 
    salvarDadosInvestidor(cli); // Atualiza os dados no arquivo  
    registrarExtrato(cli, '+', qtdComprada, moeda, cotacao, taxa); // Regista a transacao de compra no extrato
    printf("\nCompra realizada! Adquiriu %.4f %s (Taxa: R$ %.2f)\n", qtdComprada, moeda, valorTaxa);
}

// Opção 6
void venderCriptomoedas(Investidor *cli) {
    char senhaDigitada[7];
    printf("\n--- VENDER CRIPTOMOEDAS ---\n");
    printf("Confirme sua senha: ");
    scanf("%s", senhaDigitada);
    if (strcmp(senhaDigitada, cli->senha) != 0) {
        printf("\nErro: Senha incorreta!\n");
        return;
    }

    int opcao;
    float qtdVenda;
    float taxaBTC = 0.03f, taxaETH = 0.02f, taxaXRP = 0.01f;

    // Menu atualizado mostrando o saldo do utilizador na respetiva moeda
    printf("\n=== COTACOES E SALDOS ATUAIS ===\n");
    printf("1. Bitcoin  (CT: R$ %.2f | Taxa venda: 3%% | Seu saldo: %.4f BTC)\n", cotacaoBTC, cli->bitcoin);
    printf("2. Ethereum (CT: R$ %.2f | Taxa venda: 2%% | Seu saldo: %.4f ETH)\n", cotacaoETH, cli->ethereum);
    printf("3. Ripple   (CT: R$ %.2f | Taxa venda: 1%% | Seu saldo: %.2f XRP)\n", cotacaoXRP, cli->ripple);
    printf("Escolha a moeda que deseja vender: ");
    scanf("%d", &opcao);

    if (opcao < 1 || opcao > 3) {
        printf("\nOpcao invalida!\n");
        return;
    }

    printf("Quantidade da criptomoeda a vender: ");
    scanf("%f", &qtdVenda);

    if (qtdVenda <= 0) {
        printf("\nErro: Quantidade invalida.\n");
        return;
    }

    float cotacao = 0, taxa = 0;
    char moeda[10];

    // Validações melhoradas informando o saldo máximo permitido
    if (opcao == 1) { 
        if (qtdVenda > cli->bitcoin) { 
            printf("\nErro: Saldo BTC insuficiente! Voce tem apenas %.4f BTC.\n", cli->bitcoin); 
            return; 
        }
        cotacao = cotacaoBTC; taxa = taxaBTC; strcpy(moeda, "BTC"); 
    } else if (opcao == 2) { 
        if (qtdVenda > cli->ethereum) { 
            printf("\nErro: Saldo ETH insuficiente! Voce tem apenas %.4f ETH.\n", cli->ethereum); 
            return; 
        }
        cotacao = cotacaoETH; taxa = taxaETH; strcpy(moeda, "ETH"); 
    } else if (opcao == 3) { 
        if (qtdVenda > cli->ripple) { 
            printf("\nErro: Saldo XRP insuficiente! Voce tem apenas %.2f XRP.\n", cli->ripple); 
            return; 
        }
        cotacao = cotacaoXRP; taxa = taxaXRP; strcpy(moeda, "XRP"); 
    }

    float valorBruto = qtdVenda * cotacao;
    float valorTaxa = valorBruto * taxa;
    float valorLiquidoRecebido = valorBruto - valorTaxa;

    if (opcao == 1) cli->bitcoin -= qtdVenda;
    else if (opcao == 2) cli->ethereum -= qtdVenda;
    else if (opcao == 3) cli->ripple -= qtdVenda;
    
    cli->reais += valorLiquidoRecebido;

    salvarDadosInvestidor(cli);
    registrarExtrato(cli, '-', qtdVenda, moeda, cotacao, taxa);
    printf("\nVenda realizada com sucesso! Recebeu R$ %.2f (Taxa descontada: R$ %.2f)\n", valorLiquidoRecebido, valorTaxa);
}

// Opção 7
void atualizarCotacao() {
    printf("\n--- ATUALIZAR COTACAO ---\n");

    float variacaoBTC = (((float)rand() / (float)RAND_MAX) * 0.10f) - 0.05f;
    float variacaoETH = (((float)rand() / (float)RAND_MAX) * 0.10f) - 0.05f;
    float variacaoXRP = (((float)rand() / (float)RAND_MAX) * 0.10f) - 0.05f;

    float btcAnt = cotacaoBTC, ethAnt = cotacaoETH, xrpAnt = cotacaoXRP;

    cotacaoBTC *= (1.0f + variacaoBTC);
    cotacaoETH *= (1.0f + variacaoETH);
    cotacaoXRP *= (1.0f + variacaoXRP);

    printf("Cotacoes atualizadas!\n");
    printf("BTC: de R$ %.2f para R$ %.2f (%+.2f%%)\n", btcAnt, cotacaoBTC, variacaoBTC * 100.0f);
    printf("ETH: de R$ %.2f para R$ %.2f (%+.2f%%)\n", ethAnt, cotacaoETH, variacaoETH * 100.0f);
    printf("XRP: de R$ %.2f para R$ %.2f (%+.2f%%)\n", xrpAnt, cotacaoXRP, variacaoXRP * 100.0f);
}

// -----------------------------------------------------------------------------
// FUNÇÃO PRINCIPAL
// -----------------------------------------------------------------------------

int main() {
    srand((unsigned int)time(NULL));
    int opcaoPrincipal;

    do {
        exibirMenuPrincipal();
        scanf("%d", &opcaoPrincipal);

        switch (opcaoPrincipal) {
            case 1:
                criarConta();
                break;
            case 2: {
                Investidor cli;
                if (realizarLogin(&cli)) {
                    int opcaoMenu;
                    do {
                        exibirMenuLogado();
                        scanf("%d", &opcaoMenu);

                        switch (opcaoMenu) {
                            case 1: consultarSaldo(&cli); break;
                            case 2: consultarExtrato(&cli); break;
                            case 3: depositarReais(&cli); break;
                            case 4: sacarReais(&cli); break;
                            case 5: comprarCriptomoedas(&cli); break;
                            case 6: venderCriptomoedas(&cli); break;
                            case 7: atualizarCotacao(); break;
                            case 8: printf("\nA terminar sessao...\n"); break;
                            default: printf("\nOpcao invalida!\n");
                        }
                    } while (opcaoMenu != 8);
                }
                break;
            }
            case 3:
                printf("\nA sair do programa. Ate logo!\n");
                break;
            default:
                printf("\nOpcao invalida!\n");
        }
    } while (opcaoPrincipal != 3);

    return 0;
}
