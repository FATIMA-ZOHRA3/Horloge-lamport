"""Lecture des journaux CSV (pid,type,temps_ms,lamport,vecteur,msg_id,pair) et analyse de causalité."""
import csv
import glob
import os
import re
import unicodedata

ALIASES = {
    "proc": ["pid", "processus", "process", "proc", "site", "id_processus", "process_id"],
    "type": ["type", "evenement", "event", "action", "op", "type_evenement"],
    "time": ["temps_ms", "temps", "time", "timestamp_ms", "horodatage"],
    "lamport": ["lamport", "horloge_lamport", "horloge", "clock", "ts", "timestamp_lamport"],
    "vector": ["vecteur", "vector", "horloge_vectorielle", "vc", "vectorielle", "horloge_vecteur"],
    "msg": ["msg_id", "message_id", "id_msg", "msg", "id_message"],
    "peer": ["pair", "peer", "dest", "destinataire", "src", "source", "partenaire", "avec", "autre"],
}
MAX_PAIRS = 1500


def _norm(s):
    s = unicodedata.normalize("NFKD", str(s)).encode("ascii", "ignore").decode().lower().strip()
    return re.sub(r"[^a-z0-9]+", "_", s).strip("_")


def _read_rows(path):
    with open(path, encoding="utf-8-sig", newline="") as f:
        text = f.read()
    lines = text.splitlines()
    if not lines:
        return []
    # Séparateur déduit de la ligne d'en-tête (plus fiable que Sniffer)
    delim = max(",;\t|", key=lambda d: lines[0].count(d))
    if lines[0].count(delim) == 0:
        delim = ","
    return [r for r in csv.reader(lines, delimiter=delim) if any(c.strip() for c in r)]


def _map_columns(header):
    cols = [_norm(h) for h in header]
    mapping, used = {}, set()
    for field, names in ALIASES.items():
        for name in names:
            if name in cols:
                i = cols.index(name)
                if i not in used:
                    mapping[field] = i
                    used.add(i)
                    break
    vec_cols = [i for i, c in enumerate(cols) if re.fullmatch(r"(v|vc|vect|vecteur|vector)_?\d+", c)]
    if vec_cols:
        mapping["vector_cols"] = vec_cols
    return mapping


def _num(s):
    m = re.search(r"-?\d+", str(s))
    return int(m.group()) if m else None


def _float(s):
    m = re.search(r"-?\d+(?:[.,]\d+)?", str(s))
    return float(m.group().replace(",", ".")) if m else None


def _vec(s):
    v = [int(x) for x in re.findall(r"\d+", str(s))]
    return v or None


def _type(s):
    s = _norm(s)
    if any(k in s for k in ("send", "envoi", "emi", "snd")):
        return "SEND"
    if any(k in s for k in ("recv", "recep", "recu", "rcv", "receive")):
        return "RECV"
    return "LOCAL"


def _cell(row, i):
    return row[i].strip() if i is not None and i < len(row) else ""


def leq(a, b):
    n = max(len(a), len(b))
    a = a + [0] * (n - len(a))
    b = b + [0] * (n - len(b))
    return all(x <= y for x, y in zip(a, b))


def relation(a, b):
    if a == b:
        return "equal"
    if leq(a, b):
        return "before"
    if leq(b, a):
        return "after"
    return "concurrent"


def load_logs(directory):
    events, warnings = [], []
    files = sorted(glob.glob(os.path.join(directory, "*.csv"))) if os.path.isdir(directory) else []
    for path in files:
        rows = _read_rows(path)
        if len(rows) < 2:
            continue
        header, body = rows[0], rows[1:]
        mp = _map_columns(header)
        if not mp:
            mp = {"proc": 0, "type": 1, "time": 2, "lamport": 3, "vector": 4, "msg": 5, "peer": 6}
            body = rows
            warnings.append(f"{os.path.basename(path)} : pas d'en-tête reconnu, colonnes par défaut utilisées.")
        file_proc = _num(os.path.basename(path))
        for row in body:
            # Vecteur écrit avec des virgules non protégées (ex. 1,0,0) : on refusionne les cellules
            extra = len(row) - len(header)
            vi = mp.get("vector")
            if extra > 0 and vi is not None:
                row = row[:vi] + [",".join(row[vi:vi + 1 + extra])] + row[vi + 1 + extra:]
            p = _num(_cell(row, mp.get("proc"))) if "proc" in mp else file_proc
            if p is None:
                p = file_proc if file_proc is not None else 0
            if "vector_cols" in mp:
                vec = [_num(_cell(row, i)) or 0 for i in mp["vector_cols"]]
            else:
                vec = _vec(_cell(row, mp.get("vector")))
            events.append({
                "file": os.path.basename(path),
                "proc": p,
                "type": _type(_cell(row, mp.get("type"))) if "type" in mp else "LOCAL",
                "time": _float(_cell(row, mp.get("time"))) if "time" in mp else None,
                "lamport": _num(_cell(row, mp.get("lamport"))) if "lamport" in mp else None,
                "vector": vec,
                "peer": _num(_cell(row, mp.get("peer"))) if "peer" in mp else None,
                "msg": _cell(row, mp.get("msg")) or None,
            })
    # Tri chronologique (stable : l'ordre propre à chaque processus est conservé)
    if events and all(e["time"] is not None for e in events):
        events.sort(key=lambda e: e["time"])
    return analyse(events, warnings)


def analyse(events, warnings):
    for i, e in enumerate(events):
        e["id"] = i
    procs = sorted({e["proc"] for e in events})

    # --- Appariement envoi / réception ---
    sends = [e for e in events if e["type"] == "SEND"]
    recvs = [e for e in events if e["type"] == "RECV"]
    msgs = []
    if sends and all(e["msg"] for e in sends + recvs):
        idx = {e["msg"]: e for e in sends}
        for r in recvs:
            s = idx.get(r["msg"])
            if s:
                msgs.append([s["id"], r["id"]])
    else:
        chan = {}
        for s in sends:
            if s["peer"] is not None:
                chan.setdefault((s["proc"], s["peer"]), []).append(s)
        for r in recvs:
            q = chan.get((r["peer"], r["proc"])) if r["peer"] is not None else None
            if q:
                msgs.append([q.pop(0)["id"], r["id"]])
    if (sends or recvs) and not msgs:
        warnings.append("Aucun message apparié : vérifie les colonnes pair et msg_id.")

    # --- Placement horizontal (colonnes) du chronogramme ---
    by_proc = {p: [e for e in events if e["proc"] == p] for p in procs}
    col = {e["id"]: 0 for e in events}
    for _ in range(len(events) + 1):
        changed = False
        for seq in by_proc.values():
            for a, b in zip(seq, seq[1:]):
                if col[b["id"]] < col[a["id"]] + 1:
                    col[b["id"]] = col[a["id"]] + 1
                    changed = True
        for s, r in msgs:
            if col[r] < col[s] + 1:
                col[r] = col[s] + 1
                changed = True
        if not changed:
            break
    for e in events:
        e["col"] = col[e["id"]]

    # --- Statistiques de causalité ---
    has_v = any(e["vector"] for e in events)
    has_l = any(e["lamport"] is not None for e in events)
    conc = false_order = violations = 0
    conc_pairs = []
    if has_v and len(events) <= MAX_PAIRS:
        ev = [e for e in events if e["vector"]]
        for i in range(len(ev)):
            for j in range(i + 1, len(ev)):
                a, b = ev[i], ev[j]
                if a["proc"] == b["proc"]:
                    continue
                rel = relation(a["vector"], b["vector"])
                if rel == "concurrent":
                    conc += 1
                    if len(conc_pairs) < 200:
                        conc_pairs.append([a["id"], b["id"]])
                    if a["lamport"] is not None and b["lamport"] is not None and a["lamport"] != b["lamport"]:
                        false_order += 1
                elif has_l and a["lamport"] is not None and b["lamport"] is not None:
                    first, second = (a, b) if rel == "before" else (b, a)
                    if rel in ("before", "after") and not first["lamport"] < second["lamport"]:
                        violations += 1

    per_proc = []
    for p in procs:
        seq = by_proc[p]
        lam = [e["lamport"] for e in seq if e["lamport"] is not None]
        last_v = next((e["vector"] for e in reversed(seq) if e["vector"]), None)
        per_proc.append({
            "proc": p, "total": len(seq),
            "local": sum(e["type"] == "LOCAL" for e in seq),
            "send": sum(e["type"] == "SEND" for e in seq),
            "recv": sum(e["type"] == "RECV" for e in seq),
            "max_lamport": max(lam) if lam else None, "last_vector": last_v,
        })
    return {
        "processes": procs, "events": events, "messages": msgs, "warnings": warnings,
        "has_vector": has_v, "has_lamport": has_l, "per_proc": per_proc,
        "stats": {
            "events": len(events), "messages": len(msgs), "concurrent_pairs": conc,
            "lamport_false_order": false_order, "lamport_violations": violations,
            "conc_pairs": conc_pairs,
        },
    }