/**
 * @file DashboardPage.tsx
 * @brief Main dashboard page for MetaLeague frontend
 *
 * ROLE: Displays corporation and user stats, and provides navigation to key features.
 * PURPOSE: Centralizes all dashboard logic and data display for the user.
 * DEPENDENCIES: React, CorporationContext, Navbar, utils
 *
 * TODOs:
 *   - [ ] Add loading and error states for dashboard data
 *   - [ ] Add tests for dashboard rendering and logic
 *   - [ ] Add accessibility improvements
 *   - [ ] Add analytics/logging for dashboard usage
 *   - [ ] Add role-based dashboard widgets
 *
 * Patterns: Context usage, conditional rendering, effect hooks
 */
import React, { useEffect } from 'react';
import { useAuth } from '../../context/AuthContext';
import { useCorporation } from '../../context/CorporationContext';
import { useNavigate } from 'react-router-dom';
import { Navbar } from '../../components';
import { formatCurrency, formatDate } from '../../utils';

// DashboardPage: Main dashboard for authenticated users
export const DashboardPage: React.FC = () => {
  const { currentCorporation, currentDashboard, refreshDashboard } = useCorporation();
  // Helper to safely access nested dashboard fields
  const safe = <T,>(value: T | undefined | null, fallback: T) =>
    value !== undefined && value !== null ? value : fallback;
  const navigate = useNavigate();

  // On mount or when corporation changes, refresh dashboard data
  useEffect(() => {
    if (currentCorporation) {
      refreshDashboard();
    }
  }, [currentCorporation, refreshDashboard]);

  if (!currentCorporation) {
    // TODO: UX - show message before redirecting
    return (
      <div className="min-h-screen flex items-center justify-center bg-gray-50">
        <div className="text-center">
          <h2 className="text-2xl font-bold text-gray-900 mb-4">No Corporation Selected</h2>
          <button
            onClick={() => navigate('/corporation/select')}
            className="bg-blue-600 hover:bg-blue-700 text-white px-4 py-2 rounded-md"
          >
            Select Corporation
          </button>
        </div>
      </div>
    );
  }

  return (
    <div className="min-h-screen bg-gray-900">
      <Navbar />
      {/* Main content */}
      <main className="max-w-7xl mx-auto py-6 sm:px-6 lg:px-8">
        <div className="px-4 py-6 sm:px-0">
          {/* Corporation Overview */}
          <div className="bg-gray-800 rounded-lg shadow p-6 mb-6">
            <h2 className="text-2xl font-bold text-white mb-6">Corporation Overview</h2>
            <div className="grid grid-cols-1 md:grid-cols-4 gap-6">
              <div className="text-center">
                <div className="text-3xl font-bold text-green-400 mb-2">
                  {formatCurrency(safe(currentCorporation?.budget, 0))}
                </div>
                <div className="text-gray-400">Current Budget</div>
              </div>
              <div className="text-center">
                <div className="text-3xl font-bold text-yellow-400 mb-2">
                  {safe(currentCorporation?.reputation, 0)}/100
                </div>
                <div className="text-gray-400">Reputation</div>
              </div>
              <div className="text-center">
                <div className="text-3xl font-bold text-blue-400 mb-2">
                  {safe(currentCorporation?.current_season, 'S1')}
                </div>
                <div className="text-gray-400">Current Season</div>
              </div>
              <div className="text-center">
                <div className="text-lg font-bold text-gray-300 mb-2">
                  {formatDate(safe(currentCorporation?.created_at, ''))}
                </div>
                <div className="text-gray-400">Founded</div>
              </div>
            </div>
          </div>
          {/* Active Divisions */}
          <div className="bg-gray-800 rounded-lg shadow p-6 mb-6">
            <h3 className="text-xl font-bold text-white mb-4">Active Divisions</h3>
            <div className="grid grid-cols-2 md:grid-cols-4 gap-4">
              {currentCorporation?.esports_active && (
                <div className="bg-blue-900 p-4 rounded-lg border border-blue-600">
                  <div className="text-2xl mb-2">🎮</div>
                  <div className="text-white font-semibold">Esports</div>
                  <div className="text-blue-300 text-sm">MOBA, FPS, Strategy</div>
                </div>
              )}
              {currentCorporation?.racing_active && (
                <div className="bg-red-900 p-4 rounded-lg border border-red-600">
                  <div className="text-2xl mb-2">🏁</div>
                  <div className="text-white font-semibold">Racing</div>
                  <div className="text-red-300 text-sm">Hover F1, Drone Racing</div>
                </div>
              )}
              {currentCorporation?.tactical_active && (
                <div className="bg-green-900 p-4 rounded-lg border border-green-600">
                  <div className="text-2xl mb-2">⚔️</div>
                  <div className="text-white font-semibold">Tactical</div>
                  <div className="text-green-300 text-sm">Mech Battles, Warfare</div>
                </div>
              )}
              {currentCorporation?.innovation_active && (
                <div className="bg-purple-900 p-4 rounded-lg border border-purple-600">
                  <div className="text-2xl mb-2">🧬</div>
                  <div className="text-white font-semibold">Innovation</div>
                  <div className="text-purple-300 text-sm">R&D, Technology</div>
                </div>
              )}
            </div>
          </div>
          {/* Dashboard Data (if available) */}
          {currentDashboard ? (
            <>
              {/* Financial Summary */}
              <div className="bg-gray-800 rounded-lg shadow p-6 mb-6">
                <h3 className="text-xl font-bold text-white mb-4">Financial Overview</h3>
                <div className="grid grid-cols-1 md:grid-cols-3 gap-6">
                  <div>
                    <div className="text-sm text-gray-400 mb-1">Monthly Income</div>
                    <div className="text-xl font-bold text-green-400">
                      {formatCurrency(safe(currentDashboard?.financial_summary?.monthly_income, 0))}
                    </div>
                  </div>
                  <div>
                    <div className="text-sm text-gray-400 mb-1">Monthly Expenses</div>
                    <div className="text-xl font-bold text-red-400">
                      {formatCurrency(safe(currentDashboard?.financial_summary?.monthly_expenses, 0))}
                    </div>
                  </div>
                  <div>
                    <div className="text-sm text-gray-400 mb-1">Net Monthly</div>
                    <div className={`text-xl font-bold ${
                      safe(currentDashboard?.financial_summary?.net_monthly, 0) >= 0 ? 'text-green-400' : 'text-red-400'
                    }`}>
                      {formatCurrency(safe(currentDashboard?.financial_summary?.net_monthly, 0))}
                    </div>
                  </div>
                </div>
              </div>
              {/* Team Summary */}
              <div className="bg-gray-800 rounded-lg shadow p-6 mb-6">
                <h3 className="text-xl font-bold text-white mb-4">Team Status</h3>
                <div className="grid grid-cols-1 md:grid-cols-4 gap-6">
                  <div>
                    <div className="text-sm text-gray-400 mb-1">Total Players</div>
                    <div className="text-xl font-bold text-white">
                      {safe(currentDashboard?.team_summary?.total_players, 0)}
                    </div>
                  </div>
                  <div>
                    <div className="text-sm text-gray-400 mb-1">Average Rating</div>
                    <div className="text-xl font-bold text-yellow-400">
                      {safe(currentDashboard?.team_summary?.average_rating, 0).toFixed(1)}
                    </div>
                  </div>
                  <div>
                    <div className="text-sm text-gray-400 mb-1">Total Salary</div>
                    <div className="text-xl font-bold text-red-400">
                      {formatCurrency(safe(currentDashboard?.team_summary?.total_salary, 0))}
                    </div>
                  </div>
                  <div>
                    <div className="text-sm text-gray-400 mb-1">Team Chemistry</div>
                    <div className="text-xl font-bold text-blue-400">
                      {safe(currentDashboard?.team_summary?.team_chemistry, 0).toFixed(1)}%
                    </div>
                  </div>
                </div>
                {Array.isArray(currentDashboard?.team_summary?.missing_positions) && currentDashboard.team_summary.missing_positions.length > 0 && (
                  <div className="mt-4 p-3 bg-yellow-900 border border-yellow-600 rounded">
                    <div className="text-yellow-200 text-sm">
                      Missing positions: {currentDashboard.team_summary.missing_positions.join(', ')}
                    </div>
                  </div>
                )}
              </div>
            </>
          ) : (
            /* Placeholder content when no dashboard data */
            <div className="grid grid-cols-1 md:grid-cols-3 gap-6">
              <div className="bg-gray-800 rounded-lg shadow p-6">
                <h4 className="font-medium text-white mb-2">Teams</h4>
                <p className="text-gray-400 text-sm">Manage your esports teams</p>
                <p className="text-xs text-gray-500 mt-2">Coming Soon</p>
              </div>
              <div className="bg-gray-800 rounded-lg shadow p-6">
                <h4 className="font-medium text-white mb-2">Players</h4>
                <p className="text-gray-400 text-sm">Recruit and manage players</p>
                <p className="text-xs text-gray-500 mt-2">Coming Soon</p>
              </div>
              <div className="bg-gray-800 rounded-lg shadow p-6">
                <h4 className="font-medium text-white mb-2">Competitions</h4>
                <p className="text-gray-400 text-sm">Join tournaments and leagues</p>
                <p className="text-xs text-gray-500 mt-2">Coming Soon</p>
              </div>
            </div>
          )}
        </div>
      </main>
    </div>
  );
};