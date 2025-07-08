-- ===================================
-- Meta League Backend - Database Schema
-- ===================================

-- Supprimer les tables si elles existent (pour reset)
DROP TABLE IF EXISTS users;

use meta_league_dev;
-- Table des utilisateurs
CREATE TABLE users (
    id INT AUTO_INCREMENT PRIMARY KEY,
    username VARCHAR(50) NOT NULL UNIQUE,
    email VARCHAR(100) NOT NULL UNIQUE,
    password_hash VARCHAR(255) NOT NULL,
    is_active BOOLEAN DEFAULT TRUE,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
);

    -- Index pour les performances
    
        -- Index pour les performances
CREATE INDEX idx_username ON users(username);
CREATE INDEX idx_email ON users(email);

-- Données de test (optionnel)

-- Vérification
SELECT * FROM users;