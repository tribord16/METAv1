#!/bin/bash

# Récupère tous les .cpp et .h dans src/ et include/
find ./src ./include -type f \( -name "*.cpp" -o -name "*.h" \) > all_files.txt

# Compte le nombre total de lignes
total_lines=$(cat $(cat all_files.txt) | wc -l)
files_count=20
lines_per_file=$(( (total_lines + files_count - 1) / files_count ))

bundle=1
current_lines=0
> bundle_$(printf "%02d" $bundle).txt

for f in $(cat all_files.txt); do
    echo -e "\n// ====== $f ======" >> bundle_$(printf "%02d" $bundle).txt
    cat "$f" >> bundle_$(printf "%02d" $bundle).txt
    current_lines=$((current_lines + $(cat "$f" | wc -l)))
    if [ $current_lines -ge $lines_per_file ] && [ $bundle -lt $files_count ]; then
        bundle=$((bundle + 1))
        current_lines=0
        > bundle_$(printf "%02d" $bundle).txt
    fi
done

rm all_files.txt
echo "Fichiers créés : bundle_01.txt ... bundle_$(printf "%02d" $bundle).txt"