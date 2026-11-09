/*
 * horloges.c : règles de Lamport et des vecteurs
 * Responsable : [Nom 2]
 *
 * TODO : implémenter
 *  - local     : C = C + 1 ; V[moi] = V[moi] + 1
 *  - envoi     : mêmes incréments, puis copier C et V dans le message
 *  - réception : C = max(C, C_msg) + 1 ; V[k] = max(V[k], W[k]) pour tout k ; V[moi] = V[moi] + 1
 *
 * Vérification : refaire à la main le scénario scenarios/chaine_3.txt
 * et comparer avec les valeurs attendues (scenarios/README.md).
 */
