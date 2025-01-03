#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LABELS 127

// Reading txt file
int** read_txt(int* rows, int* cols, char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error opening file!\n");
        return 0;
    }
    fscanf(file, "%d %d", rows, cols);
    int** grid = (int**)malloc(*rows * sizeof(int*));
    for (int i = 0; i < *rows; i++) {
        grid[i] = (int*)malloc(*cols * sizeof(int));
        for (int j = 0; j < *cols; j++) {
            fscanf(file, "%d", &grid[i][j]);
        }
    }
    fclose(file);
    return grid;
}
// Reading ppm file
int** read_ppm(int* rows, int* cols, char* filename){
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error opening file!\n");
        return 0;
    }
    // P1 P3 formats
    char format[3];
    fscanf(file, "%s", format);
    if (strcmp(format, "P1") != 0) {
        printf("Invalid PPM format! Must be P1.\n");
        fclose(file);
        return 0;
    }
    // Opposite queue
    fscanf(file, "%d %d", cols, rows);
    // Dynamically define
    int** grid = (int**)malloc(*rows * sizeof(int*));
    for (int i = 0; i < *rows; i++) {
        grid[i] = (int*)malloc(*cols * sizeof(int));
        for (int j = 0; j < *cols; j++) {
            int val;
            fscanf(file, "%d", &val);
            // Turning 0's to black and 1's to white
            grid[i][j] = (val == 0) ? 1 : 0;  // 0 -> black (1), 1 -> white (0)
        }
    }
    fclose(file);
    return grid;
}
// Function to write txt to output file
void write_grid(const char* filename, int** grid, int rows, int cols) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        printf("Error opening output file\n");
        exit(1);
    }
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            fprintf(file, "%d", grid[i][j]);
            if (j < cols - 1) fprintf(file, " ");
        }
        fprintf(file, "\n");
    }
    fclose(file);
}
// Function to write ppm to output file
void write_ppm(const char* filename, int** grid, int rows, int cols) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        printf("Error opening PPM file\n");
        exit(1);
    }
    fprintf(file, "P3\n%d %d\n255\n", cols, rows);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            int color = grid[i][j] * 41 % 256;
            int red = (color * 3) % 256;
            int green = (color * 5) % 256;
            int blue = (color * 7) % 256;
            fprintf(file, "%d %d %d ", red, green, blue);
        }
        fprintf(file, "\n");
    }
    fclose(file);
}

int find_root(int* labels, int x) {
    // Recursively find the root of the label
    if (labels[x] != x) {
        labels[x] = find_root(labels, labels[x]);
    }
    return labels[x];
}

void union_labels(int* labels, int a, int b) {
    // Non zero two labels finding their roots and linking them
    int rootA = find_root(labels, a);
    int rootB = find_root(labels, b);
    if (rootA != rootB) {
        // Smaller root becomes label number
        if (rootA < rootB) {
            labels[rootB] = rootA;
        } else {
            labels[rootA] = rootB;
        }
    }
}

void normalize_labels(int* labels, int** output, int rows, int cols) {
    int* new_labels = (int*)calloc(rows * cols / 2, sizeof(int));
    int current_label = 1;

    for (int i = 1; i < rows*cols/2; i++) {
        if (labels[i] != 0) {
            int root = find_root(labels, i);
            // Assign new label if it hasn't been assigned yet
            if (new_labels[root] == 0) {
                new_labels[root] = current_label++;
            }
            labels[i] = new_labels[root];
        }
    }
    // Update output grid with new labels
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (output[i][j] > 0) {
                output[i][j] = labels[output[i][j]];
            }
        }
    }
    free(new_labels);
}

void labeling(int** grid, int** output, int rows, int cols) {
    int* labels = (int*)malloc((rows * cols / 2) * sizeof(int));
    int next_label = 1;
    // Initialize labels array
    for (int i = 0; i < rows * cols / 2; i++) {
        labels[i] = i;
    }
    // Pass or assign the values
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (grid[i][j] == 1) {
                int left = (j > 0) ? output[i][j - 1] : 0;
                int top = (i > 0) ? output[i - 1][j] : 0;

                if (left > 0 && top > 0) {
                    output[i][j] = left;
                    // If left and top non zero compare the values
                    union_labels(labels, left, top);
                } else if (left > 0) {
                    output[i][j] = left;
                } else if (top > 0) {
                    output[i][j] = top;
                } else {
                    // If left and top zero or not exist this is new label
                    output[i][j] = next_label;
                    labels[next_label] = next_label;
                    next_label++;
                }
            }
        }
    }

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (output[i][j] > 0) {
                output[i][j] = find_root(labels, output[i][j]);
            }
        }
    }
    // Normalize labels for output
    normalize_labels(labels, output, rows, cols);

    free(labels);
}

int main() {
    int rows, cols;

    // Process input.txt to output.txt
    int** inputGrid_txt = read_txt(&rows, &cols, "input.txt");
    if (inputGrid_txt) {
        int** outputGrid_txt = (int**)malloc(rows * sizeof(int*));
        for (int i = 0; i < rows; i++) {
            outputGrid_txt[i] = (int*)calloc(cols, sizeof(int));
        }
        labeling(inputGrid_txt, outputGrid_txt, rows, cols);
        write_grid("output.txt", outputGrid_txt, rows, cols);
        for (int i = 0; i < rows; i++) {
            free(inputGrid_txt[i]);
            free(outputGrid_txt[i]);
        }
        free(inputGrid_txt);
        free(outputGrid_txt);
    }

    // Process input.ppm to output.ppm
    int** inputGrid_ppm = read_ppm(&rows, &cols, "input.ppm");
    if (inputGrid_ppm) {
        int** outputGrid_ppm = (int**)malloc(rows * sizeof(int*));
        for (int i = 0; i < rows; i++) {
            outputGrid_ppm[i] = (int*)calloc(cols, sizeof(int));
        }
        labeling(inputGrid_ppm, outputGrid_ppm, rows, cols);
        write_ppm("output.ppm", outputGrid_ppm, rows, cols);
        for (int i = 0; i < rows; i++) {
            free(inputGrid_ppm[i]);
            free(outputGrid_ppm[i]);
        }
        free(inputGrid_ppm);
        free(outputGrid_ppm);
    }

    return 0;
}
