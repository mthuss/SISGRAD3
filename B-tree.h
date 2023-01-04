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
	int chaves[MAX];
	int32_t filhos[MAX+1];
} Pagina;
