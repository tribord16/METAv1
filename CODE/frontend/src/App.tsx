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
import { FullPageLoading } from './components';

const ProtectedRoute: React.FC<{ children: React.ReactNode }> = ({ children }) => {
  const { user, loading } = useAuth();
  
  if (loading) {
    return <FullPageLoading />;
  }
  
  return user ? <>{children}</> : <Navigate to="/login" replace />;
};

const CorporationRoute: React.FC<{ children: React.ReactNode }> = ({ children }) => {
  const { currentCorporation, corporations, loading } = useCorporation();
  
  if (loading) {
    return <FullPageLoading message="Loading corporations..." />;
  }
  
  // If no corporations exist, redirect to create one
  if (corporations.length === 0) {
    return <Navigate to="/corporation/create" replace />;
  }
  
  // If no current corporation selected, redirect to select
  if (!currentCorporation) {
    return <Navigate to="/corporation/select" replace />;
  }
  
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
                  <CorporationSelect />
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