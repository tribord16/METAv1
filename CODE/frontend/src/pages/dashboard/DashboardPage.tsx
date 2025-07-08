import React from 'react';
import { useAuth } from '../../context/AuthContext';

export const DashboardPage: React.FC = () => {
  const { user, logout } = useAuth();

  return (
    <div className="min-h-screen bg-gray-50">
      {/* Header */}
      <nav className="bg-white shadow">
        <div className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8">
          <div className="flex justify-between h-16">
            <div className="flex items-center">
              <h1 className="text-xl font-semibold text-gray-900">
                MetaLeague Manager
              </h1>
            </div>
            <div className="flex items-center space-x-4">
              <span className="text-gray-700">Welcome, {user?.username}!</span>
              <button
                onClick={logout}
                className="bg-red-600 hover:bg-red-700 text-white px-4 py-2 rounded-md text-sm font-medium"
              >
                Logout
              </button>
            </div>
          </div>
        </div>
      </nav>

      {/* Main content */}
      <main className="max-w-7xl mx-auto py-6 sm:px-6 lg:px-8">
        <div className="px-4 py-6 sm:px-0">
          <div className="border-4 border-dashed border-gray-200 rounded-lg p-8">
            <div className="text-center">
              <h2 className="text-2xl font-bold text-gray-900 mb-4">
                Welcome to MetaLeague Manager!
              </h2>
              <p className="text-gray-600 mb-8">
                Your esports team management journey starts here.
              </p>
              
              {/* User info card */}
              <div className="bg-white rounded-lg shadow p-6 max-w-md mx-auto">
                <h3 className="text-lg font-medium text-gray-900 mb-4">Your Profile</h3>
                <div className="space-y-2 text-left">
                  <p><span className="font-medium">Username:</span> {user?.username}</p>
                  <p><span className="font-medium">Email:</span> {user?.email}</p>
                  <p><span className="font-medium">Member since:</span> {user?.created_at ? new Date(user.created_at).toLocaleDateString() : 'Unknown'}</p>
                  <p><span className="font-medium">Status:</span> 
                    <span className={`ml-2 px-2 py-1 rounded-full text-xs ${user?.is_active ? 'bg-green-100 text-green-800' : 'bg-red-100 text-red-800'}`}>
                      {user?.is_active ? 'Active' : 'Inactive'}
                    </span>
                  </p>
                </div>
              </div>

              {/* Coming soon features */}
              <div className="mt-8 grid grid-cols-1 md:grid-cols-3 gap-4">
                <div className="bg-white rounded-lg shadow p-4">
                  <h4 className="font-medium text-gray-900 mb-2">Teams</h4>
                  <p className="text-gray-600 text-sm">Manage your esports teams</p>
                  <p className="text-xs text-gray-400 mt-2">Coming in Sprint 4</p>
                </div>
                <div className="bg-white rounded-lg shadow p-4">
                  <h4 className="font-medium text-gray-900 mb-2">Players</h4>
                  <p className="text-gray-600 text-sm">Recruit and manage players</p>
                  <p className="text-xs text-gray-400 mt-2">Coming in Sprint 5</p>
                </div>
                <div className="bg-white rounded-lg shadow p-4">
                  <h4 className="font-medium text-gray-900 mb-2">Competitions</h4>
                  <p className="text-gray-600 text-sm">Join tournaments and leagues</p>
                  <p className="text-xs text-gray-400 mt-2">Coming in Sprint 8</p>
                </div>
              </div>
            </div>
          </div>
        </div>
      </main>
    </div>
  );
};