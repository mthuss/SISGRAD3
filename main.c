#include <stdio.h>
#include <stdlib.h>
#include "funcoes.c"
#define MAX 4 //num max chaves (ou seja, num de ponteiros por pagina = 5)


int main()
{
	FILE *turma, *indices;
	abrirArquivos(&turma, &indices);
	inserir(criaRegistroRedux(turma,"mthus",211044067,"comp"));
	inserir(criaRegistroRedux(turma,"lucas",211044536,"comp"));
	inserir(criaRegistroRedux(turma,"gaki",211044631,"comp"));
	inserir(criaRegistroRedux(turma,"bruno",211044569,"comp"));
	inserir(criaRegistroRedux(turma,"katana",211044606,"comp"));
	Aluno* a = pesquisarAluno(211044631, turma);
	printf("Nome: %s\nCurso: %s\nRA: %d\n\n",a->nome,a->curso,a->RA_UNESP);


//	imprimirArvore();
}
