/**
 * @file AuthContext.tsx
 * @brief Authentication context/provider for MetaLeague frontend
 *
 * ROLE: Provides authentication state and actions (login, logout, register) to the app via React context.
 * PURPOSE: Centralizes user session logic, exposes hooks for login/logout/register, and manages loading state.
 * DEPENDENCIES: React, authService, User type
 *
 * TODOs:
 *   - [ ] Add persistent login (remember me)
 *   - [ ] Add error handling for all async actions
 *   - [ ] Add tests for context logic
 *   - [ ] Add role/permission checks for user
 *   - [ ] Add logging for auth events
 *
 * Patterns: Context Provider, useContext, useEffect for session restore
 */
import React, { createContext, useContext, useState, useEffect, ReactNode } from 'react';
import { User } from '../types';
import { authService } from '../services/authService';

interface AuthContextType {
  user: User | null;
  login: (username: string, password: string) => Promise<void>;
  register: (username: string, email: string, password: string) => Promise<void>;
  logout: () => void;
  loading: boolean;
}

const AuthContext = createContext<AuthContextType | undefined>(undefined);

interface AuthProviderProps {
  children: ReactNode;
}

export const AuthProvider: React.FC<AuthProviderProps> = ({ children }) => {
  // user: current authenticated user (null if not logged in)
  const [user, setUser] = useState<User | null>(null);
  // loading: true while checking session or performing auth actions
  const [loading, setLoading] = useState(true);

  // On mount, try to restore user session from storage/service
  useEffect(() => {
    const loadUser = async () => {
      if (authService.isAuthenticated()) {
        try {
          const currentUser = await authService.getCurrentUser();
          setUser(currentUser);
        } catch (error) {
          // TODO: Add user-friendly error message
          console.error('Error loading user:', error);
          authService.logout();
        }
      }
      setLoading(false);
    };
    loadUser();
  }, []);

  const login = async (username: string, password: string) => {
    const response = await authService.login({ username, password });
    if (response.success && response.user) {
      setUser(response.user);
    } else {
      throw new Error(response.message);
    }
  };

  const register = async (username: string, email: string, password: string) => {
    const response = await authService.register({ username, email, password });
    if (response.success && response.user) {
      setUser(response.user);
    } else {
      throw new Error(response.message);
    }
  };

  const logout = () => {
    authService.logout();
    setUser(null);
  };

  return (
    <AuthContext.Provider value={{ user, login, register, logout, loading }}>
      {children}
    </AuthContext.Provider>
  );
};

export const useAuth = () => {
  const context = useContext(AuthContext);
  if (!context) {
    throw new Error('useAuth must be used within AuthProvider');
  }
  return context;
};