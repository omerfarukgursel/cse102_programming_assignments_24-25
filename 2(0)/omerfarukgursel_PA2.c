#include <stdio.h>
#include <stdlib.h>

#define X 2  // Segment size
#define Y 25 // Maximum number of segments to remember

// Function to shift the window
void shiftWindow(int *window, int new_num) {
    for (int i = 0; i < X - 1; i++) {
        window[i] = window[i + 1];
    }
    window[X - 1] = new_num;
}

// Function to check if a segment has been seen before
int findSegmentIndex(int segments[Y][X], int numSegments, int *window) {
    for (int i = 0; i < numSegments; i++) {
        int isSame = 1;
        for (int j = 0; j < X; j++) {
            if (segments[i][j] != window[j]) {
                isSame = 0;
                break;
            }
        }
        if (isSame) {
            return i;
        }
    }
    return -1;
}

int main() {
    FILE *inputFile, *outputFile;
    int window[X];  // Sliding window of size X
    int segments[Y][X];  // Array to store Y segments
    int counts[Y] = {0};  // Array to hold the count of each segment

    // Open input.txt file
    inputFile = fopen("input.txt", "r");
    if (!inputFile) {
        printf("Failed to open input.txt!\n");
        return 1;
    }

    // Determine the file size using fseek and ftell
    fseek(inputFile, 0, SEEK_END);
    long fileSize = ftell(inputFile);
    fseek(inputFile, 0, SEEK_SET); // Return to the beginning of the file

    int numSegments = 0;  // Number of segments being tracked
    int num, index = 0;   // Variables to read numbers from the file

    // Initialize the window
    for (int i = 0; i < X; i++) {
        window[i] = 0;
    }

    // Initialize segments
    for (int i = 0; i < Y; i++) {
        for (int j = 0; j < X; j++) {
            segments[i][j] = 0;
        }
    }

    // Read numbers from the file and process segments
    while (fscanf(inputFile, "%d", &num) == 1) {
        // Shift the window and add the new number
        shiftWindow(window, num);

        // Skip the first X-1 numbers (until a full segment)
        if (++index < X) continue;

        // Find the current window segment
        int segmentIndex = findSegmentIndex(segments, numSegments, window);

        if (segmentIndex != -1) {
            // If segment found, increment count
            counts[segmentIndex]++;
        } else {
            // If the segment is new, replace the oldest segment
            int oldestIndex = index % Y;
            for (int j = 0; j < X; j++) {
                segments[oldestIndex][j] = window[j];
            }
            counts[oldestIndex] = 1;
            if (numSegments < Y) numSegments++;
        }
        
    }

    fclose(inputFile);

    // Write output.txt file
    outputFile = fopen("output.txt", "w");
    if (!outputFile) {
        printf("Failed to open output.txt!\n");
        return 1;
    }

    // Write segments and their counts to the file
    for (int i = 0; i < Y && counts[i] > 0; i++) {
        for (int j = 0; j < X; j++) {
            fprintf(outputFile, "%d ", segments[i][j]);
        }
        fprintf(outputFile, ": %d\n", counts[i]);
    }

    fclose(outputFile);
    return 0;
}

