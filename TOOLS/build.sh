#!/bin/bash
set -e  # stop on error

# Active ccache si dispo
if command -v ccache >/dev/null 2>&1; then
    export CC="ccache gcc"
    export CXX="ccache g++"
    echo "[build.sh] ccache activé pour gcc/g++"
else
    echo "[build.sh] ccache non trouvé, compilation sans cache"
fi

cd /home/tribord/PERSO/METAv1/CODE/backend/build

# Compile seulement la cible principale
make -j$(nproc) MetaLeagueBackend

# Tue proprement le backend s’il tourne
pkill -f MetaLeagueBackend || true

# S'assure que le binaire est exécutable
chmod +x ./MetaLeagueBackend

# Lancer le backend sans bloquer, redirige stderr/stdout
nohup ./MetaLeagueBackend > backend.log 2>&1 &
