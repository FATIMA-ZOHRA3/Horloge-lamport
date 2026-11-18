"""detection_causalite.py : detection de la causalite et de la concurrence.   Membre 2

Usage :  python detection_causalite.py <dossier_du_scenario> [--paires]
ex    :  python detection_causalite.py journaux_reels\\concurrent_3_essai1 --paires

Lit les log_*.csv du dossier (format docs/02_contrats_formats.md), compare les vecteurs
de TOUTES les paires d'evenements et compte :
  - les paires causales      (a -> b : V(a) < V(b))
  - les paires concurrentes  (ni V(a) < V(b), ni V(b) < V(a))
  - les paires que Lamport ordonne a tort (concurrentes, mais L(a) != L(b))
Verifie aussi la propriete de Lamport : a -> b  =>  L(a) < L(b).
"""
import csv
import glob
import os
import sys
from itertools import combinations

AVANT, APRES, EGAUX, CONCURRENTS = "avant", "apres", "egaux", "concurrents"


def comparer_vecteurs(a, b):
    """Meme regle que comparer_vecteurs() de horloges.c."""
    plus_petit = any(x < y for x, y in zip(a, b))
    plus_grand = any(x > y for x, y in zip(a, b))
    if not plus_petit and not plus_grand:
        return EGAUX
    if plus_petit and not plus_grand:
        return AVANT          # a precede b
    if plus_grand and not plus_petit:
        return APRES          # b precede a
    return CONCURRENTS


def lire_evenements(dossier):
    evenements = []
    fichiers = sorted(glob.glob(os.path.join(dossier, "log_*.csv")))
    if not fichiers:
        sys.exit("Aucun log_*.csv dans : " + dossier)
    for chemin in fichiers:
        with open(chemin, newline="", encoding="utf-8") as f:
            for rang, ligne in enumerate(csv.DictReader(f), start=1):
                evenements.append({
                    "nom": "P%s.%d" % (ligne["pid"], rang),     # ex : P2.1 = 1er evenement de P2
                    "pid": int(ligne["pid"]),
                    "type": ligne["type"],
                    "lamport": int(ligne["lamport"]),
                    "vecteur": tuple(int(x) for x in ligne["vecteur"].split(";")),
                    "msg_id": ligne["msg_id"],
                })
    return evenements


def analyser(evenements):
    res = {"causales": 0, "concurrentes": 0, "lamport_a_tort": 0, "violations_lamport": 0}
    concurrentes = []
    for a, b in combinations(evenements, 2):
        rel = comparer_vecteurs(a["vecteur"], b["vecteur"])
        if rel in (AVANT, APRES):
            res["causales"] += 1
            avant, apres = (a, b) if rel == AVANT else (b, a)
            if not avant["lamport"] < apres["lamport"]:
                res["violations_lamport"] += 1                 # ne doit JAMAIS arriver
        elif rel == CONCURRENTS:
            res["concurrentes"] += 1
            concurrentes.append((a, b))
            if a["lamport"] != b["lamport"]:
                res["lamport_a_tort"] += 1                     # Lamport les range comme s'ils etaient lies
    return res, concurrentes


def decrire(e):
    return "%s %s L=%d V=%s" % (e["nom"], e["type"], e["lamport"], list(e["vecteur"]))


def main():
    args = [a for a in sys.argv[1:] if not a.startswith("--")]
    if not args:
        sys.exit(__doc__)
    evs = lire_evenements(args[0])
    res, concurrentes = analyser(evs)
    total = len(evs) * (len(evs) - 1) // 2
    print("Scenario : %s" % os.path.basename(os.path.normpath(args[0])))
    print("  evenements                    : %d (paires : %d)" % (len(evs), total))
    print("  paires causales               : %d" % res["causales"])
    print("  paires concurrentes           : %d" % res["concurrentes"])
    print("  ordonnees a tort par Lamport  : %d" % res["lamport_a_tort"])
    print("  propriete a->b => L(a)<L(b)   : %s" %
          ("OK" if res["violations_lamport"] == 0 else "VIOLEE (%d)" % res["violations_lamport"]))
    if "--paires" in sys.argv:
        for a, b in concurrentes:
            print("    concurrents : %s   ||   %s" % (decrire(a), decrire(b)))


if __name__ == "__main__":
    main()