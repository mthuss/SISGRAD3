#include <stdio.h>
#include <stdlib.h>
#include "funcoes.c"
#define MAX 4 //num max chaves (ou seja, num de ponteiros por pagina = 5)


int main()
{
	abrirArquivos();
	inserir(criaRegistroRedux("mthus",211044067,"comp"));
	inserir(criaRegistroRedux("lucas",211044536,"comp"));
	inserir(criaRegistroRedux("gaki",211044631,"comp"));
	inserir(criaRegistroRedux("bruno",211044569,"comp"));
	inserir(criaRegistroRedux("katana",211044606,"comp"));
	//Aluno* a = pesquisarAluno(211044631);
	//printf("Nome: %s\nCurso: %s\nRA: %d\n\n",a->nome,a->curso,a->RA_UNESP);
	printPaginateste();
	imprimirPaginaArqTeste();

	fecharArquivos();


//	imprimirArvore();
}
