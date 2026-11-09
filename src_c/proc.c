/*
 * proc.c : programme principal d'un processus
 * Responsable : [Nom 1]
 *
 * TODO :
 *  - lire les paramètres : numéro du processus, nombre de processus n (3 à 5), fichier de scénario ;
 *  - initialiser les horloges (reseau, horloges, journal) ;
 *  - lire le scénario et ne garder que les lignes de ce processus ;
 *  - pour chaque action : L -> événement local, S k -> envoi au processus k, R -> réception ;
 *  - fermer proprement les sockets et le journal à la fin.
 *
 * Points d'attention :
 *  - la réception doit avoir un délai maximal (timeout) pour ne jamais bloquer à l'infini ;
 *  - tous les processus doivent être en écoute avant le premier envoi (prévoir une courte attente au démarrage).
 */
