#include <stdio.h>
#include <stdlib.h>
#include "process.h"
#include "scenario.h"
#include "hooks.h"

/* Usage : process.exe <numero> <n> <scenario>
 * ex :    process.exe 2 3 ..\scenarios\chaine_3.txt */
int main(int argc, char* argv[]) {
    if (argc != 4) {
        printf("Usage : %s <numero> <n> <scenario>\n", argv[0]);
        return 1;
    }
    int moi = atoi(argv[1]);
    int n   = atoi(argv[2]);

    static Scenario sc;
    if (!scenario_charger(&sc, argv[3], moi, n)) return 1;
    printf("[P%d] Scenario charge : %d action(s)\n", moi, sc.nb);

    if (!network_init()) return 1;

    Processus p;
    if (!process_init(&p, moi, n)) {
        printf("[P%d] Echec de l'initialisation\n", moi);
        process_fermer(&p);
        network_cleanup();
        return 1;
    }
    printf("[P%d] Connecte aux %d autres processus\n", moi, n - 1);

    int ok = scenario_executer(&sc, &p);
    printf("[P%d] %s\n", moi, ok ? "Scenario termine" : "Scenario interrompu");

    process_fermer(&p);
    network_cleanup();
    return ok ? 0 : 1;
}