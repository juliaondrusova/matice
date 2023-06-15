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

	// otvorenie suboru na citanie (rb- binarny)
	FILE* file = fopen(filename, "rb");

	// osetrenie, ci bol súbor uspesne otvoreny
	if (file == NULL)
		return NULL;

	// osetrenie, ci sa naozaj v subore nachadza "M 1"
	char prve_znaky[4];
	if (fgets(prve_znaky, sizeof(prve_znaky), file) == NULL || strncmp(prve_znaky, "M 1", 3) != 0) {
		fclose(file);
		return NULL;
	}

	// nacitanie poctu riadkov a stlpcov
	unsigned int riadky, stlpce;


	if (fscanf(file, "%u", &riadky) !=1) {
		fclose(file);
		return NULL;
	}

	if (fscanf(file, "%u", &stlpce) !=1) {
		fclose(file);
		return NULL;
	}

	// vytvorenie matice (resp. jej reprezentacie v pamati)
	MAT* matica = mat_create_with_type(riadky, stlpce);
	if (matica == NULL) {
		fclose(file);
		return NULL;
	}

	// nacitanie prvkov matice
	unsigned int prvky_matice = riadky * stlpce;
	for (i = 0; i < prvky_matice; i++) {
		if (fscanf(file, "%f", &matica->elem[i]) != 1) {
			mat_destroy(matica);
			fclose(file);
			return NULL;
		}
	}

	// zatvorenie suboru
	fclose(file);

	// vratime adresu vytvorenej reprezentacie matice
	return matica;
}




void mat_unit(MAT *mat) {

	int i, j;
	for (i=0; i< mat->rows; i++) {
		for (j=0; j<mat->cols; j++) {
			if (i==j)
				ELEM(*mat, i, j) = 1.0;  // prvok na diagonale
			else
				ELEM(*mat, i, j) = 0.0;  // ostatne prvky

		}
	}
}


void mat_random(MAT *mat) {
	int i, j;
	srand(time(NULL)); // generator nahodnych cisel

	for (i=0; i<mat->rows; i++) {
		for (j=0; j<mat->cols; j++) {
			// generovanie nahodnej hodnoty v intervale -1 až +1
			ELEM(*mat, i, j) = ((float)rand() / RAND_MAX) * 2 - 1;
		}
	}
}



void mat_print(MAT* mat) {
	int i, j;

	for (i= 0; i < mat->rows; i++) {
		for (j= 0; j < mat->cols; j++) {
			//formatovanie take, aby boli dostatocne medzery medzi prvkami
			printf("%7.2f ", ELEM(*mat, i, j));
		}
		printf("\n");
	}
}

//pomocna funkcia pre dolnu trojuholnikovu maticu
int hladaj_nuly (MAT* mat, int riadok, int stlpec, int ziadany_pocet_nul) {

	int i,j;
	int pocitadlo_nul=0;

	// zarazka, aby funkcia nebezala do nekonecna
	if (riadok > mat->rows)
		return riadok-1;

	// prechadzaj vsetky stlpce
	for (j = stlpec; j<mat->cols; j++) {

		//ak je prvok rovny 0, tak inkrementuj pocitadlo_nul
		if (ELEM(*mat, riadok,j)==0) {
			pocitadlo_nul++;

			// ak sme nasli ziadany_pocet_nul, tak sa vnarame hlbsie do funkcie
			if (pocitadlo_nul==ziadany_pocet_nul) {

				//rekurzivna funkcia
				int max_riadok= hladaj_nuly(mat, riadok+1, stlpec, ziadany_pocet_nul+1);

				//ak sa funkcia dostala o riadok nizsie, tak vraciame prave ten riadok
				if (max_riadok>riadok) {
					return max_riadok;
				}
				//ak sa funkcia nedostala o riadok nizsie, tak vraciame uz predtym "najdeny" riadok
				else
					return riadok;
			}
		}
		// ak prvok nie je rovny 0, tak vratime riadok pred inkrementovanim (resp. ten co sme nasli uz predtym)
		else
			return riadok-1;
	}
	//ak sa dostaneme na posledny stlpec, tak vratime uz predtym "najdeny" riadok
	return riadok-1;
}


//funkcia hlada dolnu trojuholnikovu maticu
int find_triangular_block(MAT* mat, unsigned int* a, unsigned int* b, unsigned int* c, unsigned int* d) {
	unsigned int max_rozmer = 0;  // velkost najvacsieho najdeneho bloku
	unsigned int aktual_rozmer = 0;  // velkost aktualneho bloku
	unsigned int zaciat_riadok = 0;  // riadok, kde zacina aktualny blok
	unsigned int koniec_riadok = 0;  // riadok, kde konci aktualny blok
	unsigned int zaciat_stlpec = 0;  // stlpec, kde zacina aktualny blok
	unsigned int koniec_stlpec = 0;  // sltpec, kde konci aktualny blok

	int i, j;
	int rozdiel_riadkov;

	// prechadzaj vvetky riadky
	for (i=0; i<mat->rows; i++) {
		// prechedzaj vsetky stlpce
		for (j = 0; j<mat->cols; j++) {
			
			//ak je prvok rovny 0, tak volame funkciu hladaj_nuly
			if (ELEM(*mat, i, j)==0.0) {
				rozdiel_riadkov=hladaj_nuly(mat, i+1, j, 2)-i;

				//ak najdeney blok je vacsi ako doterajsi, tak aktualizuj hodnoty premennych
				if ((rozdiel_riadkov+1)>max_rozmer) {
					max_rozmer=rozdiel_riadkov+1;
					zaciat_riadok=i;
					zaciat_stlpec=j;
					koniec_riadok=i+rozdiel_riadkov;
					koniec_stlpec=j+rozdiel_riadkov;
				}
			}
		}
	}

	//priradenie najdenych hodnot
	*a=zaciat_riadok;
	*b=koniec_riadok;
	*c=zaciat_stlpec;
	*d=koniec_stlpec;

	//vraciame maximalny najdeny blok
	return max_rozmer;


}


//pomocna funkcia pre hornu trojuholnikovu maticu
//pracuje podobne ako funkcia hladaj_nuly, no ideme od posledneho prvku smerom hore
int hladaj_nuly1(MAT* mat, int riadok, int stlpec, int ziadany_pocet_nul) {
	int i, j;
	int pocitadlo_nul = 0;

	//zarazka- ak je riadok mensi ako 0, tak opustame funkciu
	if(riadok<0) 
		return 0;

	// prechadzaj vsetky stlpce
	for (j = stlpec; j >= 0; j--) {
		if (ELEM(*mat, riadok, j) == 0.0) {
			pocitadlo_nul++;
			if (pocitadlo_nul == ziadany_pocet_nul) {
				int max_riadok = hladaj_nuly1(mat, riadok - 1, stlpec, ziadany_pocet_nul + 1);
				if (max_riadok < riadok)
					return max_riadok;
				else
					return riadok;
			}
		} else
			return riadok + 1;
	}
	return riadok + 1;
}

//funkcia hlada hornu trojuholnikovu maticu
//pracuje podobne ako funkcia find_triangular_block, no ideme od posledneho prvku smerom hore
int find_triangular_block1(MAT* mat, unsigned int* a, unsigned int* b, unsigned int* c, unsigned int* d) {
	unsigned int max_rozmer = 0;  // velkost najvacsieho najdeneho bloku
	unsigned int aktual_rozmer = 0;  // velkost aktualneho bloku
	unsigned int zaciat_riadok = 0;  // riadok, kde zacina aktualny blok
	unsigned int koniec_riadok = 0;  // riadok, kde konci aktualny blok
	unsigned int zaciat_stlpec = 0;  // stlpec, kde zacina aktualny blok
	unsigned int koniec_stlpec = 0;  // stlpec, kde konci aktualny blok

	int i, j;
	int rozdiel_riadkov;

	// prechadzaj vsetky riadky od posledneho po prvy
	for (i = mat->rows - 1; i >= 0; i--) {
		// prechadzaj vsetky stlpce
		for (j = mat->cols - 1; j >= 0; j--) {
			if (ELEM(*mat, i, j) == 0.0) {
				rozdiel_riadkov = i - hladaj_nuly1(mat, i - 1, j, 2);

				if ((rozdiel_riadkov + 1) > max_rozmer) {
					max_rozmer = rozdiel_riadkov + 1;
					zaciat_riadok = i - rozdiel_riadkov;
					zaciat_stlpec = j - rozdiel_riadkov;
					koniec_riadok = i;
					koniec_stlpec = j;
				}
			}
		}
	}

	*a = zaciat_riadok;
	*b = koniec_riadok;
	*c = zaciat_stlpec;
	*d = koniec_stlpec;

	return max_rozmer;


}

//funkcia sluzi na porovnanie velkosti najdenych blokov predchadzajucich dvoch funkcii
//uklada adresy toho vacsieho bloku
int najvacsia_trojuholnikova_matica (MAT* mat, unsigned int* a, unsigned int* b, unsigned int* c, unsigned int* d) {

	unsigned int aa, bb, cc, dd;
	unsigned int ee, ff, gg, hh;

	int rozmer= find_triangular_block(mat, &aa, &bb, &cc, &dd);
	int rozmer1= find_triangular_block1(mat, &ee, &ff, &gg, &hh);

	if (rozmer>rozmer1) {
		*a=aa;
		*b=bb;
		*c=cc;
		*d=dd;
	}

	else {
		*a=ee;
		*b=ff;
		*c=gg;
		*d=hh;
	}

	// ak nebol ani v jednej funkcii najdeny blok, tak vraciame 0
	if (rozmer1 <1 && rozmer<1)
		return 0;

	else
		return 1;

}


void vypis_vysledkov (unsigned int a, unsigned int b, unsigned int c, unsigned int d) {
	
		printf("\nNajvacsi suvisly blok v trojuholnikovom tvare:\n");
		printf("Zaciatok riadku: %u\n", a+1);
		printf("Koniec riadku: %u\n", b+1);
		printf("Zaciatok stlpca: %u\n", c+1);
		printf("Koniec stlpca: %u\n", d+1);
	
	
}




main() {

// Vytvorenie matice
	MAT* mat = mat_create_with_type(4, 4);


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
	char filename1[] = "matrix1.txt";
	// Nacitanie matice zo suboru
	MAT* loaded_mat = mat_create_by_file(filename1);

	if (loaded_mat == NULL) {
		printf("Nacitanie matice zo suboru sa nepodarilo.\n");
		return 1;
	} else
		printf("\nMatica bola nacitana zo suboru '%s'\n", filename1);

	// Vypis nacitanej matice
	printf("Nacitana matica:\n");
	mat_print(loaded_mat);


	unsigned int a, b, c, d;
	int vysledok=najvacsia_trojuholnikova_matica(loaded_mat, &a, &b, &c, &d);

	if (vysledok) {
		vypis_vysledkov (a,b,c,d);
	}

	else
		printf ("Blok sa nenasiel");

// Uvolnenie pamate
	mat_destroy(mat);
	mat_destroy(loaded_mat);

}

