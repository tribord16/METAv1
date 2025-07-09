# cmd entr
```bash
find ../CODE/backend/src ../CODE/backend/include \
  -name '*.cpp' -o -name '*.h' | \
  entr -r ./build.sh


curl https://sh.rustup.rs -sSf | sh

À la fin, il va dire d’ajouter ~/.cargo/bin à ton PATH, mais on va le faire juste après.

source $HOME/.cargo/env

rustc --version

rustc 1.82.0 (ou supérieur)

cargo install watchexec-cli

watchexec -e cpp,h ./build.sh


watchexec -w CODE/backend/src -w CODE/backend/include -e cpp,h ./TOOLS/build.sh


Backend Output with watchexec
watchexec only shows the output of the build, not the running backend.
To see backend logs, run your backend binary (./meta_league_backend) in a separate terminal after each build.

For a better dev workflow:
Use two terminals: one for watchexec (auto-build), one for running the backend (auto-restart on binary change).
or use entr to auto-restart the backend when the binary changes:


find ./build/MetaLeagueBackend | entr -r ./build/MetaLeagueBackend


tail -f /home/tribord/PERSO/METAv1/CODE/backend/build/backend.log


watchexec -w CODE/backend/src -w CODE/backend/include -e cpp,h ./TOOLS/build.sh


3. Astuce : combine build auto + logs live
Utilise watchexec ou entr pour rebuild.
Utilise un second terminal avec tail -f pour les logs.
Ou, pour un workflow « auto-restart » :
Utilise entr pour relancer le backend à chaque rebuild, et affiche les logs en direct :
(Lance ça dans le dossier build.)
