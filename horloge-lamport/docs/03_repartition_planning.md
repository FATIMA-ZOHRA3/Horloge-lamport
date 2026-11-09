# Répartition à trois et planning (jalons S1 à S5)

## Les trois rôles
| Personne | Rôle | Fichiers principaux |
|---|---|---|
| [Nom 1] | Processus et communication (C) : sockets, lancement, envoi/réception | src_c/proc.c, src_c/reseau.* |
| [Nom 2] | Horloges et journal (C) : Lamport, vecteurs, écriture du journal | src_c/horloges.*, src_c/journal.* |
| [Nom 3] | Visualisation, causalité, courbes, rapport (Python) | viz_python/*, rapport/* |

## Planning
| Jalon | [Nom 1] | [Nom 2] | [Nom 3] |
|---|---|---|---|
| S1 Sockets | 2 puis 3 à 5 processus s'échangent un message | Valide avec les autres les formats (docs/02) | Prépare le script de lancement, teste la lecture des faux journaux |
| S2 Lamport | Intègre envoi/réception dans les scénarios | Règles de Lamport + journal | Première frise avec valeurs de Lamport |
| S3 Vectorielle | Vérifie que le vecteur voyage dans les messages | Vecteurs et leurs règles | Détection de causalité et de concurrence |
| S4 Visualisation | Prépare les scénarios (chaîne, concurrents, 5 processus, aléatoire) | Vérifie les valeurs à la main | Frise, flèches, annotations, courbes |
| S5 Rapport | Partie sockets et architecture | Partie horloges et complexité spatiale | Partie résultats et comparaison |

## Points de rencontre
- Fin S1 : 3 processus s'échangent des messages.
- Fin S2 : le Python lit sans erreur un journal produit par le C.
- Fin S3 : un petit scénario est calculé à la main, puis comparé au journal. Toute différence est un bug.
- Fin S4 : les 4 scénarios sont testés, les images sont gardées pour le rapport.
- S5 : relecture croisée et répétition de la soutenance.

## Soutenance (10 à 12 minutes)
| Qui | Sujet |
|---|---|
| [Nom 1] | Problème et architecture (processus, sockets) |
| [Nom 2] | Règles de Lamport et des vecteurs, complexité spatiale |
| [Nom 3] | Démonstration graphique, causalité, comparaison |
