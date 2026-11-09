"""causalite.py : détection de la causalité.   Responsable : [Nom 3]

TODO :
  - une fonction qui compare deux vecteurs : 'avant', 'après', 'égaux' ou 'concurrents' ;
  - une fonction qui parcourt toutes les paires d'événements et compte :
      * les paires causales,
      * les paires concurrentes,
      * les paires que Lamport ordonne à tort (L(a) < L(b) alors qu'ils sont concurrents) ;
  - résultat à reporter dans rapport/plan_rapport.md (tableau des résultats).
"""
