#include <stdio.h>
#include <string.h>
#include <math.h>

#define MAX_VALUES 100
#define MAX_POLY_LENGTH 100

void evaluatePolynomials(FILE *valuesFile, FILE *polynomialsFile, FILE *evaluationsFile) {
    double values[MAX_VALUES][2];
    char polynomial[MAX_POLY_LENGTH];
    int numValues = 0;

    // Read (x, y) values from values.txt
    while (fscanf(valuesFile, "%lf %lf", &values[numValues][0], &values[numValues][1]) == 2) {
        numValues++;
    }

    // Process each polynomial
    while (fgets(polynomial, MAX_POLY_LENGTH, polynomialsFile) != NULL) {
        double coefficient, result;
        int x_power, y_power;

        // Evaluate for each (x, y) pair
        for (int i = 0; i < numValues; i++) {
            result = 0.0;
            coefficient = 1.0; // Default coefficient to 1
            x_power = 0;
            y_power = 0;
            int sign = 1;
            int term_started = 0;

            // Parse each character in the polynomial
            for (int j = 0; polynomial[j] != '\0'; j++) {
                if (polynomial[j] == '+' || polynomial[j] == '-') {
                    // If a term was started, calculate its value
                    if (term_started) {
                        result += sign * coefficient * pow(values[i][0], x_power) * pow(values[i][1], y_power);
                        coefficient = 1.0; // Reset to 1 for default
                        x_power = 0;
                        y_power = 0;
                        term_started = 0;
                    }
                    sign = (polynomial[j] == '-') ? -1 : 1; //- or + operator
                } else if (polynomial[j] >= '0' && polynomial[j] <= '9') {
                    // Parse coefficient
                    sscanf(&polynomial[j], "%lf", &coefficient);
                    while ((polynomial[j] >= '0' && polynomial[j] <= '9') || polynomial[j] == '.') j++;
                    j--; // index the loop will increment it
                    term_started = 1;
                } else if (polynomial[j] == 'x') {
                    // Parse x power if present
                    if (polynomial[j + 1] == '^') {
                        j += 2;
                        sscanf(&polynomial[j], "%d", &x_power);
                        while (polynomial[j] >= '0' && polynomial[j] <= '9') j++;
                        j--;
                    } else {
                        x_power = 1; // Only "x" present, x^1
                    }
                    term_started = 1;
                } else if (polynomial[j] == 'y') {
                    // Parse y power if present
                    if (polynomial[j + 1] == '^') {
                        j += 2;
                        sscanf(&polynomial[j], "%d", &y_power);
                        while (polynomial[j] >= '0' && polynomial[j] <= '9') j++;
                        j--;
                    } else {
                        y_power = 1; // Only "y" present, y^1
                    }
                    term_started = 1;
                }
            }

            // Add the last term if there was one
            if (term_started) {
                result += sign * coefficient * pow(values[i][0], x_power) * pow(values[i][1], y_power);
            }

            // Output the result for each (x, y) pair
            fprintf(evaluationsFile, "%.2f ", result);
            printf("Result for (x=%.2f  y=%.2f): %.2f\n", values[i][0], values[i][1], result);
        }
        fprintf(evaluationsFile, "\n");
    }
}

int main() {
    FILE *valuesFile, *polynomialsFile, *evaluationsFile;

    // Open input files
    valuesFile = fopen("values.txt", "r");
    polynomialsFile = fopen("polynomials.txt", "r");

    // Open output file
    evaluationsFile = fopen("evaluations.txt", "w");

    // Check if files are opened successfully
    if (valuesFile == NULL || polynomialsFile == NULL || evaluationsFile == NULL) {
        printf("Error opening files.\n");
        return 1;
    }

    // Evaluate polynomials and write results to evaluations.txt
    evaluatePolynomials(valuesFile, polynomialsFile, evaluationsFile);

    // Close files
    fclose(valuesFile);
    fclose(polynomialsFile);
    fclose(evaluationsFile);

    return 0;
}
