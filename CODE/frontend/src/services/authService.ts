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
    const response = await apiClient.post('/controllers/authcontroller/register', data);
    return response.data;
  },

  async login(data: LoginRequest): Promise<AuthResponse> {
    const response = await apiClient.post('/controllers/authcontroller/login', data);
    if (response.data.token) {
      localStorage.setItem('jwt_token', response.data.token);
    }
    return response.data;
  },

  async getCurrentUser(): Promise<User> {
    const response = await apiClient.get('/controllers/authcontroller/me');
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