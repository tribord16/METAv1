/**
 * @file App.tsx
 * @brief Main React application entry point for MetaLeague frontend
 *
 * ROLE: Sets up global providers (auth, corporation), routing, and protected routes.
 * PURPOSE: Centralizes all navigation and context logic for the app.
 * DEPENDENCIES: React, react-router-dom, AuthContext, CorporationContext, all page components
 *
 * TODOs:
 *   - [ ] Add error boundaries for all routes
 *   - [ ] Add analytics or logging for route changes
 *   - [ ] Improve UX for loading and error states
 *   - [ ] Add tests for route protection logic
 *
 * Patterns: Context Provider, ProtectedRoute, Nested Routing
 */

import React from 'react';
import { BrowserRouter as Router, Routes, Route, Navigate } from 'react-router-dom';
import { AuthProvider, useAuth } from './context/AuthContext';
import { CorporationProvider, useCorporation } from './context/CorporationContext';
import { LoginPage } from './pages/auth/LoginPage';
import { RegisterPage } from './pages/auth/RegisterPage';
import { DashboardPage } from './pages/dashboard/DashboardPage';
import CorporationSelect from './pages/corporation/CorporationSelect';
import CreateCorporation from './pages/corporation/CreateCorporation';
import { NotFoundPage } from './pages/NotFoundPage';
import { FullPageLoading, Header } from './components';
import Layout from './components/Layout';


// ProtectedRoute: Only renders children if user is authenticated, else redirects to login
const ProtectedRoute: React.FC<{ children: React.ReactNode }> = ({ children }) => {
  const { user, loading } = useAuth();
  if (loading) {
    return <FullPageLoading />;
  }
  // TODO: Add more granular error handling for auth failures
  return user ? <>{children}</> : <Navigate to="/login" replace />;
};


// CorporationRoute: Ensures a corporation is selected/created before rendering children
const CorporationRoute: React.FC<{ children: React.ReactNode }> = ({ children }) => {
  const { currentCorporation, corporations, loading } = useCorporation();
  if (loading) {
    // Affiche un écran de chargement pendant la récupération des corporations
    return <FullPageLoading message="Loading corporations..." />;
  }
  // Si l'utilisateur n'a aucune corporation, on le redirige vers la création
  if (corporations.length === 0) {
    // TODO: UX - afficher une notification ou un message avant la redirection
    // Exemple: toast.info('Vous devez créer une corporation pour continuer');
    return <Navigate to="/corporation/create" replace />;
  }
  // Si aucune corporation sélectionnée, on redirige vers la sélection
  if (!currentCorporation) {
    // TODO: UX - afficher une notification ou un message avant la redirection
    // Exemple: toast.info('Veuillez sélectionner une corporation');
    return <Navigate to="/corporation/select" replace />;
  }
  // Si tout est OK, on affiche les enfants
  return <>{children}</>;
};

const PublicRoute: React.FC<{ children: React.ReactNode }> = ({ children }) => {
  const { user, loading } = useAuth();
  
  if (loading) {
    return <FullPageLoading />;
  }
  
  return user ? <Navigate to="/dashboard" replace /> : <>{children}</>;
};

const App: React.FC = () => {
  return (
    <AuthProvider>
      <CorporationProvider>
        <Router>
          <Routes>
            <Route 
              path="/login" 
              element={
                <PublicRoute>
                  <LoginPage />
                </PublicRoute>
              } 
            />
            <Route 
              path="/register" 
              element={
                <PublicRoute>
                  <RegisterPage />
                </PublicRoute>
              } 
            />
            <Route 
              path="/corporation/select" 
              element={
                <ProtectedRoute>
                  <Layout>
                    <CorporationSelect />
                  </Layout>
                </ProtectedRoute>
              } 
            />
            <Route 
              path="/corporation/create" 
              element={
                <ProtectedRoute>
                  <CreateCorporation />
                </ProtectedRoute>
              } 
            />
            <Route 
              path="/dashboard" 
              element={
                <ProtectedRoute>
                  <CorporationRoute>
                    <DashboardPage />
                  </CorporationRoute>
                </ProtectedRoute>
              } 
            />
            <Route path="/" element={<Navigate to="/dashboard" replace />} />
            <Route path="*" element={<NotFoundPage />} />
          </Routes>
        </Router>
      </CorporationProvider>
    </AuthProvider>
  );
};

export default App;