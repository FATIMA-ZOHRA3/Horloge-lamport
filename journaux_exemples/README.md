# Faux journaux écrits à la main

Ils respectent le format de docs/02_contrats_formats.md.
[Nom 3] peut s'en servir pour commencer la visualisation sans attendre que le C fonctionne.

Dans exemple_concurrent_3.csv : les événements « P1 local » (vecteur 1;0;0) et « P3 local » (vecteur 0;0;1)
ont tous deux Lamport = 1 mais sont concurrents. Ce cas doit être détecté par causalite.py.
