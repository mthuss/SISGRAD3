#include <stdio.h>
#include "B-Tree.h"

int main()
{
	int RA, opt;
	abrirArquivos();
	printf("\t\t\nBem-vindo ao SISGRAD3.");
	do{
		printf("\t\nO que deseja fazer?\n1 - Gravar Aluno.\n2 - Buscar Aluno.\n3 - Ver todos os RAs\n4 - Fechar.\n\nOpção: ");
		scanf("%d", &opt);

		switch(opt){
			case 1:
				idx* novo = criaRegistro();
				if(novo)
					inserir(novo);
				break;

			case 2: 
				printf("\nQual o numero de RA a procurar?\nRA: ");
				scanf("%d", &RA);
				pesquisarAluno(RA);
				break;

			case 3:
				printf("\n\nLista RAs:\n");
				printArvore();
				printf("\n\n");
				break;

			case 4:
				break;

			default:
				printf("\nNumero invalido!\n");
				break;  
		}
		refreshArquivos();
	}while(opt != 4);

	fecharArquivos();

	return 0;
}
