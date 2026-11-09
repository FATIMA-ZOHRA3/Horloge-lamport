# Résumé de la théorie (à connaître sans notes)

## Le problème
Pas d'horloge commune fiable entre machines : on ne peut pas ordonner les événements avec l'heure.

## Horloge de Lamport (un entier C par processus, initialisé à 0)
1. Événement local : C ← C + 1
2. Envoi : C ← C + 1, puis on joint C au message
3. Réception d'un message d'estampille t : C ← max(C, t) + 1

Propriété : si a arrive avant b (a → b), alors C(a) < C(b).
Limite : la réciproque est fausse. C(a) < C(b) ne prouve pas a → b.

## Horloge vectorielle (Mattern, Fidge) : un vecteur V de n entiers par processus
1. Événement local : V[i] ← V[i] + 1
2. Envoi : V[i] ← V[i] + 1, puis on joint tout le vecteur au message
3. Réception d'un vecteur W : pour chaque k, V[k] ← max(V[k], W[k]), puis V[i] ← V[i] + 1

Comparaison (ordre partiel) :
- A précède B si toutes les cases de A sont ≤ celles de B et A ≠ B.
- A et B sont concurrents si aucun des deux ne précède l'autre.
Ici l'équivalence est vraie dans les deux sens : a → b si et seulement si V(a) < V(b).

## Exemple à refaire à la main (chaîne P1 → P2 → P3)
| Événement | Lamport | Vecteur |
|---|---|---|
| P1 local | 1 | [1,0,0] |
| P1 envoie à P2 | 2 | [2,0,0] |
| P2 reçoit | 3 | [2,1,0] |
| P2 envoie à P3 | 4 | [2,2,0] |
| P3 reçoit | 5 | [2,2,1] |

## Complexité spatiale
- Lamport : 1 entier par processus et par message, soit O(1).
- Vectorielle : n entiers par processus et par message, soit O(n).
- Avec 1000 processus, chaque message porterait 1000 entiers.

## Extension facultative : NTP et PTP
Protocoles de synchronisation des horloges PHYSIQUES (NTP : quelques ms, PTP : de l'ordre de la µs).
Ils ne garantissent pas l'ordre causal, contrairement aux horloges logiques.
