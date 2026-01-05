// #include <stdio.h>
// #include <stdlib.h>
// #include <time.h>
// #include <immintrin.h>

// // 1. Version naïve de multiplication matricielle
// void matmul_naive(float *A, float *B, float *C, int n) {
//     for (int i = 0; i < n; i++) {
//         for (int j = 0; j < n; j++) {
//             float sum = 0.0f;
//             for (int k = 0; k < n; k++) {
//                 sum += A[i * n + k] * B[k * n + j];
//             }
//             C[i * n + j] = sum;
//         }
//     }
// }

// // 2. Version vectorisée AVX-512
// void matmul_avx512(float *A, float *B, float *C, int n) {
//     // Initialisation de C à zéro
//     for (int i = 0; i < n * n; i++) {
//         C[i] = 0.0f;
//     }
    
//     // Multiplication matricielle avec AVX-512
//     // Pour chaque ligne i de A
//     for (int i = 0; i < n; i++) {
//         // Pour chaque colonne k de A (ligne k de B)
//         for (int k = 0; k < n; k++) {
//             // Charger A[i][k] et le broadcaster dans un registre 512 bits
//             __m512 vA = _mm512_set1_ps(A[i * n + k]);
            
//             // Pour chaque bloc de 16 colonnes de B
//             for (int j = 0; j < n; j += 16) {
//                 // Charger 16 éléments de C[i][j:j+16]
//                 __m512 vC = _mm512_loadu_ps(&C[i * n + j]);
                
//                 // Charger 16 éléments de B[k][j:j+16]
//                 __m512 vB = _mm512_loadu_ps(&B[k * n + j]);
                
//                 // Accumulation avec FMA : C = A * B + C
//                 vC = _mm512_fmadd_ps(vA, vB, vC);
                
//                 // Stocker le résultat dans C
//                 _mm512_storeu_ps(&C[i * n + j], vC);
//             }
//         }
//     }
// }

// // Fonction pour initialiser une matrice avec des valeurs aléatoires
// void init_matrix(float *mat, int n) {
//     for (int i = 0; i < n * n; i++) {
//         mat[i] = (float)rand() / RAND_MAX;
//     }
// }

// // Fonction pour afficher une petite partie de la matrice
// void print_matrix(float *mat, int n, int max_display) {
//     int size = (n < max_display) ? n : max_display;
//     for (int i = 0; i < size; i++) {
//         for (int j = 0; j < size; j++) {
//             printf("%6.2f ", mat[i * n + j]);
//         }
//         printf("\n");
//     }
//     if (n > max_display) {
//         printf("... (matrice %dx%d)\n", n, n);
//     }
// }

// // Fonction pour vérifier la validité des résultats
// int verify_results(float *C1, float *C2, int n, float tolerance) {
//     int errors = 0;
//     for (int i = 0; i < n * n; i++) {
//         float diff = C1[i] - C2[i];
//         if (diff < 0) diff = -diff;
//         if (diff > tolerance) {
//             errors++;
//         }
//     }
//     return errors;
// }

// // Fonction pour tester une taille de matrice
// void test_size(int N, int show_details) {
//     printf("\n");
//     printf("========================================\n");
//     printf("  Test avec matrices %dx%d\n", N, N);
//     printf("========================================\n");
    
//     // Allocation des matrices
//     float *A = (float*)aligned_alloc(64, N * N * sizeof(float));
//     float *B = (float*)aligned_alloc(64, N * N * sizeof(float));
//     float *C_naive = (float*)aligned_alloc(64, N * N * sizeof(float));
//     float *C_avx512 = (float*)aligned_alloc(64, N * N * sizeof(float));
    
//     if (!A || !B || !C_naive || !C_avx512) {
//         printf("Erreur d'allocation mémoire pour N=%d\n", N);
//         return;
//     }
    
//     // Initialisation
//     srand(42);
//     init_matrix(A, N);
//     init_matrix(B, N);
    
//     if (show_details) {
//         printf("\nPremiers éléments de A:\n");
//         print_matrix(A, N, 16);
//         printf("\nPremiers éléments de B:\n");
//         print_matrix(B, N, 16);
//     }
    
//     // Version naïve
//     printf("\n--- Version naïve ---\n");
//     clock_t start = clock();
//     matmul_naive(A, B, C_naive, N);
//     clock_t end = clock();
//     double time_naive = (double)(end - start) / CLOCKS_PER_SEC;
//     printf("Temps: %.3f secondes\n", time_naive);
    
//     if (show_details) {
//         printf("\nPremiers éléments de C (naïve):\n");
//         print_matrix(C_naive, N, 16);
//     }
    
//     // Version AVX-512
//     printf("\n--- Version AVX-512 ---\n");
//     start = clock();
//     matmul_avx512(A, B, C_avx512, N);
//     end = clock();
//     double time_avx512 = (double)(end - start) / CLOCKS_PER_SEC;
//     printf("Temps: %.3f secondes\n", time_avx512);
    
//     if (show_details) {
//         printf("\nPremiers éléments de C (AVX-512):\n");
//         print_matrix(C_avx512, N, 16);
//     }
    
//     // Vérification
//     printf("\n--- Vérification ---\n");
//     int errors = verify_results(C_naive, C_avx512, N, 0.01f);
//     if (errors == 0) {
//         printf("[OK] Résultats identiques\n");
//     } else {
//         printf("[ATTENTION !] %d erreurs détectées!\n", errors);
//     }
    
//     // Performance
//     printf("\n--- Performance ---\n");
//     printf("Speedup: %.2fx\n", time_naive / time_avx512);
//     printf("Gain de temps: %.3f secondes (%.1f%%)\n", 
//            time_naive - time_avx512, 
//            100.0 * (time_naive - time_avx512) / time_naive);
    
//     // Libération
//     free(A);
//     free(B);
//     free(C_naive);
//     free(C_avx512);
// }

// int main() {
//     printf("=======================================================\n");
//     printf("  Multiplication de matrices avec AVX-512\n");
//     printf("  Tests avec différentes tailles de matrices\n");
//     printf("=======================================================\n");
    
//     // Tableau des tailles à tester (multiples de 16 pour AVX-512)
//     int sizes[] = {256, 512, 768, 1024, 1536};
//     int num_sizes = sizeof(sizes) / sizeof(sizes[0]);
    
//     // Tester chaque taille
//     for (int i = 0; i < num_sizes; i++) {
//         // Afficher les détails seulement pour la première taille
//         test_size(sizes[i], i == 0);
//     }
    
//     // Tableau récapitulatif
//     printf("\n\n");
//     printf("========================================\n");
//     printf("  RÉCAPITULATIF DES RÉSULTATS\n");
//     printf("========================================\n");
//     printf("%-10s | %-12s | %-12s | %-8s\n", 
//            "Taille", "Naïve (s)", "AVX-512 (s)", "Speedup");
//     printf("-----------|--------------|--------------|----------\n");
    
//     // Refaire les tests pour le tableau (rapide)
//     for (int i = 0; i < num_sizes; i++) {
//         int N = sizes[i];
        
//         float *A = (float*)aligned_alloc(64, N * N * sizeof(float));
//         float *B = (float*)aligned_alloc(64, N * N * sizeof(float));
//         float *C_naive = (float*)aligned_alloc(64, N * N * sizeof(float));
//         float *C_avx512 = (float*)aligned_alloc(64, N * N * sizeof(float));
        
//         if (!A || !B || !C_naive || !C_avx512) continue;
        
//         srand(42);
//         init_matrix(A, N);
//         init_matrix(B, N);
        
//         clock_t start = clock();
//         matmul_naive(A, B, C_naive, N);
//         clock_t end = clock();
//         double time_naive = (double)(end - start) / CLOCKS_PER_SEC;
        
//         start = clock();
//         matmul_avx512(A, B, C_avx512, N);
//         end = clock();
//         double time_avx512 = (double)(end - start) / CLOCKS_PER_SEC;
        
//         printf("%-10dx%-3d | %12.3f | %12.3f | %8.2fx\n", 
//                N, N, time_naive, time_avx512, time_naive / time_avx512);
        
//         free(A);
//         free(B);
//         free(C_naive);
//         free(C_avx512);
//     }
    
//     printf("========================================\n");
//     printf("\nNote: Plus les matrices sont grandes, plus le speedup est important!\n");
    
//     return 0;
// }


//===========================================================================================================


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <immintrin.h>

// 1. Version naïve de multiplication matricielle
void matmul_naive(float *A, float *B, float *C, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            float sum = 0.0f;
            for (int k = 0; k < n; k++) {
                sum += A[i * n + k] * B[k * n + j];
            }
            C[i * n + j] = sum;
        }
    }
}

// 2. Version vectorisée AVX-512
void matmul_avx512(float *A, float *B, float *C, int n) {
    // Initialisation de C à zéro
    for (int i = 0; i < n * n; i++) {
        C[i] = 0.0f;
    }
    
    // Multiplication matricielle avec AVX-512
    for (int i = 0; i < n; i++) {
        for (int k = 0; k < n; k++) {
            __m512 vA = _mm512_set1_ps(A[i * n + k]);
            
            for (int j = 0; j < n; j += 16) {
                __m512 vC = _mm512_loadu_ps(&C[i * n + j]);
                __m512 vB = _mm512_loadu_ps(&B[k * n + j]);
                vC = _mm512_fmadd_ps(vA, vB, vC);
                _mm512_storeu_ps(&C[i * n + j], vC);
            }
        }
    }
}

// Fonction pour initialiser une matrice avec des valeurs aléatoires
void init_matrix(float *mat, int n) {
    for (int i = 0; i < n * n; i++) {
        mat[i] = (float)rand() / RAND_MAX;
    }
}

// Fonction pour afficher une petite partie de la matrice (terminal ET fichier)
void print_matrix(FILE *fp, float *mat, int n, int max_display) {
    int size = (n < max_display) ? n : max_display;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            printf("%6.2f ", mat[i * n + j]);
            if (fp) fprintf(fp, "%6.2f ", mat[i * n + j]);
        }
        printf("\n");
        if (fp) fprintf(fp, "\n");
    }
    if (n > max_display) {
        printf("... (matrice %dx%d)\n", n, n);
        if (fp) fprintf(fp, "... (matrice %dx%d)\n", n, n);
    }
}

// Fonction pour vérifier la validité des résultats
int verify_results(float *C1, float *C2, int n, float tolerance) {
    int errors = 0;
    for (int i = 0; i < n * n; i++) {
        float diff = C1[i] - C2[i];
        if (diff < 0) diff = -diff;
        if (diff > tolerance) {
            errors++;
        }
    }
    return errors;
}

// Structure pour stocker les résultats d'un test
typedef struct {
    int size;
    double time_naive;
    double time_avx512;
    double speedup;
    int errors;
} TestResult;

// Macro pour afficher à la fois sur terminal et fichier
#define PRINT_BOTH(fp, ...) do { \
    printf(__VA_ARGS__); \
    if (fp) fprintf(fp, __VA_ARGS__); \
} while(0)

// Fonction pour tester une taille de matrice
TestResult test_size(FILE *fp, int N, int show_details) {
    TestResult result = {N, 0.0, 0.0, 0.0, 0};
    
    PRINT_BOTH(fp, "\n");
    PRINT_BOTH(fp, "========================================\n");
    PRINT_BOTH(fp, "  Test avec matrices %dx%d\n", N, N);
    PRINT_BOTH(fp, "========================================\n");
    
    // Allocation des matrices
    float *A = (float*)aligned_alloc(64, N * N * sizeof(float));
    float *B = (float*)aligned_alloc(64, N * N * sizeof(float));
    float *C_naive = (float*)aligned_alloc(64, N * N * sizeof(float));
    float *C_avx512 = (float*)aligned_alloc(64, N * N * sizeof(float));
    
    if (!A || !B || !C_naive || !C_avx512) {
        PRINT_BOTH(fp, "Erreur d'allocation mémoire pour N=%d\n", N);
        return result;
    }
    
    // Initialisation
    srand(42);
    init_matrix(A, N);
    init_matrix(B, N);
    
    if (show_details) {
        PRINT_BOTH(fp, "\nPremiers éléments de A:\n");
        print_matrix(fp, A, N, 16);
        PRINT_BOTH(fp, "\nPremiers éléments de B:\n");
        print_matrix(fp, B, N, 16);
    }
    
    // Version naïve
    PRINT_BOTH(fp, "\n--- Version naïve ---\n");
    clock_t start = clock();
    matmul_naive(A, B, C_naive, N);
    clock_t end = clock();
    result.time_naive = (double)(end - start) / CLOCKS_PER_SEC;
    PRINT_BOTH(fp, "Temps: %.3f secondes\n", result.time_naive);
    
    if (show_details) {
        PRINT_BOTH(fp, "\nPremiers éléments de C (naïve):\n");
        print_matrix(fp, C_naive, N, 16);
    }
    
    // Version AVX-512
    PRINT_BOTH(fp, "\n--- Version AVX-512 ---\n");
    start = clock();
    matmul_avx512(A, B, C_avx512, N);
    end = clock();
    result.time_avx512 = (double)(end - start) / CLOCKS_PER_SEC;
    PRINT_BOTH(fp, "Temps: %.3f secondes\n", result.time_avx512);
    
    if (show_details) {
        PRINT_BOTH(fp, "\nPremiers éléments de C (AVX-512):\n");
        print_matrix(fp, C_avx512, N, 16);
    }
    
    // Vérification
    PRINT_BOTH(fp, "\n--- Vérification ---\n");
    result.errors = verify_results(C_naive, C_avx512, N, 0.01f);
    if (result.errors == 0) {
        PRINT_BOTH(fp, "[OK] Résultats identiques\n");
    } else {
        PRINT_BOTH(fp, "[ATTENTION !] %d erreurs détectées!\n", result.errors);
    }
    
    // Performance
    result.speedup = result.time_naive / result.time_avx512;
    PRINT_BOTH(fp, "\n--- Performance ---\n");
    PRINT_BOTH(fp, "Speedup: %.2fx\n", result.speedup);
    PRINT_BOTH(fp, "Gain de temps: %.3f secondes (%.1f%%)\n", 
           result.time_naive - result.time_avx512, 
           100.0 * (result.time_naive - result.time_avx512) / result.time_naive);
    
    // Libération
    free(A);
    free(B);
    free(C_naive);
    free(C_avx512);
    
    return result;
}

int main() {
    // Ouvrir le fichier de sortie
    FILE *fp = fopen("resultats_matmul.txt", "w");
    if (!fp) {
        printf("Erreur: impossible de créer le fichier resultats_matmul.txt\n");
        return 1;
    }
    
    // En-tête
    PRINT_BOTH(fp, "=======================================================\n");
    PRINT_BOTH(fp, "  Multiplication de matrices avec AVX-512\n");
    PRINT_BOTH(fp, "  Tests avec différentes tailles de matrices\n");
    PRINT_BOTH(fp, "=======================================================\n");
    
    // Tableau des tailles à tester (multiples de 16 pour AVX-512)
    int sizes[] = {256, 512, 768, 1024, 1536};
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);
    
    // Stocker les résultats
    TestResult results[num_sizes];
    
    // Tester chaque taille
    for (int i = 0; i < num_sizes; i++) {
        results[i] = test_size(fp, sizes[i], i == 0);
    }
    
    // Tableau récapitulatif
    PRINT_BOTH(fp, "\n\n");
    PRINT_BOTH(fp, "========================================\n");
    PRINT_BOTH(fp, "  RÉCAPITULATIF DES RÉSULTATS\n");
    PRINT_BOTH(fp, "========================================\n");
    PRINT_BOTH(fp, "%-10s | %-12s | %-12s | %-8s | %-10s\n", 
           "Taille", "Naïve (s)", "AVX-512 (s)", "Speedup", "Erreurs");
    PRINT_BOTH(fp, "-----------|--------------|--------------|----------|------------\n");
    
    for (int i = 0; i < num_sizes; i++) {
        PRINT_BOTH(fp, "%-10dx%-3d | %12.3f | %12.3f | %8.2fx | %10d\n", 
               results[i].size, results[i].size, 
               results[i].time_naive, 
               results[i].time_avx512, 
               results[i].speedup,
               results[i].errors);
    }
    
    PRINT_BOTH(fp, "========================================\n");
    PRINT_BOTH(fp, "\nNote: Plus les matrices sont grandes, plus le speedup est important!\n");
    
    // Statistiques globales
    PRINT_BOTH(fp, "\n");
    PRINT_BOTH(fp, "========================================\n");
    PRINT_BOTH(fp, "  STATISTIQUES GLOBALES\n");
    PRINT_BOTH(fp, "========================================\n");
    
    double avg_speedup = 0.0;
    double total_time_saved = 0.0;
    
    for (int i = 0; i < num_sizes; i++) {
        avg_speedup += results[i].speedup;
        total_time_saved += (results[i].time_naive - results[i].time_avx512);
    }
    avg_speedup /= num_sizes;
    
    PRINT_BOTH(fp, "Speedup moyen: %.2fx\n", avg_speedup);
    PRINT_BOTH(fp, "Temps total gagné: %.3f secondes\n", total_time_saved);
    
    // Date et heure du test
    time_t now = time(NULL);
    PRINT_BOTH(fp, "\nTest effectué le: %s", ctime(&now));
    
    fclose(fp);
    
    printf("\n[OK] Résultats également sauvegardés dans: resultats_matmul.txt\n");
    
    return 0;
}