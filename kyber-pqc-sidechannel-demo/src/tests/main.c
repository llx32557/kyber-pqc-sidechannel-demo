#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timing_collect.h"
#include "tvla_collect.h"

int main(int argc, char *argv[]) {
    printf("=========================================\n");
    printf("  Kyber PQC Side-Channel Demo Launcher   \n");
    printf("=========================================\n");

    const char *mode = "timing";             // "timing" 或 "tvla"
    const char *output_file = "datasets/timing_data.csv";
    int iterations = 100000;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--tvla") == 0) {
            mode = "tvla";
            output_file = "datasets/tvla_data.csv";
        } else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            output_file = argv[++i];
        } else if (strcmp(argv[i], "-n") == 0 && i + 1 < argc) {
            iterations = atoi(argv[++i]);
        }
    }

    if (strcmp(mode, "tvla") == 0) {
        printf("[*] Mode: TVLA (Fixed vs Random)\n");
        run_tvla_collection(output_file, iterations);
    } else {
        printf("[*] Mode: Timing Collection\n");
        run_timing_collection(output_file, iterations);
    }

    printf("[*] Done.\n");
    return 0;
}
