-- Table corporations (entité principale)
CREATE TABLE corporations (
    id INT PRIMARY KEY AUTO_INCREMENT,
    user_id INT NOT NULL,
    name VARCHAR(100) NOT NULL,
    budget DECIMAL(15,2) DEFAULT 1000000.00,
    reputation INT DEFAULT 50 CHECK (reputation >= 0 AND reputation <= 100),
    current_season INT DEFAULT 1,
    current_week INT DEFAULT 1,
    
    -- Divisions actives (expansion progressive)
    esports_active BOOLEAN DEFAULT TRUE,
    racing_active BOOLEAN DEFAULT FALSE,
    tactical_active BOOLEAN DEFAULT FALSE,
    innovation_active BOOLEAN DEFAULT FALSE,
    
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    last_played TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    
    FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE,
    UNIQUE KEY unique_name_per_user (user_id, name),
    INDEX idx_user_lastplayed (user_id, last_played)
);

-- Table players (pool de talents MOBA)-- Table players (pool de talents MOBA) - Suite
CREATE TABLE players (
    id INT PRIMARY KEY AUTO_INCREMENT,
    corporation_id INT DEFAULT NULL,
    name VARCHAR(100) NOT NULL,
    nickname VARCHAR(50) NOT NULL,
    position ENUM('TOP', 'JUNGLE', 'MID', 'ADC', 'SUPPORT') NOT NULL,
    age INT NOT NULL CHECK (age >= 16 AND age <= 35),
    
    -- Stats principales (0-100)
    mechanical_skill INT NOT NULL CHECK (mechanical_skill >= 0 AND mechanical_skill <= 100),
    game_sense INT NOT NULL CHECK (game_sense >= 0 AND game_sense <= 100),
    teamwork INT NOT NULL CHECK (teamwork >= 0 AND teamwork <= 100),
    champion_pool INT NOT NULL CHECK (champion_pool >= 0 AND champion_pool <= 100),
    consistency INT NOT NULL CHECK (consistency >= 0 AND consistency <= 100),
    pressure_handling INT NOT NULL CHECK (pressure_handling >= 0 AND pressure_handling <= 100),
    
    -- État actuel
    current_form ENUM('POOR', 'AVERAGE', 'GOOD', 'EXCELLENT') DEFAULT 'AVERAGE',
    morale INT DEFAULT 75 CHECK (morale >= 0 AND morale <= 100),
    salary DECIMAL(10,2) DEFAULT 5000.00,
    market_value DECIMAL(12,2) GENERATED ALWAYS AS (
        (mechanical_skill + game_sense + teamwork + champion_pool + consistency + pressure_handling) 
        * age_factor * 1000
    ) STORED,
    
    -- Contrat
    contract_end TIMESTAMP NULL,
    is_available BOOLEAN DEFAULT TRUE,
    
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    
    FOREIGN KEY (corporation_id) REFERENCES corporations(id) ON DELETE SET NULL,
    INDEX idx_position_available (position, is_available),
    INDEX idx_corporation (corporation_id),
    INDEX idx_market_value (market_value DESC)
);

-- Table game_states (état temporel du jeu)
CREATE TABLE game_states (
    corporation_id INT PRIMARY KEY,
    current_week INT DEFAULT 1 CHECK (current_week >= 1 AND current_week <= 52),
    current_season INT DEFAULT 1,
    current_phase ENUM('OFF_SEASON', 'SPRING_SPLIT', 'SPRING_PLAYOFFS', 'MID_SEASON', 'SUMMER_SPLIT', 'SUMMER_PLAYOFFS', 'WORLDS') DEFAULT 'OFF_SEASON',
    last_simulation TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    
    FOREIGN KEY (corporation_id) REFERENCES corporations(id) ON DELETE CASCADE
);

-- Table transactions (historique financier)
CREATE TABLE transactions (
    id INT PRIMARY KEY AUTO_INCREMENT,
    corporation_id INT NOT NULL,
    amount DECIMAL(15,2) NOT NULL,
    transaction_type ENUM('SALARY', 'PRIZE', 'SPONSOR', 'TRANSFER', 'FACILITY', 'OTHER') NOT NULL,
    description VARCHAR(255) NOT NULL,
    week INT NOT NULL,
    season INT NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    
    FOREIGN KEY (corporation_id) REFERENCES corporations(id) ON DELETE CASCADE,
    INDEX idx_corporation_date (corporation_id, created_at),
    INDEX idx_type_amount (transaction_type, amount)
);

-- Table contracts (contrats joueurs détaillés)
CREATE TABLE contracts (
    id INT PRIMARY KEY AUTO_INCREMENT,
    player_id INT NOT NULL,
    corporation_id INT NOT NULL,
    monthly_salary DECIMAL(10,2) NOT NULL,
    signing_bonus DECIMAL(12,2) DEFAULT 0,
    contract_start TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    contract_end TIMESTAMP NOT NULL,
    status ENUM('ACTIVE', 'EXPIRED', 'TERMINATED') DEFAULT 'ACTIVE',
    
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    
    FOREIGN KEY (player_id) REFERENCES players(id) ON DELETE CASCADE,
    FOREIGN KEY (corporation_id) REFERENCES corporations(id) ON DELETE CASCADE,
    INDEX idx_player_active (player_id, status),
    INDEX idx_corporation_active (corporation_id, status)
);

-- Vue pour calculs rapides
CREATE VIEW corporation_summary AS
SELECT 
    c.id,
    c.name,
    c.budget,
    c.reputation,
    COUNT(p.id) as player_count,
    COALESCE(SUM(p.salary), 0) as total_salary,
    COALESCE(AVG((p.mechanical_skill + p.game_sense + p.teamwork + 
                  p.champion_pool + p.consistency + p.pressure_handling) / 6), 0) as avg_rating
FROM corporations c
LEFT JOIN players p ON c.id = p.corporation_id
GROUP BY c.id, c.name, c.budget, c.reputation;