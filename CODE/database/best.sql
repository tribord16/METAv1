-- ============================================================================
-- MetaLeague Database Schema (MySQL 8.0+)
-- ============================================================================

CREATE DATABASE IF NOT EXISTS metaleague_dev CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;
USE metaleague_dev;

-- ============================================================================
-- Users Table - Authentication & User Management
-- ============================================================================
CREATE TABLE users (
    id INT PRIMARY KEY AUTO_INCREMENT,
    username VARCHAR(50) NOT NULL UNIQUE,
    email VARCHAR(255) NOT NULL UNIQUE,
    password_hash VARCHAR(255) NOT NULL,
    is_active BOOLEAN DEFAULT TRUE,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    
    INDEX idx_username (username),
    INDEX idx_email (email),
    INDEX idx_active (is_active)
);

-- ============================================================================
-- Corporations Table - E-sports Organizations
-- ============================================================================
CREATE TABLE corporations (
    id INT PRIMARY KEY AUTO_INCREMENT,
    user_id INT NOT NULL,
    name VARCHAR(100) NOT NULL,
    budget DECIMAL(15,2) DEFAULT 1000000.00,
    reputation INT DEFAULT 50,
    current_season INT DEFAULT 1,
    current_week INT DEFAULT 1,
    esports_active BOOLEAN DEFAULT TRUE,
    racing_active BOOLEAN DEFAULT FALSE,
    tactical_active BOOLEAN DEFAULT FALSE,
    innovation_active BOOLEAN DEFAULT FALSE,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    
    FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE,
    INDEX idx_user_id (user_id),
    INDEX idx_name (name),
    INDEX idx_reputation (reputation)
);

-- ============================================================================
-- Players Table - Professional Gamers/Athletes
-- ============================================================================
CREATE TABLE players (
    id INT PRIMARY KEY AUTO_INCREMENT,
    corporation_id INT,
    first_name VARCHAR(50) NOT NULL,
    last_name VARCHAR(50) NOT NULL,
    nickname VARCHAR(30) NOT NULL,
    position VARCHAR(20) NOT NULL, -- 'TOP', 'JUNGLE', 'MID', 'ADC', 'SUPPORT', 'PILOT', 'TACTICIAN'
    division_type VARCHAR(20) NOT NULL, -- 'CYBER', 'RACING', 'TACTICAL', 'INNOVATION'
    overall_rating INT DEFAULT 70,
    mechanical_skill INT DEFAULT 70,
    game_sense INT DEFAULT 70,
    teamwork INT DEFAULT 70,
    consistency INT DEFAULT 70,
    age INT NOT NULL,
    salary DECIMAL(10,2) DEFAULT 50000.00,
    contract_end_date DATE,
    is_available BOOLEAN DEFAULT TRUE,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    
    FOREIGN KEY (corporation_id) REFERENCES corporations(id) ON DELETE SET NULL,
    INDEX idx_corporation_id (corporation_id),
    INDEX idx_position (position),
    INDEX idx_division_type (division_type),
    INDEX idx_overall_rating (overall_rating),
    INDEX idx_available (is_available),
    
    CHECK (overall_rating BETWEEN 1 AND 100),
    CHECK (mechanical_skill BETWEEN 1 AND 100),
    CHECK (game_sense BETWEEN 1 AND 100),
    CHECK (teamwork BETWEEN 1 AND 100),
    CHECK (consistency BETWEEN 1 AND 100),
    CHECK (age BETWEEN 16 AND 35)
);

-- ============================================================================
-- Competitions Table - Tournaments & Events
-- ============================================================================
CREATE TABLE competitions (
    id INT PRIMARY KEY AUTO_INCREMENT,
    name VARCHAR(100) NOT NULL,
    division_type VARCHAR(20) NOT NULL,
    competition_type VARCHAR(30) NOT NULL, -- 'TOURNAMENT', 'LEAGUE', 'CHAMPIONSHIP'
    start_date DATE NOT NULL,
    end_date DATE NOT NULL,
    prize_pool DECIMAL(12,2) NOT NULL,
    max_participants INT NOT NULL,
    status VARCHAR(20) DEFAULT 'UPCOMING', -- 'UPCOMING', 'ACTIVE', 'COMPLETED'
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    
    INDEX idx_division_type (division_type),
    INDEX idx_status (status),
    INDEX idx_start_date (start_date)
);

-- ============================================================================
-- Matches Table - Individual Games/Races
-- ============================================================================
CREATE TABLE matches (
    id INT PRIMARY KEY AUTO_INCREMENT,
    competition_id INT NOT NULL,
    home_corporation_id INT NOT NULL,
    away_corporation_id INT NOT NULL,
    scheduled_at TIMESTAMP NOT NULL,
    home_score INT DEFAULT 0,
    away_score INT DEFAULT 0,
    winner_corporation_id INT,
    status VARCHAR(20) DEFAULT 'SCHEDULED', -- 'SCHEDULED', 'LIVE', 'COMPLETED', 'CANCELLED'
    match_data JSON, -- Store game-specific data (draft picks, lap times, etc.)
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    
    FOREIGN KEY (competition_id) REFERENCES competitions(id) ON DELETE CASCADE,
    FOREIGN KEY (home_corporation_id) REFERENCES corporations(id) ON DELETE CASCADE,
    FOREIGN KEY (away_corporation_id) REFERENCES corporations(id) ON DELETE CASCADE,
    FOREIGN KEY (winner_corporation_id) REFERENCES corporations(id) ON DELETE SET NULL,
    
    INDEX idx_competition_id (competition_id),
    INDEX idx_scheduled_at (scheduled_at),
    INDEX idx_status (status)
);

-- ============================================================================
-- Transactions Table - Financial Operations
-- ============================================================================
CREATE TABLE transactions (
    id INT PRIMARY KEY AUTO_INCREMENT,
    corporation_id INT NOT NULL,
    amount DECIMAL(12,2) NOT NULL,
    transaction_type VARCHAR(30) NOT NULL, -- 'SALARY', 'PRIZE', 'SPONSOR', 'TRANSFER', 'EXPENSE'
    description TEXT,
    reference_id INT, -- Can reference player_id, match_id, etc.
    reference_type VARCHAR(30), -- 'PLAYER', 'MATCH', 'COMPETITION', etc.
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    
    FOREIGN KEY (corporation_id) REFERENCES corporations(id) ON DELETE CASCADE,
    INDEX idx_corporation_id (corporation_id),
    INDEX idx_transaction_type (transaction_type),
    INDEX idx_created_at (created_at)
);

-- ============================================================================
-- Sample Data for Testing
-- ============================================================================

-- Insert sample user
INSERT INTO users (username, email, password_hash) VALUES 
('testuser', 'test@metaleague.com', '$2b$12$LQv3c1yqBWVHxkd0LHAkCOYz6TtxMQJqhN8/LewdBPj6dzkh.N1zO'); -- password: "password123"

-- Insert sample corporation
INSERT INTO corporations (user_id, name, budget, esports_active) VALUES 
(1, 'Quantum Dynamics', 1500000.00, TRUE);

-- Insert sample players
INSERT INTO players (corporation_id, first_name, last_name, nickname, position, division_type, overall_rating, age, salary) VALUES 
(1, 'Alex', 'Johnson', 'Quantum_Alex', 'MID', 'CYBER', 85, 22, 75000.00),
(1, 'Sarah', 'Chen', 'Sarah_Storm', 'ADC', 'CYBER', 82, 20, 70000.00),
(NULL, 'Marcus', 'Rodriguez', 'Velocity_Max', 'PILOT', 'RACING', 78, 24, 90000.00),
(NULL, 'Emma', 'Thompson', 'Tactical_Emma', 'TACTICIAN', 'TACTICAL', 80, 21, 65000.00);

-- Insert sample competition
INSERT INTO competitions (name, division_type, competition_type, start_date, end_date, prize_pool, max_participants) VALUES 
('Cyber Champions League 2025', 'CYBER', 'LEAGUE', '2025-08-01', '2025-11-30', 2000000.00, 16);