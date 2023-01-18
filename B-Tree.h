#define MAX 4 //maximo de ITENS na árvore
#define MIN 2
#define TAMt 128 //tamanho de cada registro do arquivo de turma
#define TAMi 52 //tamanho de cada registro do arquivo de indices (presumindo que todos os dados têm 4 bytes cada
#define NOT_SET UINT_MAX //os dados desconhecidos vão ser salvos no arquivo como UINT_MAX

typedef struct aluno
{
	char* nome;
	char* curso;
	unsigned int RA_UNESP;
} Aluno;

typedef struct indice
{
	unsigned int RA;
	unsigned int RRN;
} idx;

typedef struct BTPage
{
	unsigned int nChaves;
	idx* itens[MAX+1];
	unsigned int filhos[MAX+1]; //RRN dos filhos
	unsigned int RRN; //RRN desta pagina no arquivo de indice
} Pagina;

//Funções:
//----------------------------------------------------
void abrirArquivos();
void fecharArquivos();
void refreshArquivos();
int arquivoVazio(FILE* arq);
void novaPaginaArquivo(Pagina* no);
void atualizarPagina(Pagina* no);
void setRaiz(unsigned int RRN);
Pagina* carregarPagina(unsigned int RRN);


//Operações com bytes:
void printBytes(int n);
unsigned int readBytes();

//Funções da Árvore-B:
Pagina* criarPagina(idx* item, unsigned int filho);
void inserir(idx* item);
int inserirItem(idx* item, idx** pelem, Pagina* noAtual, unsigned int* filho);
void inserirNaPagina(idx* item, int pos, Pagina* no, unsigned int filho);
void split(idx* item, idx** pitem, int pos, Pagina* no, unsigned int filhoRRN, unsigned int* pagDir);
int pesquisar(unsigned int RA, Pagina* no, unsigned int* RRN, int* nSeeks);
void imprimirArvore(Pagina* no);
void printArvore();

//Funções para registros de aluno:
int validar(char* string);
idx* criaRegistro();
Aluno* lerRegistro(int RRN);
void pesquisarAluno(unsigned int RA);
void printAluno(Aluno* a);

//Debug:
idx* criaRegistroRedux(char* nome, int RA,  char* curso);
void printPagina(Pagina* pag);
