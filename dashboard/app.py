import glob
import os
import shutil
import subprocess
import sys
import threading
import re
from flask import Flask, jsonify, request

from parser_logs import load_logs

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
COMPOSE = os.path.join(HERE, "docker-compose.yml")
OUT_DIR = os.path.join(HERE, "sorties")
ANALYSE_DIR = os.path.join(ROOT, "analyse_python")
CAUSALITE = os.path.join(ANALYSE_DIR, "detection_causalite.py")
CONTAINER = "lamport_clocks"
SCEN_DIR = os.path.join(ROOT, "scenarios")
NAME_RE = re.compile(r"^[\w\-.]+$")
def list_scenarios():
    if not os.path.isdir(SCEN_DIR):
        return []
    names = [os.path.splitext(f)[0] for f in os.listdir(SCEN_DIR) if f.endswith(".txt")]
    return sorted(n for n in names if NAME_RE.match(n) and ".." not in n)

SCAN_DIRS = ["journaux_reels"]
def list_sources():
    out = []
    for base in SCAN_DIRS:
        top = os.path.join(ROOT, base)
        if not os.path.isdir(top):
            continue
        for dirpath, _, _ in os.walk(top):
            n = len(glob.glob(os.path.join(dirpath, "*.csv")))
            if n:
                rel = os.path.relpath(dirpath, ROOT).replace("\\", "/")
                out.append({"id": rel, "label": rel, "files": n})
    out.append({"id": "docker", "label": "Dernière exécution Docker",
                "files": len(glob.glob(os.path.join(OUT_DIR, "*.csv")))})
    return out

def source_dir(sid):
    if sid == "docker":
        return OUT_DIR
    path = os.path.normpath(os.path.join(ROOT, sid))
    return path if path.startswith(ROOT + os.sep) else OUT_DIR
app = Flask(__name__, static_folder="static", static_url_path="/static")
STATE = {"status": "idle", "lines": [], "proc": None}
LOCK = threading.Lock()


def _log(line):
    with LOCK:
        STATE["lines"].append(line.rstrip())


def _stream(cmd, env):
    flags = getattr(subprocess, "CREATE_NO_WINDOW", 0)
    p = subprocess.Popen(cmd, cwd=HERE, env=env, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                         text=True, encoding="utf-8", errors="replace", creationflags=flags)
    STATE["proc"] = p
    for line in p.stdout:
        _log(line)
    return p.wait()


def _runner(mode, scenario, n):
    env = dict(os.environ, CLOCK_MODE=mode, SCENARIO=f"../scenarios/{scenario}.txt", N_PROCS=str(n))
    os.makedirs(OUT_DIR, exist_ok=True)
    for f in glob.glob(os.path.join(OUT_DIR, "*.csv")):
        os.remove(f)
    steps = [
        ("Construction de l'image Docker", ["docker", "compose", "-f", COMPOSE, "build"]),
        (f"Exécution : scénario « {scenario} », {n} processus",
         ["docker", "compose", "-f", COMPOSE, "run", "--rm", "--name", CONTAINER, "clocks"]),
    ]
    try:
        for title, cmd in steps:
            _log(f"=== {title} ===")
            code = _stream(cmd, env)
            if STATE["status"] == "stopping":
                _log("Exécution arrêtée.")
                STATE["status"] = "stopped"
                return
            if code != 0:
                _log(f"Échec (code {code}).")
                STATE["status"] = "error"
                return
        _log("=== Terminé ===")
        STATE["status"] = "done"
    except FileNotFoundError:
        _log("Docker est introuvable : installe/démarre Docker Desktop.")
        STATE["status"] = "error"

@app.get("/")
def index():
    return app.send_static_file("index.html")


@app.get("/api/sources")
def sources():
    return jsonify(list_sources())


@app.get("/api/data")
def data():
    return jsonify(load_logs(source_dir(request.args.get("source", "docker"))))

@app.get("/api/docker")
def docker_status():
    if not shutil.which("docker"):
        return jsonify({"ok": False})
    try:
        r = subprocess.run(["docker", "info"], capture_output=True, timeout=10)
        return jsonify({"ok": r.returncode == 0})
    except Exception:
        return jsonify({"ok": False})


@app.get("/api/scenarios")
def scenarios():
    return jsonify({"scenarios": list_scenarios(), "default": "chaine_3"})

@app.get("/api/scenario")
def scenario_content():
    name = request.args.get("name", "")
    if name not in list_scenarios():
        return jsonify({"content": "Scénario introuvable."}), 404
    with open(os.path.join(SCEN_DIR, name + ".txt"), encoding="utf-8", errors="replace") as f:
        return jsonify({"content": f.read()})

@app.post("/api/run")
def run():
    if STATE["status"] in ("running", "stopping"):
        return jsonify({"error": "Une exécution est déjà en cours."}), 409
    body = request.get_json(silent=True) or {}
    mode = body.get("mode", "lamport")
    mode = mode if mode in ("lamport", "vector") else "lamport"
    scenario = str(body.get("scenario", ""))
    if scenario not in list_scenarios():
        return jsonify({"error": "Scénario inconnu."}), 400
    try:
        n = int(body.get("n", 3))
    except (TypeError, ValueError):
        n = 3
    n = max(1, min(n, 16))
    with LOCK:
        STATE["lines"] = []
        STATE["status"] = "running"
    threading.Thread(target=_runner, args=(mode, scenario, n), daemon=True).start()
    return jsonify({"status": "running"})

@app.post("/api/stop")
def stop():
    STATE["status"] = "stopping"
    p = STATE.get("proc")
    if p and p.poll() is None:
        p.terminate()
    subprocess.run(["docker", "rm", "-f", CONTAINER], capture_output=True)
    return jsonify({"status": "stopping"})


@app.get("/api/status")
def status():
    since = int(request.args.get("since", 0))
    with LOCK:
        return jsonify({"status": STATE["status"], "lines": STATE["lines"][since:], "total": len(STATE["lines"])})


@app.post("/api/causalite")
def causalite():
    if not os.path.isfile(CAUSALITE):
        return jsonify({"output": "detection_causalite.py introuvable dans analyse_python/."}), 404
    try:
        r = subprocess.run([sys.executable, CAUSALITE], cwd=ANALYSE_DIR, capture_output=True, text=True,
                           encoding="utf-8", errors="replace", timeout=120)
        return jsonify({"output": (r.stdout or "") + (r.stderr or "")})
    except subprocess.TimeoutExpired:
        return jsonify({"output": "Délai dépassé (120 s)."}), 504

if __name__ == "__main__":
    os.makedirs(OUT_DIR, exist_ok=True)
    app.run(host="127.0.0.1", port=5000, debug=False)