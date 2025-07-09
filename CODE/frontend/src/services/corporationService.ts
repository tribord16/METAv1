/**
 * @file corporationService.ts
 * @brief Service pour la gestion des corporations (MetaLeague frontend)
 *
 * ROLE: Fournit les méthodes pour créer, lister, et gérer les corporations côté client
 * PURPOSE: Centralise la logique d'appel API pour les corporations
 * DEPENDENCIES: apiClient, Corporation types
 *
 * TODOs:
 *   - [ ] Ajouter la pagination et le filtrage côté client
 *   - [ ] Ajouter des logs pour toutes les actions
 *   - [ ] Ajouter des tests unitaires pour chaque méthode
 *   - [ ] Gérer les erreurs réseau et serveur de façon plus fine
 *   - [ ] Ajouter la gestion des droits/permissions
 *
 * Patterns: Service singleton, typage fort, promesse
 */

 // src/services/corporationService.ts
import { apiClient } from './apiClient';
import { Corporation } from '../types';
import { get } from 'http';

export interface CreateCorporationRequest {
  name: string;
  initial_budget: number;
  esports_active: boolean;
  racing_active: boolean;
  tactical_active: boolean;
  innovation_active: boolean;
}

export interface CorporationResponse {
  success: boolean;
  message: string;
  data?: Corporation;
}

export interface CorporationListResponse {
  success: boolean;
  message: string;
  data?: Corporation[];
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

export const corporationService = {
  async createCorporation(data: CreateCorporationRequest): Promise<Corporation> {
    const response = await apiClient.post<CorporationResponse>('/api/corporations', data);
    if (!response.data.success || !response.data.data) {
      throw new Error(response.data.message || 'Failed to create corporation');
    }
    return response.data.data;
  },

  
  async getUserCorporations(): Promise<Corporation[]> {
    const response = await apiClient.get<CorporationListResponse>('/api/corporations/');
    console.log(response);
    if (!response.data.success || !response.data.data) {
      throw new Error(response.data.message || 'Failed to load corporations');
    }
    return response.data.data;
  },

  async getCorporationById(id: number): Promise<Corporation> {
    const response = await apiClient.get<CorporationResponse>(`/api/corporations/${id}`);
    console.log(response);
    if (!response.data.success || !response.data.data) {
      throw new Error(response.data.message || 'Failed to load corporation');
    }
    return response.data.data;
  },

  async getCorporationDashboard(id: number): Promise<CorporationDashboard> {
    const response = await apiClient.get<{ success: boolean; message: string; data?: CorporationDashboard }>(`/api/corporations/${id}/dashboard`);
    if (!response.data.success || !response.data.data) {
      throw new Error(response.data.message || 'Failed to load corporation dashboard');
    }
    return response.data.data;
  }
};

// Alias pour compatibilité avec le code existant
export const CorporationAPI = corporationService;
