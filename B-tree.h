#define MAX 4
#include <stdint.h>
typedef struct aluno
{
	char* nome;
	char* curso;
	unsigned int RA_UNESP;
} Aluno;

typedef struct indice
{
	unsigned int RA;
	uint32_t RRN;
} idx;

typedef struct BTPage
{
	unsigned int nChaves;
	idx* itens[MAX+1];
	struct BTPage* filhos[MAX+1];
	uint32_t RRN;
} Pagina;
