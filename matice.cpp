#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define ELEM(mat, i, j) ((mat).elem[(i) * (mat).cols + (j)])

typedef struct {
	unsigned int rows;
	unsigned int cols;
	float *elem;
} MAT;


MAT* mat_create_with_type(unsigned int rows, unsigned int cols) {

	// alokacia pamati pre strukturu MAT
	MAT* matica = (MAT *)malloc(sizeof(MAT));

	// ak alokacia zlyhala, vratime NULL
	if (matica == NULL)
		return NULL;

	// inicializacia hodnot "rows" a "cols" v strukture MAT
	matica->rows = rows;
	matica->cols = cols;

	// alokacia pamate pre pole prvkov matice
	unsigned int pole_prvkov = rows*cols;
	matica->elem = (float*)malloc(pole_prvkov*sizeof(float));

	// ak alokacia zlyhala, uvolnime uz alokovanu pamat a vratime NULL
	if (matica->elem==NULL) {
		free(matica);
		return NULL;
	}

	// vraciame adresu vytvorenej struktury MAT
	return matica;
}


void mat_destroy(MAT *mat) {

	// uvolnenie pamate alokovanej pre pole prvkov matice
	free(mat->elem);
	// uvolnenie pamate alokovanej pre samotnu strukturu MAT
	free(mat);
}


char mat_save(MAT *mat, char *filename) {

	int i;

	// otvorenie suboru na zapis (wb- binarne)
	FILE* file = fopen(filename, "wb");

	// osetrenie, ci bol subor otvoreny
	if (file==NULL)
		return 0;

	// zapisanie specifikacie matice, resp. M1
	if (fprintf(file, "M 1\n") < 0) {
		fclose(file);
		return 0;
	}

	// zapis poctu riadkov a stlpcov
	if (fprintf(file, "%u\n%u\n", mat->rows, mat->cols) < 0) {
		fclose(file);
		return 0;
	}

	// zapis jednotlivych prvkov
	unsigned int prvky_matice = mat->rows * mat->cols;
	for (i=0; i<prvky_matice; i++) {
		if (fprintf(file, "%f ", mat->elem[i])<0) {
			fclose(file);
			return 0;
		}
	}

	// zatvorenie súboru
	fclose(file);

	// uspesny zapis oznacime navratovou hodnotou "1"
	return 1;
}


MAT* mat_create_by_file(char* filename) {
	int i;

	// otvorenie súboru na čítanie (rb- binárny)
	FILE* file = fopen(filename, "rb");

	// ošetrenie, či bol súbor úspešne otvorený
	if (file == NULL)
		return NULL;

	// ošetrenie, či naozaj sa v súbore nachádza "M 1"
	char prve_znaky[4];
	if (fgets(prve_znaky, sizeof(prve_znaky), file) == NULL || strncmp(prve_znaky, "M 1", 3) != 0) {
		fclose(file);
		return NULL;
	}

	// načítanie počtu riadkov a stĺpcov
	unsigned int riadky, stlpce;


	if (fscanf(file, "%u", &riadky) !=1) {
		fclose(file);
		return NULL;
	}
	
	if (fscanf(file, "%u", &stlpce) !=1) {
		fclose(file);
		return NULL;
	}

	// vytvorenie matice
	MAT* matica = mat_create_with_type(riadky, stlpce);
	if (matica == NULL) {
		fclose(file);
		return NULL;
	}

	// nacítanie prvkov matice
	unsigned int prvky_matice = riadky * stlpce;
	for (i = 0; i < prvky_matice; i++) {
		if (fscanf(file, "%f", &matica->elem[i]) != 1) {
			mat_destroy(matica);
			fclose(file);
			return NULL;
		}
	}

	// zatvorenie súboru
	fclose(file);

	// vrátime adresu vytvorenej reprezentácie matice
	return matica;
}




void mat_unit(MAT *mat) {
	unsigned int i, j;
	for (i = 0; i < mat->rows; i++) {
		for (j = 0; j < mat->cols; j++) {
			if (i == j)
				ELEM(*mat, i, j) = 1.0;  // prvok na diagonale
			else
				ELEM(*mat, i, j) = 0.0;  // ostatne prvky

		}
	}
}


void mat_random(MAT *mat) {
	unsigned int i, j;
	srand(time(NULL)); // generator nahodnych cisel

	for (i = 0; i < mat->rows; i++) {
		for (j = 0; j < mat->cols; j++) {
			// generovanie nahodnej hodnoty v intervale -1 až +1
			ELEM(*mat, i, j) = ((float)rand() / RAND_MAX) * 2 - 1;
		}
	}
}



void mat_print(MAT* mat) {
	int i, j;

	for (i = 0; i < mat->rows; i++) {
		for (j = 0; j < mat->cols; j++) {
			//formatovanie take, aby boli dostatocne medzery medzi prvkami
			printf("%10.2f ", ELEM(*mat, i, j));
		}
		printf("\n");
	}
}





main() {

// Vytvorenie matice
	MAT* mat = mat_create_with_type(10, 9);


// Vygenerovanie nahodnych hodnot pre maticu
	mat_unit(mat);

	printf("Vygenerovana matica vyzera nasledovne:\n");
	mat_print(mat);

// Zapis matice do suboru
	char filename[] = "matrix.txt";
	if (!mat_save(mat, filename)) {
		printf("Zapis do suboru sa nepodaril.\n");
		return 1;
	}

	printf("Matica bola zapisana do suboru '%s'\n", filename);


	// Nacitanie matice zo suboru
	MAT* loaded_mat = mat_create_by_file(filename);

	if (loaded_mat == NULL) {
		printf("Nacitanie matice zo suboru sa nepodarilo.\n");
		return 1;
	}

	// Vypis nacitanej matice
	printf("Nacitana matica:\n");
	mat_print(loaded_mat);
	


// Uvolnenie pamate
	mat_destroy(mat);
	mat_destroy(loaded_mat);

}

