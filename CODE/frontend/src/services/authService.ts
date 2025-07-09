/**
 * @file authService.ts
 * @brief Service d'authentification pour MetaLeague frontend
 *
 * ROLE: Fournit les méthodes pour login, register, logout, et récupération de l'utilisateur courant
 * PURPOSE: Centralise la logique d'authentification et la gestion du token JWT
 * DEPENDENCIES: apiClient, User type
 *
 * TODOs:
 *   - [ ] Ajouter la gestion du refresh token
 *   - [ ] Ajouter des logs pour toutes les actions auth
 *   - [ ] Ajouter des tests unitaires pour chaque méthode
 *   - [ ] Gérer les erreurs réseau et serveur de façon plus fine
 *   - [ ] Ajouter la suppression du token côté backend lors du logout
 *
 * Patterns: Service singleton, promesse, typage fort
 */

 // src/services/authService.ts
import { apiClient } from './apiClient';
import { User } from '../types';

export interface RegisterRequest {
  username: string;
  email: string;
  password: string;
}

export interface LoginRequest {
  username: string;
  password: string;
}

export interface AuthResponse {
  success: boolean;
  message: string;
  token?: string;
  user?: User;
}

export const authService = {
  async register(data: RegisterRequest): Promise<AuthResponse> {
    const response = await apiClient.post('/api/auth/register', data);
    return response.data;
  },

  async login(data: LoginRequest): Promise<AuthResponse> {
    const response = await apiClient.post('/api/auth/login', data);
    if (response.data.token) {
      localStorage.setItem('jwt_token', response.data.token);
    }
    return response.data;
  },

  async getCurrentUser(): Promise<User> {
    const response = await apiClient.get('/api/auth/me');
    return response.data.data;
  },

  logout() {
    localStorage.removeItem('jwt_token');
    window.location.href = '/login';
  },

  isAuthenticated(): boolean {
    return !!localStorage.getItem('jwt_token');
  }
};