# Projet HPC – Vectorisation AVX-512 & Parallélisme MPI

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
![Langages](https://img.shields.io/badge/C-73.3%25-blue) ![Langages](https://img.shields.io/badge/Python-26.7%25-green)
![Release](https://img.shields.io/badge/release-v1.0-brightgreen)


## Description générale
Ce projet s’inscrit dans le cadre du module **Architectures et Modèles de Calcul (M1 CHPS)**. Il illustre deux axes majeurs du calcul haute performance (HPC) :

1. **La vectorisation SIMD avec AVX-512** appliquée à la multiplication de matrices denses en C.
2. **Le parallélisme distribué avec MPI** à travers une estimation de \(\pi\) par la méthode de Monte‑Carlo en Python.

L’objectif est d’analyser les **gains de performance**, la **scalabilité** et la **précision numérique** obtenus grâce à ces optimisations.

---

## Partie 1 – Multiplication de matrices avec AVX‑512

### Objectifs
- Comparer une implémentation **naïve** et une version **vectorisée AVX‑512**
- Mesurer le **temps d’exécution**, le **speedup** et le **gain de performance**
- Vérifier la **validité numérique** des résultats

### Technologies utilisées
- Langage : **C**
- Instructions SIMD : **AVX‑512 (Intel)**
- Compilation : `gcc`
- Simulation AVX‑512 : **Intel SDE (sde64)**

### Compilation et exécution
```bash
gcc -O3 -mavx512f -march=native matmul_avx512.c -o matmul_avx512 -lm
sde64 -icl -- ./matmul_avx512
```

### Résultats obtenus

| Taille matrice | Naïve (s) | AVX‑512 (s) | Speedup |
|---------------|-----------|-------------|---------|
| 256×256 | 0.092 | 0.072 | 1.28× |
| 512×512 | 0.752 | 0.472 | 1.59× |
| 768×768 | 2.817 | 1.636 | 1.72× |
| 1024×1024 | 19.877 | 3.750 | **5.30×** |
| 1536×1536 | 62.406 | 12.517 | **4.99×** |

**Constat clé** : plus la taille des matrices augmente, plus la vectorisation AVX‑512 devient efficace.

### Statistiques globales
- **Speedup moyen** : 2.98×
- **Temps total gagné** : 67.5 secondes
- **Erreurs numériques** : 0 (résultats identiques)

Les résultats sont sauvegardés dans :
```text
resultats_matmul.txt
```

---

## Partie 2 – Calcul de π par Monte‑Carlo avec MPI

### Objectifs
- Implémenter un schéma **client–serveur MPI**
- Étudier l’impact du **nombre de processus** sur la convergence
- Évaluer la **précision** et le **débit de calcul**

### Technologies utilisées
- Langage : **Python 3**
- Bibliothèque : **mpi4py / MPI**
- Lancement : `mpirun`

### Exécution

#### Avec 2 processus
```bash
mpirun -np 2 python3 pi_monte_carlo.py
```

#### Avec 4 processus (oversubscribe)
```bash
mpirun --oversubscribe -np 4 python3 pi_monte_carlo.py
```

### Résultats – Exemple

#### Cas 2 processus
- Temps total : **3.46 s**
- Tirages totaux : 150 millions
- Débit : **43.3 millions tirages/s**
- Erreur absolue : **7.7 × 10⁻⁶**

#### Cas 4 processus
- Temps total : **95.02 s**
- Tirages totaux : 3.01 milliards
- Débit : **31.7 millions tirages/s**
- Erreur absolue : **4.0 × 10⁻⁵**

**Observation** : l’augmentation du nombre de processus améliore la convergence mais peut dégrader les performances en cas de sur‑allocation des ressources.

---

## Analyse et enseignements
- AVX‑512 offre des **gains spectaculaires** pour les calculs intensifs et réguliers
- Le parallélisme MPI est efficace mais dépend fortement de :
  - la charge par processus
  - la communication
  - l’architecture matérielle
- Une bonne optimisation HPC nécessite un **équilibre entre vectorisation et parallélisme**

---

## Structure du projet
```text
projet_avec_avx512/
├── matmul_avx512.c
├── pi_monte_carlo.py
├── resultats_matmul.txt
└── README.md
```

---

## Auteur
**Yaya Touré (Médoune)**  
Master 1 – Calcul Haute Performance et Simulation (CHPS)  

Test effectué le : **05 janvier 2026**

---

## Perspectives
- Ajout du **cache blocking** pour la multiplication matricielle
- Comparaison avec **OpenMP** et **BLAS (MKL)**
- Étude de la scalabilité MPI sur cluster

---

Ce projet constitue une base solide pour des travaux avancés en **HPC, vectorisation SIMD et calcul parallèle**.

