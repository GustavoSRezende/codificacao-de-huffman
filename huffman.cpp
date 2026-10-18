#include <iostream>
#include <string>
#include <queue>
#include <unordered_map>
#include <vector>
#include <chrono>
#include <fstream>  
#include <sstream>

using namespace std;

struct NoHuffman {
    char caractere; //armazena o caractere do texto (ou '\0' para nos internos)
    int frequencia; //armazena a frequencia do caractere
    NoHuffman *esquerda, *direita; //ponteiros para os filhos esquerdo e direito

    NoHuffman(char c, int f) : caractere(c), frequencia(f), esquerda(nullptr), direita(nullptr) {} //construtor para nos folha (caracteres do texto), recebe apenas a letra e a frequencia e os ponteiros sao nulos

    NoHuffman(char c, int f, NoHuffman* esq, NoHuffman* dir) : caractere(c), frequencia(f), esquerda(esq), direita(dir) {} //construtor para nos internos, acionado no momento que o algoritmo decide juntar as duas menores frequencias
};  

struct Comparador { //regra de ordenacao para a fila de prioridade, que vai organizar os nos de acordo com a frequencia (menor frequencia tem maior prioridade)
    bool operator()(NoHuffman* esq, NoHuffman* dir) { //permite que a fila chame essa funcao para comparar dois nos, e assim decidir qual tem maior prioridade
        return esq->frequencia > dir->frequencia; //verifica se a frequencia do no da esquerda é maior que a do no da direita, se for, retorna true, indicando que o no da esquerda tem menor prioridade (vai para o final da fila)
    }
};

void gerarCodigosBits(NoHuffman* raiz, string codigoAtual, unordered_map<char, string>& dicionarioCodigos) { //vai percorrer a arvore recursivamente e gerar os codigos de cada caractere 
    if (raiz == nullptr) return; //a letra e seu codigo correspondente sao armazenados no dicionarioCodigos, que é um hashmap
    //exemplo, se fosse a palavra "BATATA", o dicionarioCodigos poderia ser { 'A': "0", 'T': "10", 'B': "11" } 
    // Se for um nó folha, guarda o código acumulado
    if (!raiz->esquerda && !raiz->direita) {
        dicionarioCodigos[raiz->caractere] = codigoAtual; //se for um nó folha, guarda o código acumulado
    }

    gerarCodigosBits(raiz->esquerda, codigoAtual + "0", dicionarioCodigos); //chama recursivamente 
    gerarCodigosBits(raiz->direita, codigoAtual + "1", dicionarioCodigos);
}

void libertarMemoria(NoHuffman* raiz) { //função para liberar a memória alocada para a árvore de Huffman
    if (raiz == nullptr) return; //apaga os filhos antes de apagar o pai, para evitar vazamento de memória
    libertarMemoria(raiz->esquerda);
    libertarMemoria(raiz->direita);
    delete raiz;
}

void executarHuffman(const string& texto, bool mostrarPassos = false) {
    if (texto.empty()) return;

    unordered_map<char, int> frequencias; //contagem de frequências de cada caractere no texto
    for (char ch : texto) { //percorre cada caractere do texto e incrementa a contagem de frequências no hashmap
        frequencias[ch]++;
    }

    priority_queue<NoHuffman*, vector<NoHuffman*>, Comparador> filaPrioridade;
    for (auto par : frequencias) { //percorre o hashmap de frequências e cria um nó de Huffman para cada caractere, adicionando-o à fila de prioridade, auto diz pro computador descobrir o tipo de dado sozinho
        filaPrioridade.push(new NoHuffman(par.first, par.second)); //pega uma linha, como A=3 e divide essa informacao em dois, o caractere e a frequencia, e cria um nó de Huffman com esses dados, adicionando-o à fila de prioridade
    } //esse no de Huffman ainda nao esta ligado a nada

    if (mostrarPassos) cout << "\n--- Decisoes do Algoritmo Guloso ---\n"; //false por padrao, mas se for true, vai mostrar o passo a passo do algoritmo guloso, mostrando quais nos estão sendo unidos e suas frequências

    while (filaPrioridade.size() > 1) { //enquanto houver mais de um nó na fila, continua juntando os dois menores nós
        NoHuffman* menor1 = filaPrioridade.top(); //pega o nó com menor frequência (topo da fila)
        filaPrioridade.pop();

        NoHuffman* menor2 = filaPrioridade.top(); //pega o nó com a segunda menor frequência (topo da fila)
        filaPrioridade.pop();

        int somaFrequencias = menor1->frequencia + menor2->frequencia; //soma as frequências dos dois nós menores
        
        if (mostrarPassos) { //se mostrarPassos for true, mostra o passo a passo do algoritmo guloso
            cout << "Juntou [" << (menor1->caractere == '\0' ? "Nó Interno" : string(1, menor1->caractere)) //verifica se o caractere é '\0', se for, mostra "Nó Interno", caso contrário, mostra o caractere
                 << "](" << menor1->frequencia << ") e [" 
                 << (menor2->caractere == '\0' ? "No Interno" : string(1, menor2->caractere)) 
                 << "](" << menor2->frequencia << ") -> Novo No(" << somaFrequencias << ")\n"; 
        }

        NoHuffman* novoNo = new NoHuffman('\0', somaFrequencias, menor1, menor2); //cria um novo no interno com a soma das frequencias dos nos menores, e os dois nos menores como filhos da esquerda e direita
        filaPrioridade.push(novoNo); //add o novo no a fila de prioridade
    }       //quando sobra apenas um nó na fila, esse nó é a raiz da árvore de Huffman

    NoHuffman* raiz = filaPrioridade.top(); //pega o nó restante na fila, que é a raiz da árvore de Huffman

    unordered_map<char, string> dicionarioCodigos; 
    gerarCodigosBits(raiz, "", dicionarioCodigos); //chama a funcao para gerar os códigos de bits para cada caractere, passa uma string vazia como codigo inicial, para preencher de acordo com o caminho percorrido

    cout << "\n--- Tabela de Dicionario ---\n";
    for (auto par : dicionarioCodigos) { //percorre o hashmap de códigos de bits e mostra cada caractere, seu código correspondente e sua frequência
        cout << "'" << par.first << "' : " << par.second << " (Freq: " << frequencias[par.first] << ")\n";
    }

    bool imprimirTexto = texto.length() <= 200; //se o texto for muito grande, nao imprime o texto original nem o codificado para nao poluir a tela
    cout << "\nTexto Original (" << texto.length() * 8 << " bits)" << (imprimirTexto ? ": " + texto : " [texto muito grande, nao exibido]") << "\n"; //mostra o tamanho do texto original em bits, considerando que cada caractere ocupa 8 bits
    
    cout << "Texto Codificado: " << (imprimirTexto ? "" : "[texto muito grande, nao exibido]"); //mostra o texto codificado, percorrendo cada caractere do texto original e substituindo pelo código correspondente no dicionarioCodigos
    int bitsComprimidos = 0;
    for (char ch : texto) {
        if (imprimirTexto) cout << dicionarioCodigos[ch] << " ";
        bitsComprimidos += dicionarioCodigos[ch].length(); //soma o tamanho do codigo correspondente ao caractere, para calcular o tamanho total do texto comprimido em bits
    }                                                      //por exemplo se T corresponde a 10, ele soma 2 bits
    cout << "\nTamanho Apos Compressao: " << bitsComprimidos << " bits\n"; //mostra o tamanho do texto comprimido em bits, que é a soma dos tamanhos dos códigos correspondentes a cada caractere do texto original
    cout << "--------------------------------------------------\n";

    libertarMemoria(raiz); //libera a memória alocada para a árvore de Huffman, chamando a função recursiva que percorre a árvore e deleta cada nó
}

int main() {
    cout << "==================================================\n";
    cout << "TESTE 1: PEQUENA INSTANCIA (Demonstracao)\n";
    cout << "==================================================";
    string teste1 = "TESTE"; //pequena instancia
    
    auto inicio1 = chrono::high_resolution_clock::now();
    executarHuffman(teste1, true); 
    auto fim1 = chrono::high_resolution_clock::now(); //registro do tempo final depois da execução do algoritmo
    auto duracao1 = chrono::duration_cast<chrono::milliseconds>(fim1 - inicio1); //calcula a duração da execução do algoritmo em milissegundos

    cout << "Tempo de Execucao (Teste 1): " << duracao1.count() << " milissegundos\n"; //imprime o tempo de execução do algoritmo para a pequena instância

    cout << "\n==================================================\n";
    cout << "TESTE 2: MEDIA INSTANCIA\n";
    cout << "==================================================";
    string teste2 = "ESTRUTURAS DE DADOS E ALGORITMOS GULOSOS"; //media instancia

    auto inicio2 = chrono::high_resolution_clock::now();
    executarHuffman(teste2, false);
    auto fim2 = chrono::high_resolution_clock::now(); //registro do tempo final depois da execução do algoritmo
    auto duracao2 = chrono::duration_cast<chrono::milliseconds>(fim2 - inicio2); //calcula a duração da execução do algoritmo em milissegundos

    cout << "Tempo de Execucao (Teste 2): " << duracao2.count() << " milissegundos\n"; //imprime o tempo de execução do algoritmo para a média instância

    cout << "\n==================================================\n";
    cout << "TESTE 3: GRANDE INSTANCIA (Eficiencia)\n";
    cout << "==================================================\n";

    ifstream arquivo("texto_gigante.txt");
    string teste3; 

    if (arquivo.is_open()) {
        stringstream buffer;
        buffer << arquivo.rdbuf(); // Joga todo o conteúdo do arquivo no buffer
        teste3 = buffer.str();     // Converte o buffer para a nossa string padrão
        arquivo.close();
        cout << "Arquivo lido com sucesso! Tamanho: " << teste3.length() << " caracteres.\n";
    } else {
        cout << "ERRO: O arquivo 'texto_gigante.txt' nao foi encontrado.\n";
        cout << "Crie um arquivo com esse nome na mesma pasta do terminal.\n";
        return 1; // Encerra o programa se não achar o arquivo
    }
    
    auto inicio3 = chrono::high_resolution_clock::now(); //registro do tempo inicial
    
    executarHuffman(teste3, false);
    
    auto fim3 = chrono::high_resolution_clock::now(); //registro do tempo final depois da execução do algoritmo
    auto duracao3 = chrono::duration_cast<chrono::milliseconds>(fim3 - inicio3); //calcula a duração da execução do algoritmo em milissegundos
    
    cout << "Tempo de Execucao (Teste 3): " << duracao3.count() << " milissegundos\n"; //imprime o tempo de execução do algoritmo para a grande instância
    cout << "==================================================\n"; 

    return 0;
}