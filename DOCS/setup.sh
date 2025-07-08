#etape 1
# Mise à jour
sudo apt update && sudo apt upgrade -y

sudo apt install libmariadb-dev-compat libmariadb-dev

# Dépendances essentielles
sudo apt install -y \
    build-essential \
    cmake \
    git \
    mysql-server \
    libmysqlclient-dev \
    libjsoncpp-dev \
    libssl-dev \
    uuid-dev \
    zlib1g-dev \
    curl \
    pkg-config

sudo apt install libmariadb-dev-compat libmariadb-dev
sudo apt-get install libhiredis-dev

sudo apt install git gcc g++ cmake libjsoncpp-dev uuid-dev zlib1g-dev libssl-dev

# etpae 2
# Cloner Drogon
git clone https://github.com/drogonframework/drogon
cd drogon
git submodule update --init
mkdir build
cd build
cmake ..
make
sudo make install

# Vérifier installation
drogon_ctl version

#etape 3
# Sécuriser MySQL
sudo mysql_secure_installation

# Se connecter
sudo mysql -u root -p

# Dans MySQL :
CREATE DATABASE meta_league CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;
CREATE USER 'meta_admin'@'localhost' IDENTIFIED BY 'MetaLeague2025!';
GRANT ALL PRIVILEGES ON meta_league.* TO 'meta_admin'@'localhost';
FLUSH PRIVILEGES;

#ENFAITE PASS PAR UNE EXTENSION SSH MYSQL C LE MEILLEUR TRUC

#Crée un utilisateur accessible de partout :
CREATE USER 'meta_admin'@'%' IDENTIFIED BY 'MetaLeague2025!';
GRANT ALL PRIVILEGES ON meta_league.* TO 'meta_admin'@'%';
FLUSH PRIVILEGES;


sudo nano /etc/mysql/mysql.conf.d/mysqld.cnf
bind-address = 127.0.0.1
bind-address = 0.0.0.0
sudo systemctl restart mysql

sudo ufw allow 3306
sudo ufw status




# Créer table users
USE meta_league;
CREATE TABLE users (
    id INT AUTO_INCREMENT PRIMARY KEY,
    username VARCHAR(50) UNIQUE NOT NULL,
    email VARCHAR(255) UNIQUE NOT NULL,
    password_hash VARCHAR(255) NOT NULL,
    is_active BOOLEAN DEFAULT TRUE,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
);

# Test user pour développement
INSERT INTO users (username, email, password_hash) 
VALUES ('test', 'test@example.com', '$2b$12$LQv3c1yqBWVHxkd0LHAkCOYz6TtxMQJqhN8/LewdBPj3bp.93.xSO');

EXIT;

#etape 4
# Aller dans le dossier de travail
#cd /opt
#sudo mkdir meta_league
#sudo chown $USER:$USER meta_league
#cd meta_league

# Cloner ton repo
#git clone https://github.com/TON_USER/TON_REPO.git .


# Créer config/dev.env
mkdir -p config
cat > config/dev.env << 'EOF'
APP_NAME=Meta League Backend
DEBUG_MODE=true
SERVER_HOST=0.0.0.0
SERVER_PORT=8080

# MySQL Configuration
DB_HOST=localhost
DB_PORT=3306
DB_NAME=meta_league
DB_USER=meta_admin
DB_PASSWORD=MetaLeague2025!

# JWT Configuration
JWT_SECRET=ton_secret_jwt_super_long_ici

# Game Configuration
GAME_DEFAULT_CORPORATION_BUDGET=100000.0
GAME_MINIMUM_SALARY=30000.0
GAME_MAXIMUM_SALARY=500000.0
GAME_DEFAULT_PRIZE_MONEY=50000.0
EOF

# Générer JWT secret sécurisé
JWT_SECRET=$(openssl rand -base64 64)
sed -i "s/ton_secret_jwt_super_long_ici/$JWT_SECRET/" config/dev.env


#etape 5
cat > config/config.json << 'EOF'
{
  "listeners": [
    {
      "address": "0.0.0.0",
      "port": 8080,
      "https": false
    }
  ],
  "db_clients": [
    {
      "name": "default",
      "rdbms": "mysql",
      "host": "127.0.0.1",
      "port": 3306,
      "dbname": "meta_league",
      "user": "meta_admin",
      "passwd": "MetaLeague2025!",
      "is_fast": true,
      "connection_number": 10,
      "charset": "utf8mb4",
      "auto_batch": true
    }
  ],
  "log": {
    "log_path": "./logs/",
    "logfile_base_name": "meta_league",
    "log_size_limit": 100000000,
    "log_level": "INFO"
  }
}
EOF

#etape 6
# Créer dossier build
mkdir -p backend/build
cd backend/build

# Configurer et compiler
cmake ..
make -j$(nproc)

# Vérifier que l'exécutable existe
ls -la meta_league_backend

#etape 7
# Lancer le serveur (en premier plan pour tester)  JSUQUE LA CEST SAFE EN SAH fin plus au moins
./meta_league_backend

# Dans un autre terminal, tester
curl -X GET http://localhost:8080/test
curl -X POST http://localhost:8080/api/auth/login \
  -H "Content-Type: application/json" \
  -d '{"username":"test","password":"password123"}'


#etape 8
# Créer service systemd
sudo tee /etc/systemd/system/meta-league.service > /dev/null << 'EOF'
[Unit]
Description=Meta League Backend
After=network.target mysql.service
Requires=mysql.service

[Service]
Type=simple
User=www-data
WorkingDirectory=/opt/meta_league/backend/build
ExecStart=/opt/meta_league/backend/build/meta_league_backend
Restart=always
RestartSec=3

# Logs
StandardOutput=journal
StandardError=journal

# Sécurité
NoNewPrivileges=yes
PrivateTmp=yes

[Install]
WantedBy=multi-user.target
EOF

# Activer et démarrer
sudo systemctl daemon-reload
sudo systemctl enable meta-league
sudo systemctl start meta-league

# Vérifier status
sudo systemctl status meta-league