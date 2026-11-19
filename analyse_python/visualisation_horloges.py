import csv
import glob
import os
import sys

import matplotlib.pyplot as plt
from matplotlib.lines import Line2D

#lecture
def lire_journaux(dossier):
    fichiers = sorted(glob.glob(os.path.join(dossier, "log_*.csv")))
    if not fichiers:
        sys.exit(f"Aucun log_*.csv trouve dans : {dossier}")
    evenements = []
    for chemin in fichiers:
        with open(chemin, newline="", encoding="utf-8") as f:
            for ligne in csv.DictReader(f):
                evenements.append({
                    "pid": int(ligne["pid"]),
                    "type": ligne["type"].strip(),
                    "temps_ms": ligne["temps_ms"],
                    "lamport": int(ligne["lamport"]),
                    "vecteur": tuple(int(x) for x in ligne["vecteur"].split(";")),
                    "msg_id": (ligne.get("msg_id") or "").strip(),
                    "pair": (ligne.get("pair") or "").strip(),
                })
    return evenements

#causalite
def precede(a, b):
    """a precede b : toutes les cases de a <= celles de b, et a != b."""
    va, vb = a["vecteur"], b["vecteur"]
    return va != vb and all(x <= y for x, y in zip(va, vb))


def concurrents(a, b):
    return not precede(a, b) and not precede(b, a)


def statistiques(evts):
    causales = concurrentes = mal_ordonnees = 0
    for i in range(len(evts)):
        for j in range(i + 1, len(evts)):
            a, b = evts[i], evts[j]
            if concurrents(a, b):
                concurrentes += 1
                if a["lamport"] != b["lamport"]:
                    mal_ordonnees += 1  # Lamport les range comme s'ils etaient lies
            else:
                causales += 1
    return causales, concurrentes, mal_ordonnees


#frise
STYLE = {
    "LOCAL": ("o", "tab:green"),
    "ENVOI": ("^", "tab:blue"),
    "RECEPTION": ("v", "tab:orange"),
}


def dessiner_frise(evts, nom):
    n = len(evts[0]["vecteur"])
    fig, ax = plt.subplots(figsize=(11, 5))
    y_de = lambda pid: n - pid  # P1 en haut

    xmax = max(e["lamport"] for e in evts)
    for pid in range(1, n + 1):
        ax.hlines(y_de(pid), 0, xmax + 1, color="lightgray", zorder=1)
        ax.text(-0.3, y_de(pid), f"P{pid}", ha="right", va="center",
                fontsize=12, fontweight="bold")

    # position x = valeur de Lamport (temps logique, PAS temps_ms)
    artistes = []
    for k, e in enumerate(evts):
        marqueur, couleur = STYLE.get(e["type"], ("s", "gray"))
        x, y = e["lamport"], y_de(e["pid"])
        (pt,) = ax.plot(x, y, marqueur, color=couleur, markersize=11,
                        markeredgecolor=couleur, zorder=3)
        artistes.append(pt)
        v = "[" + ",".join(map(str, e["vecteur"])) + "]"
        decalage = 14 if k % 2 == 0 else -26  # alterne haut/bas pour eviter le chevauchement
        ax.annotate(f"L={e['lamport']}\n{v}", (x, y), textcoords="offset points",
                    xytext=(0, decalage), ha="center", fontsize=8, zorder=4)
        e["_x"], e["_y"], e["_art"] = x, y, pt

    # fleches : ENVOI -> RECEPTION de meme msg_id
    envois = {e["msg_id"]: e for e in evts if e["type"] == "ENVOI" and e["msg_id"]}
    for r in evts:
        if r["type"] == "RECEPTION" and r["msg_id"] in envois:
            s = envois[r["msg_id"]]
            ax.annotate("", xy=(r["_x"], r["_y"]), xytext=(s["_x"], s["_y"]),
                        arrowprops=dict(arrowstyle="->", color="black", lw=1.2),
                        zorder=2)

    ax.set_xlim(-1, xmax + 1.5)
    ax.set_ylim(-0.8, n + 0.2)
    ax.set_yticks([])
    ax.set_xlabel("Temps logique (valeur de Lamport)")
    ax.set_title(f"Frise temporelle - {nom}\n(clique sur un evenement : "
                 f"les concurrents passent en rouge)")
    ax.legend(handles=[
        Line2D([], [], marker=m, color=c, linestyle="", label=t)
        for t, (m, c) in STYLE.items()
    ] + [Line2D([], [], marker="o", color="red", linestyle="", label="concurrent")],
        loc="upper right", fontsize=8)

    def reinitialiser():
        for e in evts:
            _, c = STYLE.get(e["type"], ("s", "gray"))
            e["_art"].set_color(c)
            e["_art"].set_markersize(11)

    def au_clic(event):
        if event.inaxes != ax or event.xdata is None:
            return
        # evenement le plus proche du clic (en pixels)
        px = ax.transData.transform((event.xdata, event.ydata))
        def dist(e):
            qx, qy = ax.transData.transform((e["_x"], e["_y"]))
            return (qx - px[0]) ** 2 + (qy - px[1]) ** 2
        choisi = min(evts, key=dist)
        if dist(choisi) > 25 ** 2:
            return
        reinitialiser()
        choisi["_art"].set_markersize(15)
        for e in evts:
            if e is not choisi and concurrents(choisi, e):
                e["_art"].set_color("red")
        fig.canvas.draw_idle()

    fig.canvas.mpl_connect("button_press_event", au_clic)
    fig.tight_layout()
    return fig


# ---------------------------------------------------------------- comparaison
def dessiner_comparaison(evts, nom):
    n = len(evts[0]["vecteur"])
    causales, conc, mal = statistiques(evts)
    fig, (a1, a2) = plt.subplots(1, 2, figsize=(11, 4.5))

    barres = a1.bar(["Paires\ncausales", "Paires\nconcurrentes",
                     "Concurrentes\nmal ordonnees\npar Lamport"],
                    [causales, conc, mal],
                    color=["tab:green", "tab:red", "tab:purple"])
    a1.bar_label(barres)
    a1.set_title(f"Ce que voient les horloges - {nom}")
    a1.set_ylabel("Nombre de paires d'evenements")

    barres2 = a2.bar(["Lamport", "Vectorielle"], [1, n],
                     color=["tab:blue", "tab:orange"])
    a2.bar_label(barres2)
    a2.set_title("Taille ajoutee a chaque message (entiers)")
    a2.set_ylabel("Nombre d'entiers")
    a2.set_ylim(0, n + 1)

    fig.tight_layout()
    return fig


# ---------------------------------------------------------------- main
def main():
    args = [a for a in sys.argv[1:] if not a.startswith("--")]
    if not args:
        sys.exit(__doc__)
    dossier = args[0]
    nom = os.path.basename(os.path.normpath(dossier))
    evts = lire_journaux(dossier)
    print(f"{len(evts)} evenements lus dans '{nom}'")

    f1 = dessiner_frise(evts, nom)
    f2 = dessiner_comparaison(evts, nom)
    if "--png" in sys.argv:
        f1.savefig(f"frise_{nom}.png", dpi=150)
        f2.savefig(f"comparaison_{nom}.png", dpi=150)
        print("Images enregistrees : frise_%s.png, comparaison_%s.png" % (nom, nom))
    plt.show()


if __name__ == "__main__":
    main()