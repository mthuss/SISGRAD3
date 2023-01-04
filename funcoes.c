#include <stdio.h>
#include <stdlib.h>
#include "B-tree.h"
#include <string.h>
void abrirArquivos(FILE** turma, FILE** indices)
{
	//note q eu não faço IDEIA de se uma declaração dessas ta 100% certa mas fico bonitinha e funciona ent vai ficar até quebrar algo
	*turma = fopen("turma.dat","r+") ? : fopen("turma.dat","w");
	*indices = fopen("ibtree.idx","r+") ? : fopen("ibtree.idx","w");

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

Aluno* criaRegistro(FILE* turma)
{
	int i,j;
	char nome[129];
	char curso[129];
	char* string = malloc(129);

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


	//if(!cadastrado(indices,a.RA_UNESP)) //garante que o RA digitado ainda não foi cadastrado
	//{
		//formata a string 
		snprintf(string, 129, "%s;%s;%d;",a->nome,a->curso,a->RA_UNESP);

		//preenche o resto dela para que esta ocupe sempre 128 bytes
		for(j = strlen(string); j < 127; j++)
			string[j] = '#';
		string[127] = ';';
		string[128] = '\0'; //terminador nulo


		fputs(string,turma);
		return a;
//		fseek(fp, 0L, SEEK_END); //vai ao fim do arquivo
//					 //escreve, de fato, a string no arquivo
//		fprintf(fp,"%s",string);

	//	tamanho++;

//		indexar(&indices,a.RA_UNESP,(tamanho-1)*128);
//
//		fseek(index, 0L, SEEK_END); //vai ao fim do arquivo
//					    //escreve o arquivo de indice
//		fprintf(index,"%d %d\n",a.RA_UNESP,(tamanho-1)*128);
	//}
}
