/**
 * @file Navbar.tsx
 * @brief Top navigation bar for MetaLeague frontend
 *
 * ROLE: Displays app title, current corporation, and user actions (logout, switch corp).
 * PURPOSE: Provides navigation and context for the user across all pages.
 * DEPENDENCIES: React, AuthContext, CorporationContext, react-router-dom
 *
 * TODOs:
 *   - [ ] Add user avatar and profile menu
 *   - [ ] Add responsive/mobile menu
 *   - [ ] Add tests for navigation and logout
 *   - [ ] Add accessibility improvements
 *
 * Patterns: Functional component, context usage, navigation hooks
 */
import React from 'react';
import { useAuth } from '../context/AuthContext';
import { useCorporation } from '../context/CorporationContext';
import { useNavigate } from 'react-router-dom';


// NavbarProps: Optional title for the navbar
interface NavbarProps {
  title?: string;
}


// Navbar: Shows app title, current corporation, and user actions
export const Navbar: React.FC<NavbarProps> = ({ title = "MetaLeague Manager" }) => {
  const { user, logout } = useAuth();
  const { currentCorporation, corporations } = useCorporation();
  const navigate = useNavigate();

  // Handler to switch corporation (if user has several)
  const handleSwitchCorporation = () => {
    navigate('/corporation/select');
  };

  return (
    <nav className="bg-gray-800 shadow border-b border-gray-700">
      <div className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8">
        <div className="flex justify-between h-16">
          <div className="flex items-center space-x-4">
            <h1 className="text-xl font-semibold text-white">
              {title}
            </h1>
            {currentCorporation && (
              <>
                <div className="text-gray-400">|</div>
                <div className="flex items-center space-x-2">
                  <span className="text-blue-400 font-medium">{currentCorporation.name}</span>
                  {corporations.length > 1 && (
                    <button
                      onClick={handleSwitchCorporation}
                      className="text-gray-400 hover:text-white text-sm"
                    >
                      Switch
                    </button>
                  )}
                </div>
              </>
            )}
          </div>
          <div className="flex items-center space-x-4">
            <span className="text-gray-300">Welcome, {user?.username}!</span>
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
  );
};
