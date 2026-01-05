#!/usr/bin/env python3
"""
Calcul de π par méthode Monte-Carlo avec architecture client/serveur MPI
"""

from mpi4py import MPI
import numpy as np
import time

# Configuration
SAMPLE_SIZE = 10_000_000  # 10 millions de tirages par paquet
TARGET_PRECISION = 1e-5   # Précision cible pour arrêter
MAX_SAMPLES = 100         # Nombre maximum d'échantillons par client
CHECK_INTERVAL = 10       # Vérifier la convergence tous les N échantillons

def compute_pi_sample(n_points):
    """
    Calcule un échantillon de π en tirant n_points points aléatoires.
    
    Returns:
        (n_in, n_total) : nombre de points dans le cercle et nombre total
    """
    # Génération de points aléatoires dans [0,1]×[0,1]
    x = np.random.uniform(0, 1, n_points)
    y = np.random.uniform(0, 1, n_points)
    
    # Test si les points sont dans le cercle : x² + y² < 1
    distances_squared = x**2 + y**2
    n_in = np.sum(distances_squared < 1.0)
    
    return (n_in, n_points)


def server_process(comm, size):
    """
    Processus serveur (rank 0) qui collecte les résultats.
    """
    print("=" * 60)
    print("SERVEUR : Démarrage du calcul de π par Monte-Carlo")
    print(f"Nombre de clients : {size - 1}")
    print(f"Taille d'échantillon : {SAMPLE_SIZE:,} tirages")
    print("=" * 60)
    
    # Variables d'accumulation
    total_n_in = 0
    total_n_total = 0
    samples_received = 0
    pi_estimates = []
    
    start_time = time.time()
    active_clients = size - 1
    
    while active_clients > 0:
        # Recevoir un résultat d'un client
        status = MPI.Status()
        data = comm.recv(source=MPI.ANY_SOURCE, tag=MPI.ANY_TAG, status=status)
        client_rank = status.Get_source()
        
        n_in_local, n_total_local = data
        
        # Accumuler les résultats
        total_n_in += n_in_local
        total_n_total += n_total_local
        samples_received += 1
        
        # Estimer π
        pi_estimate = 4.0 * total_n_in / total_n_total
        pi_estimates.append(pi_estimate)
        
        elapsed = time.time() - start_time
        error = abs(pi_estimate - np.pi)
        
        # Affichage périodique
        if samples_received % CHECK_INTERVAL == 0:
            print(f"\n[t={elapsed:.2f}s] Échantillons: {samples_received}, "
                  f"Tirages totaux: {total_n_total:,}")
            print(f"  π estimé = {pi_estimate:.8f}")
            print(f"  π réel   = {np.pi:.8f}")
            print(f"  Erreur   = {error:.8f}")
        
        # Décider si on continue ou on arrête
        should_continue = True
        
        # Condition d'arrêt : précision atteinte après un minimum d'échantillons 
        if samples_received >= CHECK_INTERVAL and error < TARGET_PRECISION:
            should_continue = False
            print(f"\n[OK] Précision cible atteinte ! (erreur < {TARGET_PRECISION})")
        
        # Ou nombre maximum d'échantillons atteint
        if samples_received >= MAX_SAMPLES * (size - 1):
            should_continue = False
            print(f"\n[ATTENTION !] Nombre maximum d'échantillons atteint")
        
        # Envoyer la commande au client
        if should_continue:
            comm.send("CONTINUE", dest=client_rank, tag=1)
        else:
            comm.send("STOP", dest=client_rank, tag=1)
            active_clients -= 1
            print(f"  -> Client {client_rank} arrêté ({active_clients} restants)")
    
    # Résultats finaux
    total_time = time.time() - start_time
    final_pi = 4.0 * total_n_in / total_n_total
    final_error = abs(final_pi - np.pi)
    
    print("\n" + "=" * 60)
    print("RÉSULTATS FINAUX")
    print("=" * 60)
    print(f"Temps total          : {total_time:.2f} secondes")
    print(f"Échantillons reçus   : {samples_received}")
    print(f"Tirages totaux       : {total_n_total:,}")
    print(f"Tirages/seconde      : {total_n_total/total_time:,.0f}")
    print(f"\nπ estimé             : {final_pi:.10f}")
    print(f"π réel               : {np.pi:.10f}")
    print(f"Erreur absolue       : {final_error:.10f}")
    print(f"Erreur relative      : {final_error/np.pi*100:.6f}%")
    print("=" * 60)
    
    # Statistiques de convergence
    if len(pi_estimates) > 10:
        convergence = np.abs(np.diff(pi_estimates[-10:]))
        print(f"\nConvergence (10 derniers échantillons):")
        print(f"  Variation moyenne  : {np.mean(convergence):.8f}")
        print(f"  Variation max      : {np.max(convergence):.8f}")


def client_process(comm, rank):
    """
    Processus client qui calcule des échantillons de π.
    """
    print(f"[Client {rank}] Démarrage")
    np.random.seed(rank * 12345)  # Graine différente pour chaque client
    
    samples_computed = 0
    
    while True:
        # Calculer un échantillon
        n_in, n_total = compute_pi_sample(SAMPLE_SIZE)
        samples_computed += 1
        
        # Envoyer au serveur
        comm.send((n_in, n_total), dest=0, tag=0)
        
        # Attendre la réponse
        command = comm.recv(source=0, tag=1)
        
        if command == "STOP":
            print(f"[Client {rank}] Arrêt après {samples_computed} échantillons")
            break
        elif command != "CONTINUE":
            print(f"[Client {rank}] Commande inconnue: {command}")
            break


def main():
    """
    Point d'entrée principal du programme.
    """
    comm = MPI.COMM_WORLD
    rank = comm.Get_rank()
    size = comm.Get_size()
    
    # Vérification : au moins 2 processus (1 serveur + 1 client)
    if size < 2:
        if rank == 0:
            print("ERREUR : Ce programme nécessite au moins 2 processus MPI.")
            print("Utilisation : mpirun -np N python3 pi.py")
            print("              où N >= 2 (1 serveur + au moins 1 client)")
        exit(1)
    
    # Synchronisation initiale
    comm.Barrier()
    
    if rank == 0:
        # Processus serveur
        server_process(comm, size)
    else:
        # Processus client
        client_process(comm, rank)
    
    # Synchronisation finale
    comm.Barrier()


if __name__ == "__main__":
    main()