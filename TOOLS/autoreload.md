# cmd entr
find ../CODE/backend/src ../CODE/backend/include \
  -name '*.cpp' -o -name '*.h' | \
  entr -r ./build.sh


curl https://sh.rustup.rs -sSf | sh
Laisse toutes les options par défaut quand il demande.

À la fin, il va dire d’ajouter ~/.cargo/bin à ton PATH, mais on va le faire juste après.

source $HOME/.cargo/env

rustc --version

rustc 1.82.0 (ou supérieur)

cargo install watchexec-cli

watchexec -e cpp,h ./build.sh

Backend Output with watchexec
watchexec only shows the output of the build, not the running backend.
To see backend logs, run your backend binary (./meta_league_backend) in a separate terminal after each build.
For a better dev workflow:
Use two terminals: one for watchexec (auto-build), one for running the backend (auto-restart on binary change).
or use entr to auto-restart the backend when the binary changes:
```bash

find ./build/meta_league_backend | entr -r ./build/meta_league_backend