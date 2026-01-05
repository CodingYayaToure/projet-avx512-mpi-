#include <stdio.h>
#include <immintrin.h> // pour les intrinsics AVX‑512

int main() {
    // Vecteurs de 16 entiers 32 bits (16 × 32 = 512 bits)
    __m512i a = _mm512_set_epi32(
        16,15,14,13,12,11,10, 9,
         8, 7, 6, 5, 4, 3, 2, 1);
    __m512i b = _mm512_set1_epi32(100); // toutes les composantes = 100

    // Addition vectorielle
    __m512i c = _mm512_add_epi32(a, b);

    // Extraire et afficher les résultats
    int results[16];
    _mm512_storeu_si512(results, c);

    printf("Result of AVX-512 vector addition:\n");
    for(int i = 0; i < 16; i++) {
        printf("element %2d = %d\n", i, results[i]);
    }

    return 0;
}
