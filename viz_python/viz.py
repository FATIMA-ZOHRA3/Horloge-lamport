"""viz.py : frise temporelle des processus.   Responsable : [Nom 3]

TODO :
  - lire tous les fichiers journaux/log_*.csv (format : docs/02_contrats_formats.md) ;
  - dessiner une ligne horizontale par processus ;
  - placer chaque événement en un point, avec une forme ou une couleur par type (LOCAL, ENVOI, RECEPTION) ;
  - tracer une flèche entre chaque ENVOI et la RECEPTION qui porte le même msg_id ;
  - annoter chaque point avec la valeur de Lamport ET le vecteur ;
  - enregistrer l'image dans resultats/.
"""
