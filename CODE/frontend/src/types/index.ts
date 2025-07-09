export interface User {
  id: number;
  username: string;
  email: string;
  is_active: boolean;
  created_at: string;
}

export interface Corporation {
  id: number;
  name: string;
  user_id: number;
  budget: number;
  reputation: number;
  current_season: string;
  last_played: string;
  esports_active: boolean;
  racing_active: boolean;
  tactical_active: boolean;
  innovation_active: boolean;
  created_at: string;
  updated_at: string;
}

export interface CorporationDashboard {
  corporation: Corporation;
  financial_summary: {
    current_budget: number;
    monthly_income: number;
    monthly_expenses: number;
    net_monthly: number;
    projected_budget_3m: number;
  };
  team_summary: {
    total_players: number;
    average_rating: number;
    total_salary: number;
    missing_positions: string[];
    team_chemistry: number;
  };
  recent_activity: Array<{
    type: string;
    description: string;
    timestamp: string;
    impact: string;
  }>;
  next_competitions: Array<{
    name: string;
    start_date: string;
    type: string;
    status: string;
  }>;
}

export interface Team {
  id: number;
  name: string;
  description: string;
  corporation_id: number;
  budget: number;
  founded_date: string;
  logo_url?: string;
  region: string;
  is_active: boolean;
  created_at: string;
}

export interface Player {
  id: number;
  name: string;
  nickname: string;
  position: 'top' | 'jungle' | 'mid' | 'adc' | 'support';
  team_id?: number;
  skill_level: number;
  salary: number;
  contract_end_date?: string;
  age: number;
  nationality: string;
  is_available: boolean;
}

export interface Competition {
  id: number;
  name: string;
  description: string;
  type: 'tournament' | 'league';
  max_teams: number;
  prize_pool: number;
  start_date: string;
  end_date: string;
  status: 'upcoming' | 'ongoing' | 'finished';
}

export interface ApiError {
  message: string;
  field?: string;
}

export interface AuthResponse {
  success: boolean;
  message: string;
  token?: string;
  user?: User;
}