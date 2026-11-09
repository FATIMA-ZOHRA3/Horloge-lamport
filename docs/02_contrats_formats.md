# Contrats entre les trois membres (à valider ensemble)

Tant que ces formats ne changent pas, chacun peut travailler seul.
Toute modification doit être annoncée aux deux autres.

## 1. Le message qui circule entre processus
| Champ | Rôle |
|---|---|
| expediteur | numéro du processus qui envoie (1 à n) |
| id | identifiant unique du message (ex : 1001, 1002, ... pour P1) |
| lamport | valeur de l'horloge scalaire de l'expéditeur APRÈS son +1 |
| vecteur | les n valeurs du vecteur de l'expéditeur APRÈS son +1 |

## 2. Le journal (un fichier CSV par processus : journaux/log_<numéro>.csv)
Une ligne par événement, en-tête obligatoire :

    pid,type,temps_ms,lamport,vecteur,msg_id,pair

| Colonne | Contenu |
|---|---|
| pid | processus où l'événement a lieu |
| type | LOCAL, ENVOI ou RECEPTION |
| temps_ms | heure réelle locale en millisecondes (sert seulement à l'affichage) |
| lamport | horloge scalaire APRÈS l'événement |
| vecteur | vecteur APRÈS l'événement, valeurs séparées par des points-virgules (ex : 2;1;0) |
| msg_id | identifiant du message (vide pour LOCAL) |
| pair | destinataire (ENVOI) ou expéditeur (RECEPTION), vide pour LOCAL |

Règle importante : on note l'ENVOI dans le journal AVANT d'envoyer sur le socket.

## 3. Le scénario (un fichier texte par scénario)
Une ligne par action, dans l'ordre d'exécution de chaque processus :

    <numéro du processus> <action> [destinataire]

Actions : L (événement local), S k (envoyer au processus k), R (recevoir un message).
Les lignes vides et celles qui commencent par # sont ignorées.

## 4. Ports
Processus i écoute sur le port TCP 5000 + i, adresse 127.0.0.1.

## 5. Nombre de processus
3 à 5 (paramètre de lancement).
