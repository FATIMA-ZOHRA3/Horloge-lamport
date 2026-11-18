#include <stdio.h>
#include "journal.h"

#ifdef _WIN32
  #include <direct.h>
  #include <windows.h>
  #define CREER_DOSSIER(d) _mkdir(d)
#else
  #include <sys/stat.h>
  #include <time.h>
  #define CREER_DOSSIER(d) mkdir(d, 0755)
#endif

static FILE* fichier = NULL;

/* Heure reelle locale en ms (sert seulement a l'affichage) */
static unsigned long long maintenant_ms(void) {
#ifdef _WIN32
    FILETIME ft;
    ULARGE_INTEGER u;
    GetSystemTimeAsFileTime(&ft);
    u.LowPart = ft.dwLowDateTime;
    u.HighPart = ft.dwHighDateTime;
    return u.QuadPart / 10000ULL - 11644473600000ULL;   /* ms depuis 1970 */
#else
    struct timespec t;
    clock_gettime(CLOCK_REALTIME, &t);
    return (unsigned long long)t.tv_sec * 1000ULL + t.tv_nsec / 1000000ULL;
#endif
}

int journal_ouvrir(int pid) {
    char chemin[64];
    CREER_DOSSIER("journaux");                            /* ignore l'erreur "existe deja" */
    snprintf(chemin, sizeof(chemin), "journaux/log_%d.csv", pid);
    fichier = fopen(chemin, "w");
    if (!fichier) {
        printf("[P%d] Impossible d'ouvrir %s\n", pid, chemin);
        return 0;
    }
    fprintf(fichier, "pid,type,temps_ms,lamport,vecteur,msg_id,pair\n");
    fflush(fichier);
    return 1;
}

void journal_ecrire(int pid, const char* type, int lamport,
                    const int* vecteur, int n, int msg_id, int pair) {
    if (!fichier) return;
    fprintf(fichier, "%d,%s,%llu,%d,", pid, type, maintenant_ms(), lamport);
    for (int k = 0; k < n; k++)
        fprintf(fichier, k ? ";%d" : "%d", vecteur[k]);   /* vecteur : 2;1;0 */
    fprintf(fichier, ",");
    if (msg_id) fprintf(fichier, "%d", msg_id);
    fprintf(fichier, ",");
    if (pair) fprintf(fichier, "%d", pair);
    fprintf(fichier, "\n");
    fflush(fichier);                                      /* rien ne se perd si le processus s'arrete */
}

void journal_fermer(void) {
    if (fichier) { fclose(fichier); fichier = NULL; }
}