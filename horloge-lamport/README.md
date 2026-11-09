# Mini-projet n°13 : Horloge logique distribuée (Lamport)

ENSAH – GI-GL – S5 – Mini-Projets SETR – Chapitre 7 – Difficulté ★★
Équipe : [Nom 1], [Nom 2], [Nom 3]

## Objectif (fiche officielle)
Synchroniser des événements dans un système distribué sans horloge commune,
avec les horloges logiques de Lamport puis les horloges vectorielles.

## Ce que le projet doit contenir
| Exigence de la fiche | Où dans ce dossier |
|---|---|
| 3 à 5 processus, sockets Unix ou TCP | `src_c/` (reseau, proc) |
| Horloge de Lamport (C ← C+1, envoi, max+1) | `src_c/horloges` |
| Horloges vectorielles (Mattern, Fidge) | `src_c/horloges` |
| Détection de la causalité | `viz_python/causalite.py` |
| Interface graphique matplotlib (frise, flèches, annotations) | `viz_python/viz.py` |
| Courbes d'horloges pour plusieurs scénarios | `viz_python/courbes.py`, `scenarios/`, `resultats/` |
| Rapport comparatif Lamport vs vectorielles | `rapport/plan_rapport.md` |
| Analyse de la complexité spatiale | `rapport/plan_rapport.md` (chapitre 6) |
| Extension : NTP / PTP (facultatif) | `rapport/plan_rapport.md` (chapitre 8) |

## Arborescence
```
horloge-lamport/
├── README.md                 (ce fichier)
├── docs/                     comprendre, contrats, répartition, soutenance
├── src_c/                    processus en C (sockets, horloges, journal)
├── scenarios/                scénarios de test (fichiers texte)
├── journaux_exemples/        faux journaux écrits à la main pour commencer la visualisation
├── viz_python/               visualisation, causalité, courbes (Python)
├── resultats/                images et courbes générées
└── rapport/                  plan du rapport et tableaux à remplir
```

## Par où commencer
1. Lire `docs/04_theorie_resume.md` et refaire à la main l'exemple.
2. Se mettre d'accord sur `docs/02_contrats_formats.md` (format du message, du journal, des scénarios) et ne plus le changer sans prévenir.
3. Lire `docs/03_repartition_planning.md` et choisir les rôles.
4. Chacun ouvre ses fichiers (`TODO` en tête de chaque fichier) et travaille dans sa branche Git.
5. Cocher `docs/06_checklist.md` au fur et à mesure.

## Règles d'équipe
- Une branche Git par personne, jamais de poussée directe sur `main`.
- Chaque modification est relue par un autre membre.
- Réunion courte deux fois par semaine.
- Chacun doit savoir expliquer tout le projet à la soutenance.
