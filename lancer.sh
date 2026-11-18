#!/bin/sh
SCEN=${1:-chaine_3}
N=${2:-3}
cd /app/src_c
rm -rf journaux
i=1
while [ "$i" -le "$N" ]; do
  ./process "$i" "$N" "../scenarios/$SCEN.txt" &
  i=$((i+1))
done
wait
echo "--- journaux ---"
head -n 20 journaux/log_*.csv