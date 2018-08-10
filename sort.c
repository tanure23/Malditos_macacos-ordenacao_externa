// =----------------------------------------------------------=
// =                           AVISO:                         =
// =                   MODIFIQUE ESTE ARQUIVO!                =
// = Aqui, você deve implementar, obrigatoriamente, essas     =
// = duas funções:                                            =
// =     - `a_menor_que_b`: deve retornar quando uma string   =
// =        é menor que a outra (ambos com o mesmo tamanho);  =
// =     - `external_sort`: aqui, você deverá implementar o   =
// =        seu trabalho. Ordene os registros em `input_file` =
// =        e escreva-os em `output_file`, usando somente     =
// =        `memory` KB de memória.                           =
// =                                                          =
// = Instruções:                                              =
// =     - use a função `mathias_malloc` ao invés de          =
// =       `malloc`;                                          =
// =     - use a função `mathias_free` ao invés de `free`;    =
// =     - lembre-se que você tem somente `memory` KB de      =
// =       memória;                                           =
// =     - note que a sua implementação de `a_menor_que_b`    =
// =       afetará o resultado da verificação da resposta.    =
// =                                                          =
// = Dicas:                                                   =
// =     - você pode usar a sua função `a_menor_que_b` em     =
// =       sua implementação da função `external_sort`;       =
// =                                                          =
// =                      BOM TRABALHO!                       =
// =----------------------------------------------------------=

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mem.h"

//////////////////////////////////////////////////////////////////////////////////////////
//								TP1 - Damn Monkeys!										//
//////////////////////////////////////////////////////////////////////////////////////////


//this is a utility function used by qsort, as it requires (const void*) types for...
//... its comparisons
//IMPORTANT!!
//It is rightful to AnyExample and I do not own any of its properties.
int compare(const void *a, const void *b){ 
    const char **pa = (const char **)a;
    const char **pb = (const char **)b;
    return strcmp(*pa, *pb); 
} 

int max(int a, int b){
	return a>b ? a:b;
}

//////////////////////////////////////////////////////////////////////////////////////////
// Parâmetros:																			//
//     - a: uma cadeia de caracteres de tamanho `len`;									//
//     - b: uma cadeia de caracteres de tamanho `len`;									//
//     - len: o tamanho (quantidade de bytes) de `a` e `b`.								//
// Essa função deve retornar 1, se `a` é menor que `b` (`a` deve vir antes de `b` no    //
// arquivo), e 0 caso contrário.														//
// Ela também é usada para verificação da ordenação! Então, se você implementá-la		//
// erroneamente, a verificação também estará errada! 									//
//////////////////////////////////////////////////////////////////////////////////////////

int a_menor_que_b(char* a, char* b, int len) {
	int i=0;
	while(i < len){
		if(a[i] != b[i]){ //if chars[i] are different, there must be a smaller one
			if(a[i] < b[i]) return 1;
			else return 0;
		}
		//we only continue traversing the strings if first (and subsequent)...
		//...chars are the same
		i++;
	}
	//we reached i==len and there's no smaller string, so both are equal
	//That means a is not less than b
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
// Parâmetros:																			//
//     - input_file: o nome do arquivo com os registros de entrada;						//
//     - output_file: o nome do arquivo com os registros de entrada ordenados;			//
//     - memory: o limite de memória a ser usado.										//
// Essa função deverá ler todos registros do arquivo `input_file`, ordená-los e 		//
// escrevê-los em `output_file`, usando memória menor ou igual a `memory`. Lembre-se 	//
// de fechar todos os arquivos que você usar por aqui!!									//
//////////////////////////////////////////////////////////////////////////////////////////

void external_sort(const char* input_file, const char* output_file, unsigned int memory) {
	FILE *in;
	FILE *out;
	FILE *f0;
	FILE *f1;
	FILE *f2;
	FILE *write_one;
	int len=0, i=0, j=0, k=0, n_strings=0, string_idx = 0;
	int ok1=0, ok2=0;
	int last=0;
	int m=0;

	in = fopen(input_file, "r");
	if(!in){
		fprintf(stderr, "***ERROR! Failure to open INPUT file***\n");
		//return -1; //treating failure to open
	}

	/////
	//we already read input_file's first line
	if(!fscanf(in, "%d%*c", &len)){
		fprintf(stderr, "***ERROR! Failure to read strings' size***\n");
		fclose(in);
		//return -2; //treating fscanf reading error
	}
	/////

	out = fopen(output_file, "w+");
	fprintf(out, "%d\n", len);
	if(!out){
		fprintf(stderr, "***ERROR! Failure to open OUTPUT file***\n");
		//return -1; //treating failure to open
	}

	char *a = mathias_malloc(len+1); //len+1 because of "\0" terminating char
	char *b = mathias_malloc(len+1);

	//as we name our auxiliary files "fi.txt" for every i in [0, k]...
	//... (k being total number of files created), we need 5+x digits, ...
	//... x being the number of digits of k. This allows k to be up to order of 10^7 + "\0", ...
	//... which is probably high enough for most test cases
	char *str = malloc(13*sizeof(char)); 

	float mem = (float)memory*1024; //parameter memory is an int for KB, which is 1024 B
	mem -= 2*(len+1)*sizeof(char); //as we already allocated 2 strings, a and b

	m = mem/(((len+1)*sizeof(char)) + sizeof(char*)); //we divide mem by the ammount of memory a single string needs
	// if(mem >= 2) m = max(m, 2); //as we're garanteed to be allowed to have at least 2 strings in primary memory
	n_strings=0;

	//now we allocate a string vector that is going to hold our files' strings
	char **vec_strings = mathias_malloc(m*sizeof(char*));
	for(i=0; i<m; i++){
		vec_strings[i] = mathias_malloc((len + 1)*sizeof(char));
	}

	////////////////////////////////////////////////////////////////////////////////
	//////////////////////////// Separation by blocks //////////////////////////////
	i=0;
	string_idx = 0;
	while(fscanf(in, "%[^\n]%*c", a)!=EOF){
		strcpy(vec_strings[string_idx], a);
		n_strings++;
		string_idx++;

		if(string_idx == m){
			sprintf(str, "f%d.txt", i);
			last = i;
			i++;
			f0 = fopen(str, "w+");

			//sorting file's strings
			qsort(vec_strings, m, sizeof(char*), compare);
			for(j=0; j<m; j++){
				fprintf(f0, "%s\n", vec_strings[j]);
			}

			fclose(f0);
			
			for(k=0; k<m; k++){
				strcpy(vec_strings[k], ""); //reset vec_strings
			}
			
			string_idx = 0;
		}

	}
	
	if(n_strings==1) {
		fprintf(out, "%s\n", a);
	}

	//if there's still a number of strings left that is less than the maximum a file can hold...
	//... (which is the maximum our memory can hold), we create another file with those and sort them
	// else if(string_idx!=0){ 
	// else if(string_idx > 0 || m == n_strings){ 
	else if(string_idx > 0){ 
		sprintf(str, "f%d.txt", i);
		f0 = fopen(str, "w+");

		//sorting file's strings
		qsort(vec_strings, string_idx, sizeof(char*), compare);
		for(j=0; j<string_idx; j++){
			fprintf(f0, "%s\n", vec_strings[j]);
		}

		fclose(f0);
		last = i;
	}

	//////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////// Merge ////////////////////////////////////////////
	k = 0;

	if(last == -1){ //this means we can sort all strings internally because memory limit is high enough
		for(j=0; j<string_idx; j++){
			fprintf(out, "%s\n", vec_strings[j]);
		}
	}
	else if(n_strings>1){
		//last == k signals we merged the last two files into another one, which...
		//... means we now have only one file that contains all strings sorted, so our merge is completed
		while(last != k){ 
			sprintf(str, "f%d.txt", k);
			f0 = fopen(str, "r");

			sprintf(str, "f%d.txt", k+1);
			f1 = fopen(str, "r");

			sprintf(str, "f%d.txt", last+1);
			write_one = fopen(str, "w+"); //this opens a new file in which we write our current merge phase

			//we use ok1 and ok2, both int, to easily keep control of reaching EOF and knowing when a file...
			//... can or can not be read anymore
			ok1 = fscanf(f0, "%[^\n]%*c", a);
			ok2 = fscanf(f1, "%[^\n]%*c", b);

			while(ok1>0 || ok2>0){ //this means there is still something to be read, despite of where it is
				if(ok1>0 && ok2>0){ // this means there is something to be read on both files
					if(a_menor_que_b(a, b, len)){
						fprintf(write_one, "%s\n", a);
						ok1 = fscanf(f0, "%[^\n]%*c", a);
					}
					else{
						fprintf(write_one, "%s\n", b);
						ok2 = fscanf(f1, "%[^\n]%*c", b);
					}
				}

				else if(ok1<=0 && ok2<=0){ //there is nothing to be read on both files
					break;
				}

				//file2 reached EOF, so we simply write the rest of file1, because it is sorted and every...
				//... element there is more than those we wrote previously
				else if(ok1>0 && ok2<=0){ 
					fprintf(write_one, "%s\n", a);
					ok1 = fscanf(f0, "%[^\n]%*c", a);
					while(ok1>0){
						fprintf(write_one, "%s\n", a);
						ok1 = fscanf(f0, "%[^\n]%*c", a);
					}
					break;
				}

				//this is the same as the above else if(), only that file1 has ended and file2 has not
				else if(ok1<=0 && ok2>0){
					fprintf(write_one, "%s\n", b);
					ok2 = fscanf(f1, "%[^\n]%*c", b);
					while(ok2>0){
						fprintf(write_one, "%s\n", b);
						ok2 = fscanf(f1, "%[^\n]%*c", b);
					}
					break;
				}
				
			}

			//now we update values
			k+=2; //because we work with 2 files at a time
			last++; //because we name our files sequencially, current last is (previous last)+1

			fclose(f0);
			fclose(f1);
			fclose(write_one);
		}

		//////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////


		//////////////////////////////////////////////////////////////////////////////////////
		////////////////////////////// Copying to output_file ////////////////////////////////
		sprintf(str, "f%d.txt", last);
		f2 = fopen(str, "r");
		ok1 = fscanf(f2, "%[^\n]%*c", a);
		while(ok1>0){
			fprintf(out, "%s\n", a);
			ok1 = fscanf(f2, "%[^\n]%*c", a);
		}

	}
	//////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////// Closing and freeing //////////////////////////////////
	if(n_strings>1 && last > 0 )	fclose(f2);
	fclose(out);
	fclose(in);
	free(str);
	mathias_free(a);
	mathias_free(b);
	for(i=0; i<m; i++){
		mathias_free(vec_strings[i]);
	}
	mathias_free(vec_strings);

	//////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////
	
}
