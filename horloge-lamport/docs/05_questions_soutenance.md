# Questions probables et réponses courtes

1. Pourquoi ne pas utiliser les horloges physiques ?
   Elles ne sont jamais parfaitement synchronisées, donc l'ordre obtenu peut être faux.
2. Que signifie « a arrive avant b » ?
   Même processus et a avant b, ou a est l'envoi dont b est la réception, ou transitivité.
3. Pourquoi prend-on le max à la réception ?
   Pour que la réception soit strictement supérieure à l'envoi, même si le récepteur est en retard.
4. Si C(a) < C(b), peut-on dire que a a causé b ?
   Non. Ils peuvent être concurrents.
5. Comment détecte-t-on la concurrence ?
   Avec les horloges vectorielles : ni V(a) < V(b) ni V(b) < V(a).
6. Quel est le coût des horloges vectorielles ?
   n entiers par message : la taille croît avec le nombre de processus.
7. Pourquoi les sockets plutôt que des threads ?
   Ce sont de vrais processus indépendants, sans mémoire partagée, comme dans un système distribué.
8. Quelle différence avec NTP et PTP ?
   NTP et PTP recalent des horloges physiques ; ils ne donnent pas la causalité.
9. Le résultat change-t-il d'une exécution à l'autre ?
   Oui si deux messages arrivent dans un ordre différent. La propriété de causalité, elle, reste vraie.
