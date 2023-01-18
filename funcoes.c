#include <stdio.h>
#include <stdlib.h>
#include "B-Tree.h"
#include <string.h>
#include <limits.h>
#include <stdint.h>

Pagina* raiz;
FILE* indices;
FILE* turma;

//Funções com arquivos:
//-------------------------------------
void abrirArquivos()
{
	turma = fopen("turma.dat","r+") ? : fopen("turma.dat","w+");

	//abre o arquivo com o modo certo e carrega a raiz apropriada
	indices = fopen("ibtree.idx","r+");
	if(!indices)
	{
		indices = fopen("ibtree.idx","w+");
		fseek(indices,0L,SEEK_SET);
		printBytes(NOT_SET);
		raiz = NULL;
	}
	else
	{
		fseek(indices,0L,SEEK_SET);
		if(arquivoVazio(indices))
			raiz = NULL;
		else
		{
			fseek(indices,0L,SEEK_SET);
			int RRN = readBytes();
			raiz = carregarPagina(RRN);
			if(raiz)
				raiz->RRN = RRN;
		}
	}

	if(turma == NULL)
	{
		printf("Erro ao abrir o arquivo de turma!!");
		exit(1);
	}
	if(indices == NULL)
	{
		printf("Erro ao abrir o arquivo de indices!!");
		exit(1);
	}
}

void fecharArquivos()
{
	fclose(indices);
	fclose(turma);
}

//para previnir perda de dados caso o programa seja interrompido bruscamente
void refreshArquivos() 
{
	fflush(indices);
	fflush(turma);
}

int arquivoVazio(FILE* arq)
{
	int vazio;
	long posAnt = ftell(arq);
	fseek(arq,0L,SEEK_END);
	if(ftell(arq) > 0)
		vazio = 0;
	else vazio = 1;

	fseek(arq,posAnt,SEEK_SET);
	return vazio;
}

void novaPaginaArquivo(Pagina* no)
{
	long posAnt = ftell(indices);

	fseek(indices,0L,SEEK_END);
	unsigned int RRN = (ftell(indices) - 4) / TAMi;

	printBytes(no->filhos[0]);

	for(int i = 1; i <= MAX; i++)
	{
		if(no->itens[i] != NULL)
		{
			printBytes(no->itens[i]->RA);
			printBytes(no->itens[i]->RRN);
		}
		else
		{
			printBytes(NOT_SET);
			printBytes(NOT_SET);
		}

		printBytes(no->filhos[i]);
	}

	no->RRN = RRN;
	fseek(indices,posAnt,SEEK_SET);
}

void atualizarPagina(Pagina* no)
{
	long posAnt = ftell(indices);

	unsigned int RRN = no->RRN;
	long offset = RRN * TAMi + 4; //4 bytes do cabeçalho
	fseek(indices,offset,SEEK_SET);

	printBytes(no->filhos[0]);

	for(int i = 1; i <= MAX; i++)
	{
		if(no->itens[i])
		{
			printBytes(no->itens[i]->RA);
			printBytes(no->itens[i]->RRN);
		}
		else
		{
			printBytes(NOT_SET);
			printBytes(NOT_SET);
		}

		printBytes(no->filhos[i]);
	}

	fseek(indices,posAnt,SEEK_SET);
}

//vai pro começo do arquivo e imprime RRN da raiz no cabeçalho
void setRaiz(unsigned int RRN)
{
	long posAnt = ftell(indices);
	fseek(indices,0L,SEEK_SET);
	printBytes(RRN);
	fseek(indices,posAnt,SEEK_SET);
}

Pagina* carregarPagina(unsigned int RRN)
{
	if(RRN == NOT_SET)
		return NULL;

	long posAnt = ftell(indices);

	long offset = RRN * TAMi + 4;
	fseek(indices, offset, SEEK_SET);

	Pagina* nova = malloc(sizeof(Pagina));
	idx* aux = NULL;
	nova->nChaves = 0;

	nova->filhos[0] = readBytes();
	for(int i = 1; i <= MAX; i++)
	{
		aux = malloc(sizeof(idx));
		aux->RA = readBytes();
		aux->RRN = readBytes();
		nova->itens[i] = aux;

		nova->filhos[i] = readBytes();

		if(nova->itens[i]->RA != NOT_SET)
			nova->nChaves++;
	}

	nova->RRN = RRN;
	fseek(indices,posAnt,SEEK_SET);
	return nova;
}
//-------------------------------------



//Operações com bytes:
//-------------------------------------

//imprime no arquivo de indices um int convertido em 4 caracteres contendo seus 4 bytes
void printBytes(int n)
{
	fputc(n >> 24,indices);
	fputc(n >> 16,indices);
	fputc(n >> 8,indices);
	fputc(n,indices);
}

unsigned int readBytes()
{
	return (fgetc(indices) << 24) + (fgetc(indices) << 16) + (fgetc(indices) << 8) + fgetc(indices);
}

//-------------------------------------


//Funções da Árvore-B
//-------------------------------------
Pagina* criarPagina(idx* item, unsigned int filho)
{
	Pagina* novaPagina = malloc(sizeof(Pagina));
	novaPagina->itens[1] = item;

	//inicializando elementos
	for(int i = 0; i <= MAX; i++)
		novaPagina->filhos[i] = NOT_SET;	
	for(int i = 2; i <= MAX; i++)
		novaPagina->itens[i] = NULL;

	novaPagina->nChaves = 1;
	novaPagina->filhos[0] = raiz == NULL ? NOT_SET : raiz->RRN;
	novaPagina->filhos[1] = filho;
	return novaPagina;
}

void inserir(idx* item)
{
	if(!item)
	{
		printf("Item adicionado é inválido\n");
		return;
	}
	if(raiz && raiz->RRN == UINT_MAX - 1)
	{
		printf("A árvore está cheia!!!\nNão podem ser adicionados mais itens"); 
		return;
	}
	int novaPagina;
	idx* elemento; //elemento que será, de fato, inserido
	unsigned int filho; //RRN do filho direito do elemento a ser inserido
	novaPagina = inserirItem(item, &elemento, raiz, &filho);
	if(novaPagina)
	{
		raiz = criarPagina(elemento,filho);
		novaPaginaArquivo(raiz);
		setRaiz(raiz->RRN);
	}

}

int inserirItem(idx* item, idx** pelem, Pagina* noAtual, unsigned int* filho)
{
	if(noAtual == NULL)
	{
		*pelem = item;
		*filho = NOT_SET;

		return 1;
	}

	//encontrando posição:
	//----------------------------------------------------------------------------
	int pos;
	if(item->RA < noAtual->itens[1]->RA)
		pos = 0; //item->RA deve ser inserido na primeira posição
	else
		for(pos = noAtual->nChaves; item->RA < noAtual->itens[pos]->RA && pos > 1; pos--); //encontra a posição apropriada pro item->RA no nó
	//----------------------------------------------------------------------------

	Pagina* proxima = carregarPagina(noAtual->filhos[pos]);
	if(inserirItem(item,pelem,proxima,filho))
	{
		if(noAtual->nChaves < MAX)
			inserirNaPagina(*pelem, pos, noAtual, *filho);
		else //overflow
		{
			split(*pelem, pelem, pos, noAtual, *filho, filho);
			free(proxima);
			return 1;
		}
		free(proxima);

	}
	return 0;


}

void inserirNaPagina(idx* item, int pos, Pagina* no, unsigned int filho)
{
	int i = no->nChaves;
	while(i > pos)
	{
		no->itens[i+1] = no->itens[i];
		no->filhos[i+1] = no->filhos[i];
		i--;
	}
	no->itens[i+1] = item;
	no->filhos[i+1] = filho;
	no->nChaves++;

	atualizarPagina(no);
}

//pagDir tem q assumir o valor do RRN da novaPagina
void split(idx* item, idx** pitem, int pos, Pagina* no, unsigned int filhoRRN, unsigned int* pagDir)
{
	int mediana, i;
	if(pos > MIN)
		mediana = MIN + 1;
	else
		mediana = MIN;
	Pagina* novaPagina = malloc(sizeof(Pagina));
	for(int j = 0; j <= MAX; j++)
		novaPagina->filhos[j] = NOT_SET;	
	i = mediana + 1;

	while(i <= MAX) //preenchendo o filho "direito"
	{
		novaPagina->itens[i-mediana] = no->itens[i];
		novaPagina->filhos[i-mediana] = no->filhos[i];
		no->itens[i] = NULL; //"limpando" os itens que foram movidos
		no->filhos[i] = NOT_SET;
		i++;
	}

	//atualiza o número de itens em cada um dos nós resultantes
	no->nChaves = mediana;
	novaPagina->nChaves = MAX - mediana;

	//registra a pagina criada no arquivo de indices
	novaPaginaArquivo(novaPagina);

	if(pos <= MIN)
		inserirNaPagina(item,pos,no,filhoRRN);
	else
		inserirNaPagina(item, pos - mediana, novaPagina, filhoRRN);

	*pitem = no->itens[no->nChaves]; //pitem aponta para o item que será promovido
	no->itens[no->nChaves] = NULL;
	novaPagina->filhos[0] = no->filhos[no->nChaves]; //primeiro filho do nó "direito" recebe o nó esquerdo do item que será promovido
	no->nChaves--;

	atualizarPagina(novaPagina);
	atualizarPagina(no);
	*pagDir = novaPagina->RRN;
	free(novaPagina);
}

int pesquisar(unsigned int RA, Pagina* no, unsigned int* RRN, int* nSeeks)
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
	if(no->filhos[pos] != NOT_SET)
	{
		Pagina* proxima = carregarPagina(no->filhos[pos]);
		if(nSeeks)
			*nSeeks = *nSeeks + 1;
		int res = pesquisar(RA,proxima,RRN,nSeeks);
		free(proxima);
		return res;
	}
	return 0;
}

void imprimirArvore(Pagina* no)
{
	if(no)
	{
		int i;
		Pagina* proxima;
		for(i = 0; i < no->nChaves; i++)
		{
			proxima = carregarPagina(no->filhos[i]);
			imprimirArvore(proxima);
			free(proxima);
			printf("%u ",no->itens[i+1]->RA);
		}
		proxima = carregarPagina(no->filhos[i]);
		imprimirArvore(proxima);
		free(proxima);
	}
}
void printArvore()
{
	if(!raiz)
		printf("A árvore está vazia!!\n");
	else
		imprimirArvore(raiz);
}
//-------------------------------------



//Funções para registros de aluno
//-------------------------------------

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

idx* criaRegistro()
{
	idx* novo = malloc(sizeof(idx));
	uint32_t RRN;
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
	if(a->RA_UNESP == UINT_MAX)
	{
		printf("Este RA não está acima do limite!!!\nPor favor tente outro\n");
		return NULL;
	}
	printf("----------------------------\n\n");

	if(pesquisar(a->RA_UNESP,raiz,NULL,NULL))
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

Aluno* lerRegistro(int RRN)
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


void pesquisarAluno(unsigned int RA)
{
	unsigned int RRN;
	int nSeeks = 0;
	if(pesquisar(RA,raiz,&RRN,&nSeeks))
	{
		printf("\n----------------------------\n");
		printf("Aluno encontrado!\n");
		printAluno(lerRegistro(RRN));
		printf("nº de seeks: %d\n",nSeeks);
		printf("----------------------------\n");
	}
	else
		printf("Aluno NÃO encontrado!!!\n");
}

void printAluno(Aluno* a)
{
	if(a)
		printf("Nome: %s | RA: %u\nCurso: %s\n",a->nome,a->RA_UNESP,a->curso);
	else printf("Algo deu bem errado!!\n");
}

//-------------------------------------




//FUNÇÕES PRA DEBUG
//---------------------------------
idx* criaRegistroRedux(char* nome, int RA,  char* curso)
{
	if(pesquisar(RA,raiz,NULL,NULL))
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

void printPagina(Pagina* pag)
{
	if(!pag)
	{
		printf("\n\npagina não existe!!!\n");
		return;
	}
	if(pag->filhos[0] != NOT_SET)
		printf("%u",pag->filhos[0]);
	else printf("\?\?\?\?");
	for(int i = 1; i <= MAX; i++)
	{
		if(pag->itens[i])
			printf("<%u,%u>",pag->itens[i]->RA,pag->itens[i]->RRN);
		else printf("<\?\?\?\?,\?\?\?\?>");
		if(pag->filhos[i] != NOT_SET)
			printf("%u",pag->filhos[i]);
		else printf("\?\?\?\?");
	}


}
