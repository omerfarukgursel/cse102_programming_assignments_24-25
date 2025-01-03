#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_MODULES 100        
#define MAX_MODULE_NAME 30     
#define MAX_DEFINITION 3000    

// Store information about a module
typedef struct {
    double coeff;              // Number multiplies the module
    char ident[MAX_MODULE_NAME];  // Name of the module
} module_t;

// Ffunction to calculate the value of a module
double evaluation(char* name, module_t* all_modules, int* module_count) {

    printf("> Define %s: \n> ", name);
    
    // Store the module's definition
    char def[MAX_DEFINITION];

    fgets(def, sizeof(def), stdin);
    // Remove the newline at the end of the input
    def[strcspn(def, "\n")] = 0;
    
    // Array to store parts of the module definition
    module_t array_of_modules[MAX_MODULES];
    // How many parts found
    int number_of_terms = 0;
    
    // Break the definition into pieces
    char* token = strtok(def, "+");
    while (token != NULL) {
        // Remove any spaces at the start each piece
        while (*token == ' ') token++;
        
        // Module name or not
        char* star = strchr(token, '*');
        if (star) {
            // This is a module with a coefficient
            *star = '\0';  // Split string
            // Store number
            array_of_modules[number_of_terms].coeff = atof(token); // atoi--int  atof--float
            // Store module name 
            strcpy(array_of_modules[number_of_terms].ident, star + 1);
        } else {
            // Simple number
            array_of_modules[number_of_terms].coeff = atof(token);
            // No module name
            array_of_modules[number_of_terms].ident[0] = '\0';
        }
        
        // Move to the next piece
        number_of_terms++;
        token = strtok(NULL, "+");
    }
    
    // Calculate the total value
    double sum = 0;
    for (int i = 0; i < number_of_terms; i++) {
        if (array_of_modules[i].ident[0] != '\0') {
            // (Recursive part) if there is a module in formula find each modules formulas recursively
            double r = evaluation(array_of_modules[i].ident, all_modules, module_count);
            sum += r * array_of_modules[i].coeff;
        } else {
            // If just a number add directly
            sum += array_of_modules[i].coeff;
        }
    }
    
    return sum;
}

int main() {

    module_t all_modules[MAX_MODULES];

    int module_count = 0;
    
    char module_name[MAX_MODULE_NAME];
    
    printf("> Module name?: ");
    scanf("%29s", module_name);
    getchar();  // Remove the extra newline
    
    double result = evaluation(module_name, all_modules, &module_count);

    printf("> %.2lf\n", result);
    
    return 0;
}