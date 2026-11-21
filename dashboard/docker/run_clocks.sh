#!/bin/sh
set -e

# Copie du dépôt dans le conteneur (sans .git, dashboard ni anciens journaux)
rm -rf /work && mkdir -p /work
cd /repo
tar cf - --exclude=.git --exclude=dashboard --exclude=journaux_reels . | tar xf - -C /work

cd "/work/${SRC_SUBDIR:-src_c}"
rm -rf journaux && mkdir -p journaux

echo "[docker] Compilation..."
if [ -f Makefile ]; then
  make
else
  gcc -O2 -Wall -o process $(find . -name '*.c') -lpthread -lm
fi

N="${N_PROCS:-3}"
START="${START_INDEX:-1}"
touch /tmp/start_run

echo "[docker] Lancement de ${N} processus, scénario : ${SCENARIO}"
PIDS=""
i=0
while [ "$i" -lt "$N" ]; do
  id=$((START + i))
  ./process "$id" "$N" "$SCENARIO" &
  PIDS="$PIDS $!"
  sleep "${START_DELAY:-0}"
  i=$((i + 1))
done
for p in $PIDS; do
  wait "$p" || echo "[docker] Un processus s'est terminé avec un code non nul"
done

# Récupère tous les CSV créés pendant l'exécution
find /work -name '*.csv' -newer /tmp/start_run -exec cp {} /logs/ \;
echo "[docker] CSV récupérés :"
ls /logs
echo "[docker] Terminé"