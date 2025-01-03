#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_GATE_ARRAY_SIZE 10

// numeration defining different types of logic gates
typedef enum { INPUT, OUTPUT, AND, OR, NOT, FLIPFLOP } GateType;

// Structure representing a logic gate
typedef struct Gate {
    GateType type;
    char name[20];                    // Gate identifier name
    struct Gate* input_gates[10];     // Array of pointers to input gates (linked)
    int number_of_inputs;             // Number of inputs connected
    int output;                       // Current output value
    int former_out;                   // for FLIPFLOP
    short evaluated;                  // Flag if gate has been evaluated
    int (*characteristic_function)(int*, int); // Function pointer for gate logic
} Gate;

// Function prototypes
Gate* create_gate(GateType type, const char* name);
void create_connection(Gate** input_gates, int input_count,
                       Gate** output_gates, int output_count,
                       Gate** other_gates, int other_count,
                       const char* from_name, const char* to_name);
int evaluate(Gate* gate);
void reset_gates(Gate** gates, int count);
int and_function(int* inputs, int n);
int or_function(int* inputs, int n);
int not_function(int* inputs, int n);
int flipflop_function(int* inputs, int former_out);



int main() {
    // Initialize dynamic arrays for different gate types
    Gate** input_gates = malloc(INITIAL_GATE_ARRAY_SIZE * sizeof(Gate*));
    Gate** output_gates = malloc(INITIAL_GATE_ARRAY_SIZE * sizeof(Gate*));
    Gate** other_gates = malloc(INITIAL_GATE_ARRAY_SIZE * sizeof(Gate*));
    int input_count = 0, output_count = 0, other_count = 0;
    int input_size = INITIAL_GATE_ARRAY_SIZE, output_size = INITIAL_GATE_ARRAY_SIZE, other_size = INITIAL_GATE_ARRAY_SIZE;

    // Open and read circuit definition file
    FILE* circuit_file = fopen("circuit.txt", "r");
    if (!circuit_file) {
        printf("Error opening circuit.txt");
        return 0;
    }

    // Process circuit definition file line by line
    char line[256];
    while (fgets(line, sizeof(line), circuit_file)) {
        char keyword[20], type_or_from[20], name_or_to[20];
        sscanf(line, "%s %s %s", keyword, type_or_from, name_or_to);
        
        if (strcmp(keyword, "GATE") == 0) {
            // Parse gate type and create new gate
            GateType temp_type;
            if (strcmp(type_or_from, "INPUT") == 0) temp_type = INPUT;
            else if (strcmp(type_or_from, "OUTPUT") == 0) temp_type = OUTPUT;
            else if (strcmp(type_or_from, "AND") == 0) temp_type = AND;
            else if (strcmp(type_or_from, "OR") == 0) temp_type = OR;
            else if (strcmp(type_or_from, "NOT") == 0) temp_type = NOT;
            else if (strcmp(type_or_from, "FLIPFLOP") == 0) temp_type = FLIPFLOP;
            else continue;

            Gate* new_gate = create_gate(temp_type, name_or_to);
        
            // Add gate if necessary
            if (temp_type == INPUT) {
                if (input_count == input_size) {
                    input_size *= 2;
                    input_gates = realloc(input_gates, input_size * sizeof(Gate*));
                }
                input_gates[input_count++] = new_gate;
            } else if (temp_type == OUTPUT) {
                if (output_count == output_size) {
                    output_size *= 2;
                    output_gates = realloc(output_gates, output_size * sizeof(Gate*));
                }
                output_gates[output_count++] = new_gate;
            } else {
                if (other_count == other_size) {
                    other_size *= 2;
                    other_gates = realloc(other_gates, other_size * sizeof(Gate*));
                }
                other_gates[other_count++] = new_gate;
            }
        
        } else if (strcmp(keyword, "CONNECTION") == 0) {
            // Create connection between gates
            create_connection(input_gates, input_count, output_gates, output_count, other_gates, other_count, type_or_from, name_or_to);
        }
    }

    fclose(circuit_file);

    // Open and read input file
    FILE* input_file = fopen("input.txt", "r");
    if (!input_file) {
        printf("Error opening input.txt");
        return 0;
    }

    // Process each line of input
    while (fgets(line, sizeof(line), input_file)) {
        // Reset all gates for new evaluation
        reset_gates(input_gates, input_count);
        reset_gates(output_gates, output_count);
        reset_gates(other_gates, other_count);

        // Set input values from input file
        for (int i = 0; i < input_count; i++) {
            input_gates[i]->output = line[i] - '0';
        }
        
        // Evaluate and print output values
        for (int i = 0; i < output_count; i++) {
            printf("%d", evaluate(output_gates[i]));
        }
        printf("\n");
    }

    fclose(input_file);

    // Free allocated memory
    free(input_gates);
    free(output_gates);
    free(other_gates);

    return 0;
}

// Creates a new gate of specified type with given name
Gate* create_gate(GateType type, const char* name) {
    Gate* new_gate = (Gate*)malloc(sizeof(Gate));
    new_gate->type = type;
    strncpy(new_gate->name, name, 20);
    new_gate->number_of_inputs = 0;
    new_gate->output = 0;
    new_gate->former_out = 0;
    new_gate->evaluated = 0;

    // Assign appropriate function pointer based on gate type
    switch (type) {
        case AND:
            new_gate->characteristic_function = and_function;
            break;
        case OR:
            new_gate->characteristic_function = or_function;
            break;
        case NOT:
            new_gate->characteristic_function = not_function;
            break;
        case FLIPFLOP:
            new_gate->characteristic_function = flipflop_function; 
            break;
        default:
            new_gate->characteristic_function = NULL;
    }

    return new_gate;
}

// Creates a connection between two gates
void create_connection(Gate** input_gates, int input_count,
                       Gate** output_gates, int output_count,
                       Gate** other_gates, int other_count,
                       const char* from_name, const char* to_name) {
    Gate* from_gate = NULL;
    Gate* to_gate = NULL;

    // Search for source gate
    for (int i = 0; i < input_count; i++) {
        if (strcmp(input_gates[i]->name, from_name) == 0) {
            from_gate = input_gates[i];
            break;
        }
    }
    for (int i = 0; i < output_count; i++) {
        if (strcmp(output_gates[i]->name, from_name) == 0) {
            from_gate = output_gates[i];
            break;
        }
    }
    for (int i = 0; i < other_count; i++) {
        if (strcmp(other_gates[i]->name, from_name) == 0) {
            from_gate = other_gates[i];
            break;
        }
    }

    // Search for destination gate
    for (int i = 0; i < input_count; i++) {
        if (strcmp(input_gates[i]->name, to_name) == 0) {
            to_gate = input_gates[i];
            break;
        }
    }
    for (int i = 0; i < output_count; i++) {
        if (strcmp(output_gates[i]->name, to_name) == 0) {
            to_gate = output_gates[i];
            break;
        }
    }
    for (int i = 0; i < other_count; i++) {
        if (strcmp(other_gates[i]->name, to_name) == 0) {
            to_gate = other_gates[i];
            break;
        }
    }

    // Create the connection if both gates are found and input limit not exceeded
    if (from_gate && to_gate && to_gate->number_of_inputs < 10) {
        to_gate->input_gates[to_gate->number_of_inputs++] = from_gate;
    } else {
        if (!from_gate) printf("Error: Source gate '%s' not found!\n", from_name);
        if (!to_gate) printf("Error: Destination gate '%s' not found!\n", to_name);
        if (to_gate && to_gate->number_of_inputs >= 10) {
            printf("Error: Destination gate '%s' has too many inputs!\n", to_gate->name);
        }
    }
}

// Evaluates the output of a gate recursively
int evaluate(Gate* gate) {
    // Return cached result if already evaluated
    if (gate->evaluated) {
        return gate->output;
    }

    // Input gates return their stored value
    if (gate->type == INPUT) {
        gate->evaluated = 1;
        return gate->output;
    }

    // Evaluate all input gates recursively
    int inputs[10];
    for (int i = 0; i < gate->number_of_inputs; i++) {
        inputs[i] = evaluate(gate->input_gates[i]);
    }
    
    // Calculate output based on gate type
    if (gate->type == OUTPUT && gate->number_of_inputs > 0) {
        gate->output = evaluate(gate->input_gates[0]);
    } else if (gate->characteristic_function) {
        gate->output = gate->characteristic_function(inputs, gate->number_of_inputs);
    }
    
    // Special if block for FLIPFLOP gates
    if (gate->type == FLIPFLOP) {
        int temp = flipflop_function(inputs, gate->former_out);
        gate->output = gate->former_out = temp;
        // Update connected output gates if any
        for (int i = 0; i < gate->number_of_inputs; i++) {
            if (gate->input_gates[i]->type == OUTPUT) {
                gate->input_gates[i]->output = gate->output;
            }
        }
    }

    gate->evaluated = 1;
    return gate->output;
}

// Resets evaluation state of gates for new simulation
void reset_gates(Gate** gates, int count) {
    for (int i = 0; i < count; i++) {
        gates[i]->evaluated = 0;
    }
}

// Logic function for AND gate
int and_function(int* inputs, int n) {
    for (int i = 0; i < n; i++) {
        if (!inputs[i]) {
            return 0;
        }
    }
    return 1;
}

// Logic function for OR gate
int or_function(int* inputs, int n) {
    for (int i = 0; i < n; i++) {
        if (inputs[i]) {
            return 1;
        }
    }
    return 0;
}

// Logic function for NOT gate
int not_function(int* inputs, int n) {
    return !inputs[0];
}

// Logic function for FLIPFLOP gate
int flipflop_function(int* inputs, int former_out) {
    int a = inputs[0];  // Current input
    int output = (a != former_out) ? 1 : 0;  // Output 1 if input differs from previous state
    return output;
}

void free_gate_list(Gate** gates, int count) {
    for (int i = 0; i < count; i++) {
        free(gates[i]);  // Her bir Gate nesnesinin belleğini serbest bırak
    }
    free(gates);  // Gate dizisini serbest bırak
}