#include <stdio.h>
#include <stdlib.h>
#include "B-tree.h"
#include <string.h>

#define MAX 4 //maximo de ITENS na árvore
#define MIN 2
#define TAMt 128 //tamanho de cada registro do arquivo de turma

Pagina* raiz = NULL;

void abrirArquivos(FILE** turma, FILE** indices)
{
	//note q eu não faço IDEIA de se uma declaração dessas ta 100% certa mas fico bonitinha e funciona ent vai ficar até quebrar algo
	*turma = fopen("turma.dat","r+") ? : fopen("turma.dat","w+");
	*indices = fopen("ibtree.idx","r+") ? : fopen("ibtree.idx","w+");

	if(*turma == NULL)
	{
		printf("Erro ao abrir o arquivo de turma!!");
		exit(1);
	}
	if(*indices == NULL)
	{
		printf("Erro ao abrir o arquivo de indices!!");
		exit(1);
	}
}

Aluno* lerRegistro(int RRN, FILE* turma)
{
	char string[129];
	char* nome;
	char* curso;
	int RA;
	int tamArq;

	fseek(turma,0L,SEEK_END);
	tamArq = ftell(turma);
	rewind(turma);
	if(RRN*TAMt >= tamArq)
	{
		printf("A posição requisitada não existe no arquivo de turma!!!\n");
		return NULL;
	}

	fseek(turma,RRN*TAMt,SEEK_SET);
	fgets(string,TAMt+1,turma);

        nome = strtok(string,";");
	curso = strtok(NULL,";");
	RA = strtol(strtok(NULL,";"),NULL,10);

	Aluno* novoAluno = malloc(sizeof(Aluno));
	novoAluno->nome = strcpy(malloc(strlen(nome)+1),nome);
	novoAluno->curso = strcpy(malloc(strlen(curso)+1),curso);
	novoAluno->RA_UNESP = RA;


	return novoAluno;

}

int pesquisar(int RA, Pagina* no, int32_t* RRN)
{
	if(!no)
		return 0;
	int pos;
	if(RA < no->itens[1]->RA)
		pos = 0; //item->RA deve ser inserido na primeira posição
	else
	{

		for(pos = no->nChaves; RA < no->itens[pos]->RA && pos > 1; pos--); //encontra a posição apropriada pro item->RA no nó
		if(no->itens[pos]->RA == RA)
		{
			if(RRN)
				*RRN = no->itens[pos]->RRN;
			return 1;
		}
	}
	if(no->filhos[pos])
		return pesquisar(RA,no->filhos[pos],RRN);
	return 0;
}

//valida se a string digitada não possui nenhum caractere inválido
int validar(char* string)
{
	for(int i = 0; string[i] != '\0'; i++)
		if(string[i] == ';')
		{
			printf("\nCaractere inválido ';' inserido. Por favor realize o cadastro novamente, removendo-o\n");
			return 0;
		}
	return 1;
			
}

idx* criaRegistro(FILE* turma)
{
	idx* novo = malloc(sizeof(idx));
	int32_t RRN;
	int i,j;
	char nome[129];
	char curso[129];
	char string[129];

	Aluno* a = malloc(sizeof(Aluno));

	//Recebe os dados do usuario e escreve eles no arquivo
	//-----------------------------------------------------------------------------
	printf("\n----------------------------\n");
	printf("Nome: ");
	scanf(" %[^\n]s",nome);
	if(validar(nome))
		a->nome = nome;
	else return NULL;

	printf("Curso: ");
	scanf(" %[^\n]s",curso);
	if(validar(curso))
		a->curso = curso;
	else return NULL;

	printf("RA: ");
	scanf("%d", &a->RA_UNESP);
	printf("----------------------------\n\n");

	/* -------------------------
	   Colocar aqui uma pesquisa 
	   pelo RA na árvore B pra
	   garantir que não terá
	   itens duplicados
	   ------------------------- */
	if(pesquisar(a->RA_UNESP,raiz,NULL))
	{
		printf("Esse RA já foi registrado anteriormente!!!\n");
		return NULL;
	}


	//formata a string 
	snprintf(string, 129, "%s;%s;%d;",a->nome,a->curso,a->RA_UNESP);

	//preenche o resto dela para que esta ocupe sempre 128 bytes
	for(j = strlen(string); j < 127; j++)
		string[j] = '#';
	string[127] = ';';
	string[128] = '\0'; //terminador nulo



	fseek(turma,0L,SEEK_END);
	RRN = ftell(turma) / 128;
	fputs(string,turma);
	novo->RRN = RRN;
	novo->RA = a->RA_UNESP;

	return novo;
}

idx* criaRegistroRedux(FILE* turma, char* nome, int RA,  char* curso)
{
	if(pesquisar(RA,raiz,NULL))
	{
		printf("Esse RA já foi registrado anteriormente!!!\n");
		return NULL;
	}
	int RRN;
	int j;
	idx* novo = malloc(sizeof(idx));
	char string[129];
	if(!validar(nome))
		return NULL;
	if(!validar(curso))
		return NULL;

	snprintf(string, 129, "%s;%s;%d;",nome,curso,RA);
	for(j = strlen(string); j < 127; j++)
		string[j] = '#';
	string[127] = ';';
	string[128] = '\0'; //terminador nulo

	fseek(turma,0L,SEEK_END);
	RRN = ftell(turma) / 128;
	fputs(string,turma);
	novo->RRN = RRN;
	novo->RA = RA;

	return novo;
}


//cria um novo nó (pagina) já sabido o primeiro item->RA
//começa na posição 1, deixando a 0 como reserva pra overflow
//(descobrir pq o filhos[0] aponta pra raiz)
Pagina* criarPagina(idx* item, Pagina* filho)
{
	Pagina* novaPagina = malloc(sizeof(Pagina));
	novaPagina->itens[1] = item;
	novaPagina->nChaves = 1;
	novaPagina->filhos[0] = raiz; //raiz atual agora é filha esquerda da nova raiz
	novaPagina->filhos[1] = filho;
	return novaPagina;
}

//inserir valor na pagina já sabia a página e a posição q será ocupada
void inserirNaPagina(idx* item, int pos, Pagina* no, Pagina* filho)
{
	int j = no->nChaves;
	//move todos os elementos até o da pos para a direita a fim de inserir o novo item->RA na nova posição vaga
	//aparentemente, se pos é 1, o item->RA é inserido em itens[2];
	while(j > pos)
	{
		no->itens[j+1] = no->itens[j];
		no->filhos[j+1] = no->filhos[j];
		j--;
	}
	no->itens[j+1] = item;
	no->filhos[j+1] = filho; //filho à esquerda do item->RA inserido
	no->nChaves++;
}

//(item->RA que devia ser inserido antes do overflow, endereço dele, pos a ser inserida, no no qual será feita a inserção, filho(esquerdo?) dele, novo nó gerado a partir do split( o qual substituirá o filho esquerdo
void split(idx* item, idx** pitem, int pos, Pagina* no, Pagina* filho, Pagina** novaPagina)
{
	int median, j;
	//verificação feita a fim de deixar nos nós gerados balanceados
	if(pos > MIN)
		median = MIN + 1;
	else
		median = MIN;

	*novaPagina = malloc(sizeof(Pagina)); //filho da "direita"
	j = median + 1;
	while(j <= MAX) //preenchendo o filho "direito"
	{
		(*novaPagina)->itens[j-median] = no->itens[j];
		(*novaPagina)->filhos[j-median] = no->filhos[j];
		j++;
	}
	//setta o número de itens em cada um dos nós resultantes
	no->nChaves = median;
	(*novaPagina)->nChaves = MAX - median;

	//enfim insere o item->RA que causou o overflow no nó apropriado
	if(pos <= MIN)
		inserirNaPagina(item,pos,no,filho);
	else
		inserirNaPagina(item, pos-median, *novaPagina, filho);
	*pitem = no->itens[no->nChaves]; //pitem assume o valor do último valor do no atual. presumidamente, esse é o valor que será promovido.
	(*novaPagina)->filhos[0] = no->filhos[no->nChaves]; //primeiro filhos do novo nó ("direito") recebe o nó esquerdo do item->RA que será promovido
	no->nChaves--;
}
int inserirItem(idx* item, idx** pitem, Pagina* no, Pagina** filho)
{
	int pos;
	if(!no) //criterio de parada para quando chegar na folha da árvore. setta o conteúdo de pitem como o valor que deve ser inserido no nó acima deste na pilha de recursão.
	{
		*pitem = item;
		*filho = NULL; //o nó a ser criado não vai ter filhos (provavelmente vai sr a nova raiz)
		return 1;
	}

	//encontrando posição:
	//----------------------------------------------------------------------------
	if(item->RA < no->itens[1]->RA)
		pos = 0; //item->RA deve ser inserido na primeira posição
	else
	{
		for(pos = no->nChaves; item->RA < no->itens[pos]->RA && pos > 1; pos--); //encontra a posição apropriada pro item->RA no nó
		if(item->RA == no->itens[pos]->RA)
		{
			printf("Duplicates not allowed!!\n");
			return 0;
		}
	}
	//----------------------------------------------------------------------------
	if(inserirItem(item, pitem, no->filhos[pos], filho)) //tenta ir para o filho esquerdo da posição na qual o novo item->RA teoricamente ocuparia
		//só vai ser verdadeiro se chegar em um nó folha (ou seja, tentar ir para um filho mas este não existir)
	{
		if(no->nChaves < MAX) //nó atual não está em overflow
			inserirNaPagina(*pitem, pos, no, *filho);
		else //overflow
		{
			split(*pitem, pitem, pos, no, *filho, filho);
			return 1;
		}
	}
	return 0;


}
void inserir(idx* item)
{
	int flag; //flag que indica ***
	idx* i;
	//i se refere a ***
	Pagina* filho; //inicialmente não aponta pra nada
	flag = inserirItem(item, &i, raiz, &filho); //pelo visto, filho vai ser alterado
	if(flag) //caso será criada uma nova raíz
		raiz = criarPagina(i, filho); //i é o valor a ser inserido na raiz

}


void imprimir(Pagina* no)
{
	int i;
	if(no)
	{
		for(i = 0; i < no->nChaves; i++)
		{
			imprimir(no->filhos[i]);
			printf("RA: %d\nRRN: %d\n\n",no->itens[i+1]->RA,no->itens[i+1]->RRN);
		}
		imprimir(no->filhos[i]);
	}
}

void imprimirArvore() //wrapper pra função imprimir(raiz);
{
	imprimir(raiz);
	return;
}

void pesquisarRA(int RA)
{
	if(pesquisar(RA,raiz,NULL))
		printf("RA encontrado!!!\n");
	else
		printf("RA NÃO encontrado!!!\n");
}

Aluno* pesquisarAluno(int RA, FILE* turma)
{
	int RRN;

	pesquisar(RA,raiz,&RRN);
	printf("RRN encontrado: %d\n",RRN);
	return lerRegistro(RRN, turma);
}
