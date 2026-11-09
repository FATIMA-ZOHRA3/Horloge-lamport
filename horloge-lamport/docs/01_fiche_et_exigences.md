# La fiche officielle, point par point

## Objectif
Synchroniser des événements dans un système distribué sans horloge commune,
avec les horloges de Lamport puis les horloges vectorielles.

## Cahier des charges de la fiche
1. 3 à 5 processus qui communiquent par sockets Unix ou TCP.
2. Horloge de Lamport (scalaire) : local C←C+1 ; envoi : joindre C au message ; réception : C←max(C, C_msg)+1.
3. Extension aux horloges vectorielles (Mattern, Fidge) : vecteur V de taille n ; comparaison partielle (causalité).
4. Détection de la causalité entre événements.
5. Interface graphique (Python/matplotlib) : frise temporelle de chaque processus, flèches de communication, annotations des horloges.

## Livrables attendus
- Code source (C pour les processus, Python pour la visualisation).
- Courbes d'horloges pour plusieurs scénarios.
- Rapport comparatif Lamport vs vectorielles.
- Analyse de la complexité spatiale.

## Extension possible (facultatif)
Comparaison avec les horloges physiques NTP, synchronisation avec PTP (IEEE 1588).

## Jalons
| Jalon | Contenu |
|---|---|
| S1 | Communication sockets multi-processus |
| S2 | Horloge de Lamport |
| S3 | Horloge vectorielle |
| S4 | Visualisation graphique |
| S5 | Rapport et soutenance |
