#!/bin/bash
set -e  # stop on error

cd /home/tribord/PERSO/METAv1/CODE/backend/build

# Compile seulement si nécessaire
make -j$(nproc)

# Tue proprement le backend s’il tourne
pkill -f MetaLeagueBackend || true

# Lancer le backend sans bloquer, redirige stderr/stdout
#nohup ./meta_league_backend > backend.log 2>&1 &
./MetaLeagueBackend