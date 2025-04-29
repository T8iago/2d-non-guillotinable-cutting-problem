#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/types.h>

typedef struct {

  int a;    // height of the rectangle ('a' for 'altura' in portuguese)
  int l;    // length of the rectangle
  int x, y; // coordinates of the rectangle, in the bottom left vertex
  int number, area;

} rectangleSet;

typedef struct {

  int upperY, lowerY, rightX, leftX;

} bigRectangles;



// the idea is: each square put into the area is creating 1-2 squares (as we are
// using the order of putting to the left, then to put it below)
typedef struct {

  int x, y; // the coordinates of the corner

} intersectionSet;

int min(int a, int b) { return (a < b) ? a : b; }

int max(int a, int b) { return (a > b) ? a : b; }

// first things first ill do a quicksort here
void swap(rectangleSet *a, rectangleSet *b) {
  rectangleSet temp = *a;
  *a = *b;
  *b = temp;
}

int partition(rectangleSet * rect, int low, int high) {
  int pivot = rect[high].area;
  int i = (low - 1);

  for (int j = low; j <= high - 1; j++) {
    if (rect[j].area >= pivot) {
      i++;
      // swap(&rect[i].l, &rect[j].l);
      // swap(&rect[i].y, &rect[j].y);
      // swap(&rect[i].number, &rect[j].number);
      swap(&rect[i], &rect[j]);
    }
  }

  // swap(&rect[i + 1].l, &rect[high].l);
  // swap(&rect[i + 1].a, &rect[high].a);
  // swap(&rect[i + 1].number, &rect[high].number);
  swap(&rect[i + 1], &rect[high]);
  return (i + 1);
}

void quickSortLength(rectangleSet * rect, int low, int high) {
  if (low < high) {
    int pi = partition(rect, low, high);

    quickSortLength(rect, low, pi - 1);
    quickSortLength(rect, pi + 1, high);
  }
}

void doBig(int n, rectangleSet *rect, int verified, int placing,
           bigRectangles *edge) {
  int temp;

  edge[verified].leftX = rect[verified].x - rect[placing].l;
  edge[verified].lowerY = rect[verified].y - rect[placing].a;
  edge[verified].rightX = rect[verified].x + rect[verified].l;
  edge[verified].upperY = rect[verified].y + rect[verified].a;
  // printf ("%d edges: %d, %d, %d, %d \n", verified, edge[verified].leftX,
  // edge[verified].lowerY, edge[verified].rightX, edge[verified].upperY);
}

int intersectionFilter2 (int placing, int maxHeight, int maxLength, rectangleSet *rect, intersectionSet *temp, bigRectangles *edge) {

  // printf("\n");
  int maxY = maxHeight - rect[placing].a;
  int maxX = maxLength - rect[placing].l;

  int result = 0;
  // for (int i = 1; i <= placing; i++) {
  for (int k = 1; k < placing; k++) {
    // printf("Iteracao %d | %d, %d, %d, %d | (%d, %d) ", j, edge[k].lowerY, edge[k].upperY, edge[k].leftX, edge[k].rightX, temp->x, temp->y);
    if (temp->y > edge[k].lowerY &&
        temp->y < edge[k].upperY &&
        temp->x > edge[k].leftX &&
        temp->x < edge[k].rightX) {
      result = 1;
      // printf("AQUI 1\n");
      //("AQUI 1\n");
    }
    if (temp->y < 0 || temp->y > maxY) {
      // printf("AQUI 2\n");
      result = 1;
      //("AQUI 2\n");
    }
    if (temp->x < 0 || temp->x > maxX) {
      // printf("AQUI 3\n");
      result = 1;
      //("AQUI 3\n");
    }
  }

  // printf("RESULT == %d\n\n", result);
  return result;
    // printf("\n\n");

}

int betterIntersection(intersectionSet *temp, intersectionSet *intersection) {
  if (temp->x < intersection->x) {
    return 0;
  } else if (temp->x == intersection->x) {
    if (temp->y < intersection->y) {
      return 0;
    } else {
      return 1;
    }
  } else {
    return 1;
  }
}

void findIntersections(int n, int placing, rectangleSet *rect,
                       bigRectangles *edge, int maxHeight, int maxLength) {
  int count = 0;

  intersectionSet temp, intersection;

  intersection.x = INT_MAX;
  intersection.y = INT_MAX;
  // 'placing' can only be between 2 and n
  // if (placing == 2) {
  // Caso específico para placing = 2
  // doBig(n, rect, 1, placing, edge); // Calcula as bordas apenas para o
  // primeiro retângulo
  //} else {
  // Caso geral para placing > 2

  //printf("\n\n");
  for (int i = placing - 1; i >= 1; i--) {
    doBig(n, rect, i, placing, edge); // Check where you can place the
    if (edge[i].leftX > edge[i].rightX) {
      int temp = edge[i].leftX;
      edge[i].leftX = edge[i].rightX;
      edge[i].rightX = temp;
    }
    if (edge[i].lowerY > edge[i].upperY) {
      int temp = edge[i].lowerY;
      edge[i].lowerY = edge[i].upperY;
      edge[i].upperY = temp;
    }

    // rectangles within the other rectangles.
    //printf("%d Iteration, %d Rectangle = %d - %d - %d - %d\n", placing, i, edge[i].lowerY, edge[i].rightX, edge[i].upperY, edge[i].leftX);
  }
 // printf("\n");

  //}

  // restrictions for it to find all the intersections
  for (int i = 2; i <= n; i++) {
    for (int j = i - 1; j >= 1; j--) {
      // Intersections with Y = 0
      if (edge[j].lowerY <= 0 <= edge[j].upperY) {
        if (0 < edge[i].leftX < maxLength) {
          temp.x = edge[j].leftX;
          temp.y = 0;

          // printf("%d ", k);
            if (intersectionFilter2(placing, maxHeight, maxLength, rect, &temp, edge) == 0) {
              if (betterIntersection(&temp, &intersection) == 0) {
                intersection.x = temp.x;
                intersection.y = temp.y;
                // printf("-1 It: %d; Rect: %d; Comprd: %d ; Int: (%d, %d) \n", placing, i, j, intersection.x, intersection.y);
              }
            }

          // printf("\n");
        }
        if (0 < edge[i].rightX < maxLength) {
          temp.x = edge[j].rightX;
          temp.y = 0;

          // printf("%d ", k);
            if (intersectionFilter2(placing, maxHeight, maxLength, rect, &temp, edge) == 0) {
              if (betterIntersection(&temp, &intersection) == 0) {
                intersection.x = temp.x;
                intersection.y = temp.y;
                // printf("0  It: %d; Rect: %d; Comprd: %d ; Int: (%d, %d) \n", placing, i, j, intersection.x, intersection.y);
              }
            }

          // printf("\n");
        }
      }
      // Intersections with X = 0
      if (edge[j].leftX <= 0 <= edge[j].rightX) {
        if (0 < edge[i].lowerY < maxHeight) {
          temp.x = 0;
          temp.y = edge[j].lowerY;

          // printf("%d ", k);
            if (intersectionFilter2(placing, maxHeight, maxLength, rect, &temp, edge) == 0) {
              if (betterIntersection(&temp, &intersection) == 0) {
                intersection.x = temp.x;
                intersection.y = temp.y;
                // printf("1  It: %d; Rect: %d; Comprd: %d ; Int: (%d, %d) \n", placing, i, j, intersection.x, intersection.y);
              }
            }

          // printf("\n");
        }
        if (0 < edge[i].upperY < maxHeight) {
          temp.x = 0;
          temp.y = edge[j].upperY;

          // printf("%d ", k);
            if (intersectionFilter2(placing, maxHeight, maxLength, rect, &temp, edge) == 0) {
              if (betterIntersection(&temp, &intersection) == 0) {
                intersection.x = temp.x;
                intersection.y = temp.y;
                // printf("2  It: %d; Rect: %d; Comprd: %d ; Int: (%d, %d) \n", placing, i, j, intersection.x, intersection.y);
              }
            }

          // printf("\n");
        }
      }

      // Intersections with lowerY
      if (edge[j].lowerY <= edge[i].lowerY <= edge[j].upperY) {
        if (edge[i].leftX < edge[j].leftX < edge[i].rightX) {
          temp.x = edge[j].leftX;
          temp.y = edge[i].lowerY;

          // printf("%d ", k);
            if (intersectionFilter2(placing, maxHeight, maxLength, rect, &temp, edge) == 0) {
              if (betterIntersection(&temp, &intersection) == 0) {
                intersection.x = temp.x;
                intersection.y = temp.y;
                // printf("3  It: %d; Rect: %d; Comprd: %d ; Int: (%d, %d) \n", placing, i, j, intersection.x, intersection.y);
              }
            }

          // printf("\n");
        }
        if (edge[i].leftX < edge[j].rightX < edge[i].rightX) {
          temp.x = edge[j].rightX;
          temp.y = edge[i].lowerY;

          // printf("%d ", k);
            if (intersectionFilter2(placing, maxHeight, maxLength, rect, &temp, edge) == 0) {
              if (betterIntersection(&temp, &intersection) == 0) {
                intersection.x = temp.x;
                intersection.y = temp.y;
                // printf("4  It: %d; Rect: %d; Comprd: %d ; Int: (%d, %d) \n", placing, i, j, intersection.x, intersection.y);
              }
            }

          // printf("\n");
        }
      }
      // Intersectionbs with upperY
      if (edge[j].lowerY <= edge[i].upperY <= edge[j].upperY) {
        if (edge[i].leftX < edge[j].leftX < edge[i].rightX) {
          temp.x = edge[j].leftX;
          temp.y = edge[i].upperY;

          // printf("%d ", k);
            if (intersectionFilter2(placing, maxHeight, maxLength, rect, &temp, edge) == 0) {
              if (betterIntersection(&temp, &intersection) == 0) {
                intersection.x = temp.x;
                intersection.y = temp.y;
                // printf("5  It: %d; Rect: %d; Comprd: %d ; Int: (%d, %d) \n", placing, i, j, intersection.x, intersection.y);
              }
            }

          // printf("\n");
        }
        if (edge[i].leftX < edge[j].rightX < edge[i].rightX) {
          temp.x = edge[j].rightX;
          temp.y = edge[i].upperY;

          // printf("%d ", k);
            if (intersectionFilter2(placing, maxHeight, maxLength, rect, &temp, edge) == 0) {
              if (betterIntersection(&temp, &intersection) == 0) {
                intersection.x = temp.x;
                intersection.y = temp.y;
                // printf("6  It: %d; Rect: %d; Comprd: %d ; Int: (%d, %d) \n", placing, i, j, intersection.x, intersection.y);
              }
            }

          // printf("\n");
        }
      }
      // Intersection with leftX
      if (edge[j].leftX <= edge[i].leftX <= edge[j].rightX) {
        if (edge[i].lowerY < edge[j].lowerY < edge[i].upperY) {
          temp.x = edge[i].leftX;
          temp.y = edge[j].lowerY;

          // printf("%d ", k);
            if (intersectionFilter2(placing, maxHeight, maxLength, rect, &temp, edge) == 0) {
              if (betterIntersection(&temp, &intersection) == 0) {
                intersection.x = temp.x;
                intersection.y = temp.y;
                // printf("7  It: %d; Rect: %d; Comprd: %d ; Int: (%d, %d) \n", placing, i, j, intersection.x, intersection.y);
              }
            }

          // printf("\n");
        }
        if (edge[i].lowerY < edge[j].upperY < edge[i].upperY) {
          temp.x = edge[i].leftX;
          temp.y = edge[j].upperY;

          // printf("%d ", k);
            if (intersectionFilter2(placing, maxHeight, maxLength, rect, &temp, edge) == 0) {
              if (betterIntersection(&temp, &intersection) == 0) {
                intersection.x = temp.x;
                intersection.y = temp.y;
                // printf("8  It: %d; Rect: %d; Comprd: %d ; Int: (%d, %d) \n", placing, i, j, intersection.x, intersection.y);
              }
            }

          // printf("\n");
        }
      }
      // Intersections with rightX
      if (edge[j].leftX <= edge[i].rightX <= edge[j].rightX) {
        if (edge[i].lowerY < edge[j].lowerY < edge[i].upperY) {
          temp.x = edge[i].rightX;
          temp.y = edge[j].lowerY;

          // printf("%d ", k);
            if (intersectionFilter2(placing, maxHeight, maxLength, rect, &temp, edge) == 0) {
              if (betterIntersection(&temp, &intersection) == 0) {
                intersection.x = temp.x;
                intersection.y = temp.y;
                // printf("9  It: %d; Rect: %d; Comprd: %d ; Int: (%d, %d) \n", placing, i, j, intersection.x, intersection.y);
              }
            }

          // printf("\n");
        }
        if (edge[i].lowerY < edge[j].upperY < edge[i].upperY) {
          temp.x = edge[i].rightX;
          temp.y = edge[j].upperY;

          // printf("%d ", k);
            if (intersectionFilter2(placing, maxHeight, maxLength, rect, &temp, edge) == 0) {
              if (betterIntersection(&temp, &intersection) == 0) {
                intersection.x = temp.x;
                intersection.y = temp.y;
                // printf("10 It: %d; Rect: %d; Comprd: %d ; Int: (%d, %d) \n", placing, i, j, intersection.x, intersection.y);
              }
            }

          //printf("\n");
        }
      }
    }
  }

  rect[placing].x = intersection.x;
  rect[placing].y = intersection.y;
  // printf("%d CHOSEN (%d, %d)\n", placing, rect[placing].x, rect[placing].y);

}

int minLength (int n, rectangleSet *rect) {

  int min = rect[1].x + rect[1].l;

  for (int i = 2; i <= n; i++) {
    if ((rect[i].x + rect[i].l) > min) {
      min = rect[i].x + rect[i].l;
    }
  }

  return min;

}

void tikzRectangles(int n, rectangleSet *rect, FILE *file, int maxHeight,
                    int maxLength, float time, const char *fileName) {

  // Packages and begin document in tex
  fprintf(file, "\\documentclass{article}\n");
  fprintf(file, "\\usepackage[a1paper, landscape]{geometry}\n");
  fprintf(file, "\\usepackage{graphicx} %% Required for inserting images\n");
  fprintf(file, "\\usepackage{float}\n");
  fprintf(file, "\\usepackage{tikz}\n");
  fprintf(file, "\\usetikzlibrary{arrows,calc}\n");
  fprintf(file, "\\tikzset{\n");
  fprintf(file, "%%Define standard arrow tip\n");
  fprintf(file, ">=stealth',\n");
  fprintf(file, "%%Define style for different line styles\n");
  fprintf(file, "help lines/.style={dashed, thick},\n");
  fprintf(file, "axis/.style={<->},\n");
  fprintf(file, "important line/.style={thick},\n");
  fprintf(file, "connection/.style={thick, dotted},\n");
  fprintf(file, "}\n");
  fprintf(file, "\n");
  fprintf(file, "\\begin{document}\n");
  fprintf(file, "\n");
  fprintf(file, "\\centering\n");
  fprintf(file, "\n");
  fprintf(file, "\\LARGE{Sorted by area}\n");
  fprintf(file, "\n");
  fprintf(file, "\\LARGE{Input %s}\n", fileName);
  fprintf(file, "\n");

  // Tikz for the rectangle and the cartesian plane
  fprintf(file, "\\begin{figure}[H]\n");
  fprintf(file, "	\\centering\n");
  fprintf(file, "\\begin{tikzpicture}[scale=0.16]\n");
  fprintf(file, "\\centering\n");
  fprintf(file, "\n");

  // Drawing the graph
  fprintf(file, "	%% Draw the graph\n");
  fprintf(file, "	\\coordinate (y) at (0, %dmm);\n", maxHeight + 1);
  fprintf(file, "	\\coordinate (x) at (%dmm,0);\n", maxLength + 1);
  fprintf(file, "	\\draw[axis] (y) -- (0,0) -- (x);\n");
  fprintf(file, "\n");
  fprintf(file,
          "	%% Point where the height and the length will be drawn\n");
  fprintf(file, "	\\coordinate (height)	at (0, %dmm);\n", maxHeight);
  fprintf(file, "	\\coordinate (length) at (%dmm , 0);\n", maxLength);
  fprintf(file, "\n");
  fprintf(file, "	%% Max height\n");
  fprintf(file,
          "	\\draw[ultra thick] (height) node[left] {$A$} -- (%dmm , %dmm);\n",
          maxLength + 1, maxHeight);
  fprintf(file, "	%% Plane's 0 point\n");
  fprintf(file,
          "	\\fill (0,0) node[below] {$(0,0)$} circle[radius=2pt];\n");

  // Draw the rectangle
  for (int i = 1; i < n + 1; i++) {
    fprintf(file, "\n");
    fprintf(file, "	%% Rectangle %d\n", rect[i].number);
    // fprintf(file, "		\\draw[thick] (0,0) coordinate (A) -- ( %d ,0)
    // coordinate (B) -- (%d , %d) coordinate (C) -- (0, %d ) coordinate (D) --
    // cycle;\n", rect[i].l, rect[i].l, rect[i].a, rect[i].a);

    // puts the coordinates (A) = x,y; (B) = x+l,y; (C) = x+l,y+a; (D) = x,y+a
    fprintf(file, "		\\draw[thick] (%dmm , %dmm) coordinate (A)",
            rect[i].x, rect[i].y);
    fprintf(file, " -- (%dmm , %dmm) coordinate (B)", rect[i].x + rect[i].l,
            rect[i].y);
    fprintf(file, " -- (%dmm , %dmm) coordinate (C)", rect[i].x + rect[i].l,
            rect[i].y + rect[i].a);
    fprintf(file, " -- (%dmm , %dmm) coordinate (D) -- cycle;\n", rect[i].x,
            rect[i].y + rect[i].a);

    fprintf(file, "		\\fill[green!20, opacity=0.5] (A) -- (B) -- "
                  "(C) -- (D) -- cycle;\n");
    fprintf(file, "\n");
    fprintf(file, "		%% Number centered in the rectangle\n");
    fprintf(file, "		\\coordinate (mid) at ($(A)!0.5!(C)$);\n");
    fprintf(file, "		\\node at (mid) {$%d$};\n", rect[i].number);
  }

  // Limit of the rectangle set
  fprintf(file, "\n");
  fprintf(file, "	%% Limit\n");
  fprintf(file,
          "	\\draw[ultra thick, dashed, blue] (%dmm, %dmm) -- (length) "
          "node[below] {$%d$};\n",
          maxLength, maxHeight, maxLength);  fprintf(file, "\n");
  fprintf(file, "\\end{tikzpicture}\n");
  fprintf(file, "\\end{figure}\n");

  fprintf(file, "\n");
  fprintf(file, "\n");
  fprintf(file, "\\LARGE{Tempo: %f}\n", time);
  fprintf(file, "\\end{document}\n");
}

void executeFiles(FILE *finp, FILE *file, const char *fileName) {
    int n, a, l;
    int maxHeight;
    clock_t start, end;
    double time;

    start = clock();
    
    // Leitura do arquivo de entrada
    fscanf(finp, "%d %d", &maxHeight, &n);

    // Alocar memória para os retângulos e bordas
    rectangleSet *rect = (rectangleSet *)malloc((n + 1) * sizeof(rectangleSet));
    bigRectangles *edge = (bigRectangles *)malloc((n + 1) * sizeof(bigRectangles));

    // Leitura dos retângulos
    for (int i = 1; i < n + 1; i++) {
        fscanf(finp, "%d %d", &rect[i].l, &rect[i].a);
        rect[i].number = i;
        rect[i].area = rect[i].l * rect[i].a;
    }

    quickSortLength(rect, 1, n);
    
    // Calcula o comprimento total
    int maxLength = 0;
    for (int i = 1; i <= n; i++) {
        maxLength += rect[i].l;
    }

    // Inicializa as posições dos retângulos
    for (int i = 1; i < n + 1; i++) {
        rect[i].x = 0;
        rect[i].y = 0;
    }

    // Calcula interseções
    for (int i = 2; i <= n; i++) {
        findIntersections(n, i, rect, edge, maxHeight, maxLength);
    }

    // Encontra o comprimento mínimo para o arquivo de saída
    int minX = minLength(n, rect);

    end = clock();
    time = ((double)(end - start)) / CLOCKS_PER_SEC;

    // Escreve os resultados no arquivo .tex
    tikzRectangles(n, rect, file, maxHeight, minX, 0, fileName);  // 0 para o tempo aqui, pois será passado na main

    printf("Resultado: %d\n", minX);
    // Libera a memória
    free(rect);
    free(edge);
}


int main() {
clock_t start, end;
    double time;

    start = clock();

    char pasta_entrada[] = "./inputs";   // Pasta onde estão os arquivos de entrada
    char pasta_saida[] = "./outputs_area";      // Pasta onde os arquivos de saída serão salvos

    // Criar a pasta de saída, se não existir
    struct stat st = {0};
    if (stat(pasta_saida, &st) == -1) {
        mkdir(pasta_saida, 0700);  // Permissões padrão para a pasta
    }

    DIR *dir = opendir(pasta_entrada);
    if (dir == NULL) {
        perror("Erro ao abrir a pasta de entrada");
        return 1;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {  // Verifica se é um arquivo regular
            // Caminho completo para o arquivo de entrada
            char caminho_entrada[512];
            snprintf(caminho_entrada, sizeof(caminho_entrada), "%s/%s", pasta_entrada, entry->d_name);

            char fileName[256];
            strncpy(fileName, entry->d_name, sizeof(fileName));

            // Caminho completo para o arquivo de saída (adicionando "_out.txt")
            char caminho_saida[512];
            snprintf(caminho_saida, sizeof(caminho_saida), "%s/%s_out2.tex", pasta_saida, entry->d_name);

            // Abrir arquivo de entrada
            FILE *finp = fopen(caminho_entrada, "r");
            if (finp == NULL) {
                perror("Erro ao abrir o arquivo de entrada");
                continue;
            }

            // Abrir arquivo de saída
            FILE *file = fopen(caminho_saida, "w");
            if (file == NULL) {
                perror("Erro ao criar o arquivo de saída");
                fclose(finp);
                continue;
            }

            // Processar o arquivo
            clock_t startEx, endEx;
            double executeTime;
            startEx = clock();
            executeFiles(finp, file, fileName);
            endEx = clock();

            executeTime = ((double)(endEx - startEx)) / CLOCKS_PER_SEC;
            // Fechar os arquivos
            fclose(finp);
            fclose(file);

            printf("Processado: %s -> %s | %.2f segundos\n", caminho_entrada, caminho_saida, executeTime);
        }
    }

    closedir(dir);

    end = clock();
    time = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Execution completed in %.2f seconds\n", time);

    return 0;
}
