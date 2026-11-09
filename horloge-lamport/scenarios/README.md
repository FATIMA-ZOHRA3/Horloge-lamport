# Scénarios

Format : voir docs/02_contrats_formats.md (une ligne = processus, action, destinataire éventuel).

| Fichier | Processus | But |
|---|---|---|
| chaine_3.txt | 3 | Chaîne P1 → P2 → P3 : causalité simple |
| concurrent_3.txt | 3 | Événements concurrents : montre la limite de Lamport |
| tous_vers_tous_3.txt | 3 | Chacun envoie aux deux autres |
| anneau_5.txt | 5 | Anneau P1 → P2 → P3 → P4 → P5 → P1 |
| (à créer) aleatoire_N.txt | 3 à 5 | Scénario tiré au sort avec une graine FIXÉE et notée dans le rapport |

## Valeurs attendues pour chaine_3.txt (à vérifier avec votre programme)
| Événement | Lamport | Vecteur |
|---|---|---|
| P1 local | 1 | [1,0,0] |
| P1 envoie à P2 | 2 | [2,0,0] |
| P2 reçoit | 3 | [2,1,0] |
| P2 envoie à P3 | 4 | [2,2,0] |
| P3 reçoit | 5 | [2,2,1] |

## Attention à l'ordre d'arrivée (concurrent_3.txt)
P2 reçoit deux messages (de P1 avec t = 2, de P3 avec t = 3). Selon l'ordre d'arrivée :
- P1 d'abord : réceptions à 3 puis 4.
- P3 d'abord : réceptions à 4 puis 5.
Les deux exécutions sont correctes. À expliquer dans le rapport : l'ordre d'arrivée n'est pas déterministe.
