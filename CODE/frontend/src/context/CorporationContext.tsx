import React, { useCallback, createContext, useContext, useState, useEffect, ReactNode } from 'react';
import { Corporation, CorporationDashboard } from '../types';
import { corporationService } from '../services/corporationService';
import { useAuth } from './AuthContext';

interface CorporationContextType {
  corporations: Corporation[];
  currentCorporation: Corporation | null;
  currentDashboard: CorporationDashboard | null;
  loading: boolean;
  error: string | null;
  loadCorporations: () => Promise<void>;
  selectCorporation: (id: number) => Promise<void>;
  createCorporation: (data: any) => Promise<Corporation>;
  refreshDashboard: () => Promise<void>;
}

const CorporationContext = createContext<CorporationContextType | undefined>(undefined);

interface CorporationProviderProps {
  children: ReactNode;
}

export const CorporationProvider: React.FC<CorporationProviderProps> = ({ children }) => {
  const [corporations, setCorporations] = useState<Corporation[]>([]);
  const [currentCorporation, setCurrentCorporation] = useState<Corporation | null>(null);
  const [currentDashboard, setCurrentDashboard] = useState<CorporationDashboard | null>(null);
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState<string | null>(null);
  const { user } = useAuth();

  const loadCorporations = useCallback(async () => {
    if (!user) return;
    
    try {
      setLoading(true);
      setError(null);
      const corps = await corporationService.getUserCorporations();
      setCorporations(corps);
      
      // Auto-select saved corporation or first one
      const savedCorporationId = localStorage.getItem('currentCorporationId');
      if (savedCorporationId && corps.length > 0) {
        const savedCorp = corps.find(c => c.id.toString() === savedCorporationId);
        if (savedCorp) {
          setCurrentCorporation(savedCorp);
        } else if (corps.length > 0) {
          setCurrentCorporation(corps[0]);
          localStorage.setItem('currentCorporationId', corps[0].id.toString());
        }
      } else if (corps.length > 0) {
        setCurrentCorporation(corps[0]);
        localStorage.setItem('currentCorporationId', corps[0].id.toString());
      }
    } catch (err) {
      setError(err instanceof Error ? err.message : 'Failed to load corporations');
    } finally {
      setLoading(false);
    }
  }, []);

  const selectCorporation = async (id: number) => {
    try {
      setLoading(true);
      setError(null);
      const corporation = await corporationService.getCorporationById(id);
      setCurrentCorporation(corporation);
      localStorage.setItem('currentCorporationId', id.toString());
      
      // Load dashboard for selected corporation
      await refreshDashboard();
    } catch (err) {
      setError(err instanceof Error ? err.message : 'Failed to select corporation');
    } finally {
      setLoading(false);
    }
  };

  const createCorporation = async (data: any): Promise<Corporation> => {
    try {
      setLoading(true);
      setError(null);
      const newCorporation = await corporationService.createCorporation(data);
      setCorporations(prev => [...prev, newCorporation]);
      setCurrentCorporation(newCorporation);
      localStorage.setItem('currentCorporationId', newCorporation.id.toString());
      return newCorporation;
    } catch (err) {
      setError(err instanceof Error ? err.message : 'Failed to create corporation');
      throw err;
    } finally {
      setLoading(false);
    }
  };

  const refreshDashboard = async () => {
    if (!currentCorporation) return;
    
    try {
      const dashboard = await corporationService.getCorporationDashboard(currentCorporation.id);
      setCurrentDashboard(dashboard);
    } catch (err) {
      console.error('Failed to load corporation dashboard:', err);
      // Don't set error here as dashboard is optional
    }
  };

  // Load corporations when user changes
  useEffect(() => {
    if (user) {
      loadCorporations();
    } else {
      setCorporations([]);
      setCurrentCorporation(null);
      setCurrentDashboard(null);
    }
  }, [user]);

  // Load dashboard when current corporation changes
  useEffect(() => {
    if (currentCorporation) {
      refreshDashboard();
    } else {
      setCurrentDashboard(null);
    }
  }, [currentCorporation]);

  return (
    <CorporationContext.Provider value={{
      corporations,
      currentCorporation,
      currentDashboard,
      loading,
      error,
      loadCorporations,
      selectCorporation,
      createCorporation,
      refreshDashboard
    }}>
      {children}
    </CorporationContext.Provider>
  );
};

export const useCorporation = () => {
  const context = useContext(CorporationContext);
  if (!context) {
    throw new Error('useCorporation must be used within CorporationProvider');
  }
  return context;
};
