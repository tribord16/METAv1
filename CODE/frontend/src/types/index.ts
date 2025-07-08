export interface User {
  id: number;
  username: string;
  email: string;
  is_active: boolean;
  created_at: string;
}

export interface Team {
  id: number;
  name: string;
  description: string;
  owner_id: number;
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