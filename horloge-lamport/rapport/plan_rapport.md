# Plan du rapport

| N° | Chapitre | Contenu | Pages |
|---|---|---|---|
| 0 | Page de garde | Titre, noms, filière ENSAH GI-GL S5, date | 1 |
| 1 | Introduction | Problème des horloges dans un système distribué, objectif, plan | 1 |
| 2 | Théorie | Modèle, relation « arrive avant », Lamport, preuve de C(a) < C(b), vectorielles, comparaison partielle | 3 |
| 3 | Conception | Architecture C + Python, sockets TCP, formats du message et du journal | 2 |
| 4 | Réalisation | Choix techniques, extraits commentés, difficultés rencontrées | 2 |
| 5 | Scénarios et courbes | Chaîne, concurrent, tous vers tous, anneau, aléatoire : frise + courbes + analyse | 4 |
| 6 | Complexité spatiale | Taille d'un message et mémoire par processus : O(1) contre O(n), tableau pour n = 3, 4, 5 et discussion pour n grand | 1 |
| 7 | Comparatif Lamport vs vectorielles | Tableau + nombre de paires que Lamport ordonne à tort | 1 à 2 |
| 8 | Extension (facultatif) | NTP et PTP : principe, précision, pourquoi ils ne donnent pas la causalité | 1 |
| 9 | Conclusion | Bilan, limites, pistes | 1 |
| A | Annexes | Mode d'emploi, répartition du travail | selon besoin |

## Tableau comparatif à remplir
| Critère | Lamport | Vectorielle |
|---|---|---|
| Donnée par processus | 1 entier | n entiers |
| Taille ajoutée à chaque message | | |
| a → b implique | | |
| Réciproque vraie ? | | |
| Détecte la concurrence | | |
| Ordre obtenu | | |

## Résultats à reporter pour chaque scénario
| Scénario | Nb d'événements | Nb de paires comparées | Paires causales | Paires concurrentes | Paires ordonnées à tort par Lamport |
|---|---|---|---|---|---|
| Chaîne 3 | | | | | |
| Concurrent 3 | | | | | |
| Tous vers tous 3 | | | | | |
| Anneau 5 | | | | | |
| Aléatoire | | | | | |
