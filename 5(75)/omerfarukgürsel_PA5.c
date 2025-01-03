#include <stdio.h>
#include <string.h> //for strcmp
#include <math.h> // for fabs

#define N 5 //number of languages
#define MAX_LANGUAGES 100
#define BIGRAM_COUNT 729


void create_bigrams(char bigrams[BIGRAM_COUNT][3]) {
    // All possible letters
    char letters[27] = "abcdefghijklmnopqrstuvwxyz ";
    int index = 0;

    // All combinations
    for(int i=0; i<27; i++) {
        for(int j=0; j<27; j++) {
            bigrams[index][0] = letters[i];
            bigrams[index][1] = letters[j];
            bigrams[index][2] = '\0';
            index++;
        }
    }
}

// Calculate frequency of each bigram in the given file
void calculate_freq(const char* filename, double freq[BIGRAM_COUNT], char bigrams[BIGRAM_COUNT][3]) {
    FILE *file = fopen(filename, "r");
    if(file == NULL) {
        printf("File open error\n");
        return;
    }

    // Reset freqs
    for(int i=0; i<BIGRAM_COUNT; i++) {
        freq[i] = 0.0;
    }

    int total_count = 0;  // Total number of bigrams found
    int prev ;
    int current;
    int flag = 0;

    // Read the file char by char
    while((current = fgetc(file)) != EOF) {
        // If statement not allows \n
        if(current >= 'a' && current <= 'z' || current == ' ') {
            // If statement not allows two spaces bigram
            if(!(current == ' ' && prev == ' ')){
                char current_bigram[3] = {prev, current, '\0'};

                // Beginning of file
                if(flag){
                    // Find and count bigram
                    for(int i=0; i<BIGRAM_COUNT; i++) {
                        if(strcmp(current_bigram, bigrams[i]) == 0) {
                            freq[i]++;
                            total_count++;
                            break;
                        }
                    }
                }
                else {
                    // Then starts finding bigrams
                    flag = 1;
                }
            }
            prev = current;
        }
    }

    // Count number to real frequencies
    for(int i=0; i<BIGRAM_COUNT; i++) {
        if(total_count > 0){
            freq[i] = freq[i] / total_count;
        }
        else{
            freq[i] = 0;
        }
    }

    fclose(file);
}


double dissimilarity(double freq1[BIGRAM_COUNT], double freq2[BIGRAM_COUNT]) {
    double total_diff = 0.0;

    for(int i=0; i<BIGRAM_COUNT; i++) {
        total_diff += fabs(freq1[i] - freq2[i]);
    }

    return total_diff;
}


int main() {

    // Create bigram list
    char bigrams[BIGRAM_COUNT][3];
    create_bigrams(bigrams);

    // Frequencies for language_x
    double ref_freq[BIGRAM_COUNT];
    calculate_freq("language_x.txt", ref_freq, bigrams);


    // Calculate dissimilarity for each language
    for(int i=1; i<=N; i++) {

        char filename[17];
        sprintf(filename, "language_%d.txt",i);

        double other_freq[BIGRAM_COUNT];
        calculate_freq(filename, other_freq, bigrams);

        // Calculate and print dissimilarity
        double diff = dissimilarity(ref_freq, other_freq);
        printf("%f\n", diff);
    }

    return 0;
}