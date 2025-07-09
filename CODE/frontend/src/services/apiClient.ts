/**
 * @file apiClient.ts
 * @brief Axios HTTP client instance for MetaLeague frontend
 *
 * ROLE: Centralizes all HTTP requests and config (baseURL, headers, interceptors)
 * PURPOSE: Ensures consistent API calls and error handling across the app
 * DEPENDENCIES: axios
 *
 * TODOs:
 *   - [ ] Add support for refresh token/auto-renew
 *   - [ ] Add logging for all requests/responses
 *   - [ ] Add tests for interceptors and error handling
 *   - [ ] Make API_BASE_URL configurable (env)
 *
 * Patterns: Singleton client, request/response interceptors
 */

import axios from 'axios';

const API_BASE_URL = 'http://localhost:8080';

export const apiClient = axios.create({
  baseURL: API_BASE_URL,
  headers: {
    'Content-Type': 'application/json',
  },
  // withCredentials: true, // TEMPORAIREMENT DÉSACTIVÉ POUR DEBUG
});

// Interceptor pour ajouter le token JWT
apiClient.interceptors.request.use((config) => {
  const token = localStorage.getItem('jwt_token');
  if (token) {
    config.headers.Authorization = `Bearer ${token}`;
  }
  return config;
});

// Interceptor pour gérer les erreurs 401
apiClient.interceptors.response.use(
  (response) => response,
  (error) => {
    if (error.response?.status === 401) {
      localStorage.removeItem('jwt_token');
      window.location.href = '/login';
    }
    return Promise.reject(error);
  }
);