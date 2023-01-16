#define MAX 4
typedef struct aluno
{
	char* nome;
	char* curso;
	int RA_UNESP;
} Aluno;

typedef struct indice
{
	int RA;
	int32_t RRN;
} idx;

typedef struct BTPage
{
	int nChaves;
	idx* itens[MAX+1];
	struct BTPage* filhos[MAX+1];
	int32_t RRN;
} Pagina;
