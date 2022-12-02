#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int** importData(char* filename, int* nLato);
int** cloneMatrice(int** matr, int nLato);
int pre_solver_metodo1(int** matrice, int nLato, int nSeg);
int pre_solver_metodo2(int** matrice, int nLato, int nSeg);
int solver_metodo1(int** matrice, int nLato, int nSeg, int pos);
int solver_metodo2(int** matrice, int nLato, int nSeg, int index_pos, int* pos, int nPos);
int validate_metod1(int **matrice, int nLato, int nSeg, int pos);

int main(void) {
    int nLato;
    double diff;
    time_t pre_time;
    time_t post_time;
    int** matrice = importData("../import_9.txt", &nLato);


    /*pre_time = time(NULL);
    solver_metodo1(cloneMatrice(matrice, nLato), nLato, 3, 0);
    post_time = time(NULL);
    diff = difftime(post_time, pre_time);
    printf("%f s impiegati\n", diff);*/

    pre_time = time(NULL);
    pre_solver_metodo2(cloneMatrice(matrice, nLato), nLato, 3);
    post_time = time(NULL);
    diff = difftime(post_time, pre_time);
    printf("%f s impiegati\n", diff);

    return 0;
}

int** importData(char* filename, int *nLato){
    FILE *fp = fopen(filename, "r");
    if(fp == NULL) return NULL;
    int tmp;
    fscanf(fp, "%d", &tmp);
    *nLato = tmp;

    int** matrice = (int**) malloc(*nLato * sizeof(int*));

    for(int i = 0; i < *nLato; i++){
        matrice[i] = (int*) malloc(*nLato * sizeof(int));
        for(int j = 0; j < *nLato; j++){
            fscanf(fp, "%d", &matrice[i][j]);
        }
    }
    fclose(fp);
    return matrice;
}

int** cloneMatrice(int** matr, int nLato){
    int** new = malloc(nLato *sizeof (int*));
    for(int i = 0; i < nLato; i++){
        new[i] = malloc(nLato*sizeof(int));
        for(int j = 0; j < nLato; j++){
            new[i][j] = matr[i][j];
        }
    }

    return new;
}

/* Si organizza il vettore pos in ordine da in alto a sx fino in basso a dx, riga per riga */
/* Usa: solver_metodo2 */
int pre_solver_metodo1(int** matrice, int nLato, int nSeg){
    int* pos = malloc(nLato * nLato *sizeof (int));
    for(int i = 0; i < nLato * nLato; i++) {
        pos[i] = i;
    }

    solver_metodo2(matrice, nLato, nSeg, 0, pos, nLato*nLato);
}

/* Usa: solver_metodo2 */
int pre_solver_metodo2(int** matrice, int nLato, int nSeg){
    int** nRiempimenti = malloc(nSeg*nSeg*sizeof(int*));
    for(int i = 0; i < nSeg; i++){
        for(int j = 0; j < nSeg; j++) {
            nRiempimenti[i*nSeg + j] = malloc(2*sizeof(int));
            nRiempimenti[i*nSeg + j][0] = 0;
            nRiempimenti[i*nSeg + j][1] = i*nSeg+j;
            for (int k = 0; k < nSeg; k++)
                for (int l = 0; l < nSeg; l++)
                    if(matrice[i*nSeg + k][j*nSeg + l] != 0)
                        nRiempimenti[i*nSeg+j][0]++;
        }
    }

    for(int i = 0; i < nSeg * nSeg - 1; i++){
        for(int j = i; j < nSeg*nSeg; j++){
            if(nRiempimenti[i][0] < nRiempimenti[j][0]){
                int tmp = nRiempimenti[i][0];
                nRiempimenti[i][0] = nRiempimenti[j][0];
                nRiempimenti[j][0] = tmp;

                tmp = nRiempimenti[i][1];
                nRiempimenti[i][1] = nRiempimenti[j][1];
                nRiempimenti[j][1] = tmp;
            }
        }
    }

    int nPos = 0;
    int* pos = malloc(nLato*nLato*sizeof(int));

    for(int i = 0; i < nSeg * nSeg; i++){
        //printf("%d %d\n", nRiempimenti[i][0], nRiempimenti[i][1]);

        for(int j = 0; j < nSeg; j++){
            for(int k = 0; k < nSeg; k++){
                if(matrice[(int) (nRiempimenti[i][1] / nSeg) * nSeg + j][(int) (nRiempimenti[i][1] % nSeg) * nSeg + k] == 0) {
                    pos[nPos] = ((int) (nRiempimenti[i][1] / nSeg) * nSeg + j) * nLato +
                                ((int) (nRiempimenti[i][1] % nSeg) * nSeg + k);
                    nPos++;
                }
            }
        }
    }

    return solver_metodo2(matrice, nLato, nSeg, 0, pos, nPos);


}

/* Metodo classico: si parte da in alto a sinistra e si scorre andando prima a destra poi in basso riga per riga
 * Si provano tutti i numeri da 1 a 9 */
int solver_metodo1(int** matrice, int nLato, int nSeg, int pos){
    //printf("pos: %d\n", pos);
    if(pos >= nLato * nLato){
        for(int i = 0; i < nLato; i++){
            for(int j = 0; j < nLato; j++)
                printf("%d ", matrice[i][j]);
            printf("\n");
        }
        return 1;
    }

    int i = pos / nLato;
    int j = pos % nLato;
    //printf("i: %d, j: %d, matr: %d\n", i, j, matrice[i][j]);

    if(matrice[i][j] != 0)
        return solver_metodo1(matrice, nLato, nSeg, pos + 1);

    for(int num = 1; num < nLato +1; num++){
        matrice[i][j] = num;
        //printf("inserisci %d\n", num);
        if(validate_metod1(matrice, nLato, nSeg, pos) == 1 && solver_metodo1(matrice, nLato, nSeg, pos+1) == 1){
            return 1;
        }
        //printf("remove %d\n", num);
    }
    matrice[i][j] = 0;
    return 0;
}

/* Si provano tutti i numeri da 1 a 9, ma le celle vengono analizzate nell'ordine indicato nel vettore pos */
int solver_metodo2(int** matrice, int nLato, int nSeg, int index_pos, int* pos, int nPos){
    //printf("%d %d\n", index_pos, pos[index_pos]);

    if(index_pos >= nPos){
        for(int i = 0; i < nLato; i++){
            for(int j = 0; j < nLato; j++)
                printf("%d ", matrice[i][j]);
            printf("\n");
        }
        return 1;
    }


    int i = pos[index_pos] / nLato;
    int j = pos[index_pos] % nLato;
    //printf("i: %d, j: %d, matr: %d\n", i, j, matrice[i][j]);

    if(matrice[i][j] != 0)
        return solver_metodo2(matrice, nLato, nSeg, index_pos + 1, pos,nPos);

    for(int num = 1; num < nLato +1; num++){
        //printf("%d provo %d\n", pos[index_pos], num);
        matrice[i][j] = num;
        if(validate_metod1(matrice, nLato, nSeg, pos[index_pos]) == 1 && solver_metodo2(matrice, nLato, nSeg, index_pos + 1, pos, nPos) == 1){
            return 1;
        }
    }
    matrice[i][j] = 0;
    return 0;
}

int validate_metod1(int **matrice, int nLato, int nSeg, int pos){
    int i = pos / nLato;
    int j = pos % nLato;

    /*for(int a = 0; a < nLato; a++){
        for(int b = 0; b < nLato; b++)
            printf("%d ", matrice[a][b]);
        printf("\n");
    }*/

    for(int k = 0; k < nLato; k++){
        if(k != j && matrice[i][k] == matrice[i][j])
            return 0;
        if(k != i && matrice[k][j] == matrice[i][j])
            return 0;
    }

    int a = i / nSeg;
    int b = j / nSeg;

    //printf("valido 1 e 2\n");

    for(int k = 0; k < nSeg; k++){
        for(int l = 0; l < nSeg; l++){
            if((((a*nSeg) + k) != i || (b*nSeg) + l != j) && matrice[(int)(a*nSeg) + k][(int)(b*nSeg) + l] == matrice[i][j]){
                return 0;
            }
        }
    }
    //printf("valido 3\n");
    return 1;
}
