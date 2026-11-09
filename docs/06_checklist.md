# Checklist

## Compréhension
- [ ] Chacun explique les règles de Lamport et des vecteurs sans notes
- [ ] Chacun refait l'exemple de docs/04 au tableau
- [ ] Chacun explique pourquoi la réciproque de Lamport est fausse

## S1 à S4 (réalisation)
- [ ] S1 : 3 à 5 processus s'échangent des messages par sockets
- [ ] S2 : horloge de Lamport, journal écrit
- [ ] S3 : horloge vectorielle jointe aux messages
- [ ] S4 : frise, flèches, annotations, courbes, détection de causalité

## Scénarios
- [ ] Chaîne (3 processus)
- [ ] Concurrent (3 processus)
- [ ] Tous vers tous (3 processus)
- [ ] Anneau (5 processus)
- [ ] Aléatoire (graine fixée)

## Livrables
- [ ] Code source commenté (C et Python)
- [ ] Courbes pour plusieurs scénarios dans resultats/
- [ ] Rapport comparatif Lamport vs vectorielles
- [ ] Analyse de la complexité spatiale
- [ ] README à jour avec les commandes
- [ ] Diapositives et plan B de démonstration (images déjà générées)

## Facultatif
- [ ] Page de comparaison avec NTP / PTP dans le rapport
