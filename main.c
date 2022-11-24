#include <stdio.h>
#include<stdlib.h>
#include<string.h>

// PPM data
typedef struct {
    int red, green, blue, y, x;
} PPMPixel;

typedef struct {
    int x, y;
    PPMPixel *background;
    PPMPixel **image;
} PPMImage;

void printPixel(PPMPixel* pixel){
    printf("r[%d]g[%d]b[%d] at [%d,%d]\n", pixel->red, pixel->green, pixel->blue, pixel->y, pixel->x);
}

int isEqual(PPMPixel* p1, PPMPixel* p2){
    return p1->red == p2->red &&  p1->green == p2->green && p1->blue == p2->blue;
}
// endppm

//stack
typedef struct No {  // Definicao do tipo no (de uma pilha).
    PPMPixel iDado;       //  Um item de dado. Poderia ser outro tipo qquer, ate mesmo uma struct
    struct No *prox; // Link para o no abaixo do no atual na pilha
} No;                // O no da base aponta para NULL.
typedef struct { // Definicao do tipo pilha.
    No *topo;     // Ponteiro para o topo da pilha.
    int size;      // Tamanho da pilha: numero de nos.
}Pilha;
// Inicializa uma pilha.
void stack_init(Pilha *pilha){
    pilha->topo = NULL; // Pilha inicialmente vazia.
    pilha->size = 0;     // Tamanho inicial zero
}
// Coloca um no no topo da pilha
void stack_push(Pilha *pilha, PPMPixel valor){
    No *novo = (No*) malloc(sizeof(No)); // Cria novo no
    novo->iDado = valor;

//    printf("adding to stack - ");
//    printPixel(&valor);

    if (pilha->topo == NULL){ // Pilha vazia ?
        novo->prox = NULL;  // No na base nao tem proximo
        pilha->topo = novo; // novo no vai para o topo da pilha
    }
    else { // Pilha nao esta vazia
        novo->prox = pilha->topo;
        pilha->topo = novo;
    }
    pilha->size++; // Aumenta em um no o tamanho da pilha
};
// Retira o no do topo da pilha
PPMPixel* stack_pop(Pilha *pilha) {
    if (pilha->topo != NULL) {
        PPMPixel *aux = &pilha->topo->iDado;

        pilha->topo = pilha->topo->prox;

        pilha->size--;  // Diminui em um no o tamanho da pilha
        return aux;
    }
};
// endstack

int main() {
    const int MAX = 255;
    char path[255];
    printf("Type ppm path(len:%d): ", MAX);
    scanf("%s", path);

    PPMImage *imageData = malloc(sizeof(PPMImage));
    FILE *imageFile = fopen(path, "rb");

    if (!imageFile) {
        fprintf(stderr,"Error opening file! Your PATH [%s] may be wrong, please try again\n", path);
        exit(1);
    }

    char str[MAX];

    // safety checks:
    fscanf(imageFile, "%s", str);
    if(str[0] != 'P' || str[1] != '6'){
        fprintf(stderr, "This file is not formatted as a PPM file!\n");
        exit(1);
    }

    if(fscanf(imageFile, "%d %d", &imageData->y, &imageData->x) != 2){
        fprintf(stderr, "This file does not have a valid size!\n");
        exit(1);
    }

    imageData->image = (PPMPixel**) malloc(sizeof(PPMPixel*)*imageData->y);
    for(int i = 0; i < imageData->y; i++){
        imageData->image[i] = (PPMPixel*) calloc(imageData->x, sizeof(PPMPixel));
    }

    int rgb_comp_color;
    fscanf(imageFile, "%d", &rgb_comp_color);
    if(rgb_comp_color != 255) {
        fprintf(stderr, "Invalid rgb component (error loading '%s')\n", path);
        exit(1);
    }
    fgetc(imageFile);

    int count = 0;
    int line = 0;
    do
    {
        if(count == imageData->y){
            count = 0;
            line += 1;
        }

        unsigned char r, g, b;
        fscanf(imageFile, "%c%c%c", &r, &g, &b);

        imageData->image[line][count].red = r;
        imageData->image[line][count].green = g;
        imageData->image[line][count].blue = b;
        imageData->image[line][count].y = line;
        imageData->image[line][count].x = count;
//        printPixel(&imageData->image[line][count]);
        if (line == imageData->y-1 && count == imageData->x-1) {
            break;
        }
        count++;
    } while(1);
    fclose(imageFile);

    imageData->background = &imageData->image[0][0];
//    printPixel(imageData->background);
    int**visited = (int**) malloc(imageData->y * sizeof(int*));
    for(int i = 0; i < imageData->y; i++){
        visited[i] = (int*) calloc(imageData->x, sizeof(int*));
    }

    Pilha stack;
    stack_init(&stack);
    int objCount = 0;

    for(int y = 0; y < imageData->y; y++){
        for(int x = 0; x < imageData->x; x++){
            if(!visited[y][x]) {
                if (!isEqual(imageData->background, &imageData->image[y][x])) {
                    stack_push(&stack, imageData->image[y][x]);
                    printf("new object ");
                    printPixel(&imageData->image[y][x]);
                    objCount++;
                }else {
                    visited[y][x] = 1;
                }
            }
//            //se mais que zero
            while(stack.size>0) {
                PPMPixel* pixel;
                pixel = stack_pop(&stack);
                if(!visited[pixel->y][pixel->x]){
//                    printf("\tvisiting [%d,%d]\n", pixel->y, pixel->x);
                    visited[pixel->y][pixel->x] = 1;

//                    printf(" check1");
                    if(pixel->y < imageData->y){
                        if(!isEqual(imageData->background, &imageData->image[pixel->y+1][pixel->x])){
                            stack_push(&stack, imageData->image[pixel->y+1][pixel->x]);
                        }
                    }
//                    printf(" check2");
                    if(pixel->y > 0) {
                        if (!isEqual(imageData->background, &imageData->image[pixel->y - 1][pixel->x])) {
                            stack_push(&stack, imageData->image[pixel->y - 1][pixel->x]);
                        }
                    }
//                    printf(" check3");
                    if(pixel->x < imageData->x) {
                        if (!isEqual(imageData->background, &imageData->image[pixel->y][pixel->x + 1])) {
                            stack_push(&stack, imageData->image[pixel->y][pixel->x + 1]);
                        }
                    }
//                    printf(" check4");
                    if(pixel->x > 0) {
                        if (!isEqual(imageData->background, &imageData->image[pixel->y][pixel->x - 1])) {
                            stack_push(&stack, imageData->image[pixel->y][pixel->x - 1]);
                        }
                    }
//                    printf(" - ALL OK\n");
                }else{
//                    printf("already visited [%d,%d] -", pixel->y, pixel->x);
                }
            }
        }
    }

    printf("\n\n%d", objCount);

    return 0;
}
