#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct QNode {
        struct QNode *anterior, *prox;
        unsigned paginaNumero;
} QNode;

typedef struct Fila {
        unsigned quadrosCheios;
        unsigned numeroQuadros;
        QNode *primeiroFila, *posteriorFila;
} Fila;

typedef struct Hash {
        int capacidade;
        QNode* *vetor;
} Hash;

typedef struct memoriaFis {
        int numero;
        int numeroPagProcesso;
        int alocada;
} paginaMemoria;

typedef struct processo {
        char nome[3];
        int primeiraPag;
        int ultimaPag;
        float numeroTotalPags;
        float numeroPagsAlocadas;
        struct processo *prox;
} processao;

typedef struct pagPro {
        char nome[3];
        int num;
        int alocada;
        unsigned int comeco;
        unsigned int fim;
        struct pagPro *prox;
} paginaProcesso;

int situacaoQuadros();
int situacaoFilaVazia();
void referenciaPagina();
void colocaNaFila();
void tiraDaFila();
Hash* criaHash();
Fila* criaFila();
QNode* novoQNode();

int verificaHit();
int pegaHitIndex();
int paginaEndereco();
void inicia();
void mostraPaginas();
void mostraNumeroPageFaults();
void lru();
void segundaChance();
void novoProcesso();
void appendProcesso();
void appendPagina();
void lruBits();
void verificaPorcentagemPuro();
void verificaPorcentagemBits();

int win = 0; // zero pra linux
int i,j,k,n,paginasFisicas;
int memoriaPrincipalPags[100], numeroPagina[50];
int algEscolhido = 0, hit = 0, qtdePageFault=0;

int main(int argc, char *argv[])
{
        unsigned int endereco, pagina;
        int qtdPagFis, priPag, ultPag, tamanho, qtdPagProcesso, numProcesso, posi, contadorQuadros, quadrosVazios, tamPag, tamMem;
        int *ptrnum, indicePagina=0, num;
        char comando, nomeProcesso[3], unidade[3], argPag[3] = "-p", argMem[3] = "-m";
        QNode *filaPura; //pra imprimir lru puro
        Hash* hash = criaHash( 999 );
        paginaProcesso *paginaNova = NULL;
        processao *cadaProcesso = NULL;

        //  tamPag = 1024;    //kb
        //  tamMem = 4096;    //kb

        //if (argc != 6)
        //  {
        //    printf("Quantidade de parametros errados!");
        //    exit(0);
        //}
        for (int i = 1; i < argc; i += 2)
        {
                if (!strcmp(argv[i], "-p"))
                {
                        tamPag = atoi(argv[i + 1]);
                        continue;
                }
                else if (!strcmp(argv[i], "-m"))
                {
                        tamMem = atoi(argv[i + 1]);
                }
        }
        if (tamPag <= 0 || tamMem <= 0)
        {
                printf("O tamanho precisa ser maior que 0!\n");
                return 0;
        }
        else if (tamMem < tamPag)
        {
                printf("O tamanho de pagina nao pode ser maior que o da memória!\n");
                return 0;
        }
        else if (((tamMem % tamPag)) != 0)
        {
                printf("O tamanho da memória deve ser multiplo do tamanho da pagina!\n");
                return 0;
        }

        printf("\tBEM VINDO(A)!");
        printf("\nEscolha um algoritmo de paginacao:");
        printf("\n1)LRU\n2)Segunda chance\n3)LRU com bit de referencia\n");
        do {
                scanf("%d",&algEscolhido);
                if (algEscolhido > 0 && algEscolhido < 4)
                {
                        break;
                }
                printf("Numero errado, digite novamente:\n");
        } while(1);
        printf("Tamanho da Pagina em KB  : %d\n", tamPag);
        printf("Tamanho da Memoria em KB : %d\n", tamMem);

        qtdPagFis = tamMem / tamPag;
        Fila* filaOriginal = criaFila( qtdPagFis );
        printf("Paginas em cache : %d\n", qtdPagFis);

        paginaMemoria paginaMP[qtdPagFis];

        for (size_t i = 0; i < qtdPagFis; i++)
        {
                paginaMP[i].numero = num++;
                paginaMP[i].numeroPagProcesso = 0;
                paginaMP[i].alocada = 0;
        }

        FILE *pont_arq = fopen("memoria2.txt", "r");

        if (pont_arq == NULL)
                printf("\nErro na abertura do arquivo!");

        num = 1;
        ptrnum = &num;

        n = 99;
        paginasFisicas = qtdPagFis;

        memoriaPrincipalPags[n] = n;

        fflush(stdin);
        getchar();
        if (win == 1) {
                system("cls"); //windows
        }else{
                system("clear"); // linux
        }



        while (!feof(pont_arq))
        {
                printf("\n\n\t\tLendo TXT...\n");
                fscanf(pont_arq, "%s %c", nomeProcesso, &comando);
                if (comando == 'C')
                {
                        printf("\nCRIACAO!!\n");
                        fscanf(pont_arq, "%d %s", &tamanho, unidade);
                        printf("\nNome processo: %s\n", nomeProcesso);
                        printf("Tipo: %c\n", comando);
                        printf("Tamanho: %d\n", tamanho);
                        printf("Unidade: %s\n", unidade);

                        if (!strcmp(unidade, "MB"))
                                tamanho *= 1024;

                        printf("\nCriando paginas do processo...\n");

                        qtdPagProcesso = tamanho / tamPag;

                        if (qtdPagProcesso < 1)
                                qtdPagProcesso = 1;

                        else if ((float)(tamanho % tamPag) != 0)
                                qtdPagProcesso++;

                        printf("\nNumero de Paginas criadas: %d\n", qtdPagProcesso);

                        posi = 0;
                        priPag = num;
                        for (size_t i = 0; i < qtdPagProcesso; i++)
                        {
                                appendPagina(&paginaNova, nomeProcesso, tamPag, posi, *ptrnum);
                                num++;
                                posi += tamPag;
                        }
                        ultPag = num-1;
                        appendProcesso(&cadaProcesso, nomeProcesso, priPag, ultPag, qtdPagProcesso);

                        fflush(stdin);
                        getchar();
                        if (win == 1) {
                                system("cls"); //windows
                        }else{
                                system("clear"); // linux
                        }

                }
                else
                {
                        if (comando == 'R')
                        {
                                printf("\nLEITURA!!\n");
                        }
                        else if (comando == 'W')
                        {
                                printf("\nESCRITA!!\n");
                        }

                        fscanf(pont_arq, "%x", &endereco);
                        printf("\nNome do processo: %s\n", nomeProcesso);
                        printf("Tipo: %c\n", comando);
                        printf("Endereco: %x\n", endereco);
                        printf("\nProcurando pagina na memoria virtual...\n");
                        numProcesso = paginaEndereco(paginaNova, endereco, nomeProcesso);

                        if (numProcesso > 0)
                        {
                                memoriaPrincipalPags[indicePagina] =  numProcesso;
                                indicePagina++;
                                if (algEscolhido == 1) {
                                        inicia();
                                        referenciaPagina( filaOriginal, hash, numProcesso, &filaPura);
                                        filaPura = filaOriginal->primeiroFila;   // pra imprimir lru puro
                                        contadorQuadros = 0;
                                        while (filaPura != NULL) {
                                                printf("\n ------\n");
                                                printf("|  %d  |",filaPura->paginaNumero);
                                                printf("\n ------\n");
                                                filaPura = filaPura->prox;
                                                if (contadorQuadros <= qtdPagFis) {
                                                        contadorQuadros++;
                                                }
                                        }
                                        quadrosVazios = (qtdPagFis-contadorQuadros);
                                        if (quadrosVazios >= 0  ) {
                                                for (size_t i = 0; i < quadrosVazios; i++) {
                                                        printf("\n ------\n");
                                                        printf("|     |");
                                                        printf("\n ------\n");
                                                }
                                        }

                                        verificaPorcentagemPuro(filaOriginal, cadaProcesso);


                                }
                                else if (algEscolhido == 2) {
                                        segundaChance(qtdPagFis);
                                        verificaPorcentagemBits(cadaProcesso);
                                }
                                else{
                                        lruBits(qtdPagFis);
                                        verificaPorcentagemBits(cadaProcesso);
                                }
                        }
                        fflush(stdin);
                        getchar();
                        if (win == 1) {
                                system("cls"); //windows
                        }else{
                                system("clear"); // linux
                        }
                }
        }
        printf("FIM DA SIMULAÇÃO :)\n\n");
        fclose(pont_arq);
}

void verificaPorcentagemPuro(Fila *filaOriginal, processao *cadaProcesso) {
        float porcentagem, qtdePagsAlocadas = 0,totalPags = 0;
        QNode *filaResultado; //pra imprimir lru puro
        QNode *aux;
        while (cadaProcesso != NULL) {
                qtdePagsAlocadas = 0;
                filaResultado = filaOriginal->primeiroFila;
                for (aux = filaResultado; aux != NULL; aux = aux->prox) {
                        if ((aux->paginaNumero>=cadaProcesso->primeiraPag) && (aux->paginaNumero<=cadaProcesso->ultimaPag))
                                qtdePagsAlocadas++;
                }
                totalPags = (float) cadaProcesso->numeroTotalPags;
                porcentagem = (qtdePagsAlocadas/totalPags) * 100;
                printf("\n%.2f porcento do processo %s", porcentagem, cadaProcesso->nome);
                cadaProcesso = cadaProcesso->prox;
        }
}

void verificaPorcentagemBits(processao *cadaProcesso) {
        float porcentagem, qtdePagsAlocadas = 0,totalPags = 0;
        while (cadaProcesso != NULL) {
                qtdePagsAlocadas = 0;
                for (i=0; i<paginasFisicas; i++) {
                        if ((numeroPagina[i]>= cadaProcesso->primeiraPag) && (numeroPagina[i]<=cadaProcesso->ultimaPag))
                        {
                                qtdePagsAlocadas++;
                        }
                }
                totalPags = (float) cadaProcesso->numeroTotalPags;
                porcentagem = (qtdePagsAlocadas/totalPags) * 100;
                printf("\n%.2f porcento do processo %s", porcentagem, cadaProcesso->nome);
                cadaProcesso = cadaProcesso->prox;
        }
}

void inicia() {
        qtdePageFault=0;
        for(i=0; i<paginasFisicas; i++)
                numeroPagina[i]= -1;
}

int verificaHit(int paginaAtual) {
        hit=0;
        for(j=0; j<paginasFisicas; j++)
        {
                if(numeroPagina[j]==paginaAtual)
                {
                        hit=1;
                        break;
                }
        }
        return hit;
}

int pegaHitIndex(int paginaAtual) {
        int hitInd;
        for(k=0; k<paginasFisicas; k++)
        {
                if(numeroPagina[k]==paginaAtual)
                {
                        hitInd=k;
                        break;
                }
        }
        return hitInd;
}

void mostraPaginas(int qtdPagFis) {
        int contador = 0;
        for (k=0; k<paginasFisicas; k++)
        {
                if (contador < qtdPagFis)
                {
                        if(numeroPagina[k]!= -1) //se a pagina nao tiver um valor definido
                        {
                                printf("\n ------\n");
                                printf("|  %d  |",numeroPagina[k]);
                                printf("\n ------\n");
                        }else{
                                printf("\n ------\n");
                                printf("|     |");
                                printf("\n ------\n");
                        }
                        contador++;
                }
        }


}

void mostraNumeroPageFaults() {
        printf("\nPage faults ate agora: %d",qtdePageFault);
}

void lru(int qtdPagFis) {
        mostraPaginas(qtdPagFis);
}

void segundaChance(int qtdPagFis) {
        int bitUsado[50], hitIndex;
        int paginaVitima=0;
        inicia();
        for(i=0; i<paginasFisicas; i++)
                bitUsado[i]=0;
        for(i=0; i<n; i++)
        {
                if (memoriaPrincipalPags[i] != 0 && memoriaPrincipalPags[i] != memoriaPrincipalPags[n]) {
                        if(verificaHit(memoriaPrincipalPags[i]))
                        {
                                hitIndex=pegaHitIndex(memoriaPrincipalPags[i]);
                                if(bitUsado[hitIndex]==0)
                                        bitUsado[hitIndex]=1;
                        }
                        else
                        {
                                qtdePageFault++;
                                if(bitUsado[paginaVitima]==1)
                                {
                                        do
                                        {
                                                bitUsado[paginaVitima]=0;
                                                paginaVitima++;
                                                if(paginaVitima==paginasFisicas)
                                                        paginaVitima=0;
                                        }
                                        while(bitUsado[paginaVitima]!=0);
                                }
                                if(bitUsado[paginaVitima]==0)
                                {
                                        numeroPagina[paginaVitima]=memoriaPrincipalPags[i];
                                        bitUsado[paginaVitima]=1;
                                        paginaVitima++;
                                }
                        }
                        if(paginaVitima==paginasFisicas)
                                paginaVitima=0;
                }
        }
        mostraPaginas(qtdPagFis);
        mostraNumeroPageFaults();

}

void lruBits (int qtdPagFis) {

        inicia();

        int menosRecente[50];
        for(i=0; i<n; i++)
        {

                if (memoriaPrincipalPags[i] != 0 && memoriaPrincipalPags[i] != memoriaPrincipalPags[n])
                {
                        if(verificaHit(memoriaPrincipalPags[i])==0)
                        {

                                for(j=0; j<paginasFisicas; j++)
                                {
                                        int numeropagina = numeroPagina[i];
                                        int encontrada = 0;
                                        for(k=i-1; k>=0; k--)
                                        {
                                                if(numeroPagina[j]==memoriaPrincipalPags[k])
                                                {
                                                        menosRecente[j]=k;
                                                        encontrada=1;
                                                        break;
                                                }
                                                else
                                                        encontrada=0;
                                        }
                                        if(!encontrada)
                                                menosRecente[j]=-9999;
                                }
                                int min=9999;
                                int indiceMemoriaPrincipal;
                                for(j=0; j<paginasFisicas; j++)
                                {
                                        if(menosRecente[j]<min) //se não foi  encontrada
                                        {
                                                min=menosRecente[j];
                                                indiceMemoriaPrincipal=j;
                                        }
                                }
                                numeroPagina[indiceMemoriaPrincipal]=memoriaPrincipalPags[i];
                                qtdePageFault++;
                        }
                }
        }
        mostraPaginas(qtdPagFis);
        mostraNumeroPageFaults();
}

void appendProcesso(processao **headProcessos, char nome[], int priPag, int ultPag, int qtdPagProcesso) {
        processao *novoNodoProcesso = (processao *)malloc(sizeof(processao));

        processao *ultimoProcesso = *headProcessos;

        strcpy(novoNodoProcesso->nome, nome);
        novoNodoProcesso->primeiraPag = priPag;
        novoNodoProcesso->ultimaPag = ultPag;
        novoNodoProcesso->numeroTotalPags = qtdPagProcesso;
        novoNodoProcesso->numeroPagsAlocadas = 0;

        novoNodoProcesso->prox = NULL;

        if (*headProcessos == NULL)
        {
                *headProcessos = novoNodoProcesso;
                return;
        }

        while (ultimoProcesso->prox != NULL)
                ultimoProcesso = ultimoProcesso->prox;

        ultimoProcesso->prox = novoNodoProcesso;
        return;
}

void appendPagina(paginaProcesso **headPaginas, char nome[], int tamPag, int posi, int num) {
        paginaProcesso *novoNodoPaginas = (paginaProcesso *)malloc(sizeof(paginaProcesso));
        paginaProcesso *ultimaPagina = *headPaginas;

        strcpy(novoNodoPaginas->nome, nome);
        novoNodoPaginas->num = num;
        novoNodoPaginas->comeco = posi;
        num++;
        posi += tamPag;
        novoNodoPaginas->fim = posi;

        novoNodoPaginas->prox = NULL;

        if (*headPaginas == NULL)
        {
                *headPaginas = novoNodoPaginas;
                return;
        }

        while (ultimaPagina->prox != NULL)
                ultimaPagina = ultimaPagina->prox;

        ultimaPagina->prox = novoNodoPaginas;
        return;
}

int paginaEndereco(paginaProcesso *paginaNova, unsigned int enderecoLido, char nomeProcesso[]) {

        unsigned int comeco = paginaNova->comeco;
        unsigned int fim = paginaNova->fim;

        while (paginaNova != NULL)
        {
                comeco = paginaNova->comeco;
                fim = paginaNova->fim;

                if (strcmp(paginaNova->nome, nomeProcesso) == 0)
                {
                        if ((enderecoLido >= paginaNova->comeco) && (enderecoLido <= paginaNova->fim))
                        {
                                printf("\nO endereco esta na pagina %d do processo %s\ncomeco: %d fim: %d", paginaNova->num, paginaNova->nome, paginaNova->comeco, paginaNova->fim);
                                printf("\nEndereco da pagina: %d\n", paginaNova->num);
                                return paginaNova->num;
                        }
                }
                paginaNova = paginaNova->prox;
        }
        printf("\n\nO endereco %x nao esta dentro das paginas criadas do processo!",enderecoLido);
        return 0;
}

//LRU PURO

QNode* novoQNode( unsigned paginaNumero ) {
        QNode* temp = (QNode *)malloc( sizeof( QNode ) );
        temp->paginaNumero = paginaNumero;

        temp->anterior = temp->prox = NULL;

        return temp;
}

Fila* criaFila( int numeroQuadros ) {
        Fila* filinha = (Fila *)malloc( sizeof( Fila ) );

        filinha->quadrosCheios = 0;
        filinha->primeiroFila = filinha->posteriorFila = NULL;

        filinha->numeroQuadros = numeroQuadros;

        return filinha;
}

// A utility function to create an empty Hash of given capacidade
Hash* criaHash( int capacidade ) {
        Hash* hash = (Hash *) malloc( sizeof( Hash ) );
        hash->capacidade = capacidade;

        hash->vetor = (QNode **) malloc( hash->capacidade * sizeof( QNode* ) );

        int i;
        for( i = 0; i < hash->capacidade; ++i )
                hash->vetor[i] = NULL;

        return hash;
}

int situacaoQuadros( Fila* filinha ) {
        return filinha->quadrosCheios == filinha->numeroQuadros;
}

int situacaoFilaVazia( Fila* filinha ) {
        return filinha->posteriorFila == NULL;
}

void tiraDaFila( Fila* filinha ) {
        if( situacaoFilaVazia( filinha ) )
                return;

        if (filinha->primeiroFila == filinha->posteriorFila)
                filinha->primeiroFila = NULL;

        QNode* temp = filinha->posteriorFila;
        filinha->posteriorFila = filinha->posteriorFila->anterior;

        if (filinha->posteriorFila)
                filinha->posteriorFila->prox = NULL;

        free( temp );

        filinha->quadrosCheios--;
}

void colocaNaFila( Fila* filinha, Hash* hash, unsigned paginaNumero ) {
        // Remove o ultimo da fila
        if ( situacaoQuadros ( filinha ) )
        {
                hash->vetor[ filinha->posteriorFila->paginaNumero ] = NULL;
                tiraDaFila( filinha );
        }

        QNode* temp = novoQNode( paginaNumero );
        temp->prox = filinha->primeiroFila;

        // Se a fila estiver vazia, muda o primeiro e posterior
        if ( situacaoFilaVazia( filinha ) )
                filinha->posteriorFila = filinha->primeiroFila = temp;
        else
        {
                filinha->primeiroFila->anterior = temp;
                filinha->primeiroFila = temp;
        }

        hash->vetor[ paginaNumero ] = temp;

        filinha->quadrosCheios++;
}

//Função para referenciar uma pagina da memoria principal. Duas opçes:
// 1. O quadro nao está na memoria; trazemos para a memoria e adicionamos em primeiro da fila
// 2. O quadro já está na memoria; movemos o frame para primeiro da fila
void referenciaPagina( Fila* filinha, Hash* hash, unsigned paginaNumero, QNode *filaPura ) {
        QNode* reqPage = hash->vetor[ paginaNumero ];

        if ( reqPage == NULL )
                colocaNaFila( filinha, hash, paginaNumero );

        else if (reqPage != filinha->primeiroFila)
        {
                //tira do lugar atual
                reqPage->anterior->prox = reqPage->prox;
                if (reqPage->prox)
                        reqPage->prox->anterior = reqPage->anterior;

                if (reqPage == filinha->posteriorFila)
                {
                        filinha->posteriorFila = reqPage->anterior;
                        filinha->posteriorFila->prox = NULL;
                }

                // Coloca no começo
                reqPage->prox = filinha->primeiroFila;
                reqPage->anterior = NULL;

                reqPage->prox->anterior = reqPage;

                filinha->primeiroFila = reqPage;
        }
}
