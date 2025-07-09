/**
 * @file NotFoundPage.tsx
 * @brief 404 Not Found page for MetaLeague frontend
 *
 * ROLE: Displays a user-friendly message when a route is not found.
 * PURPOSE: Improves UX by guiding users back to valid pages.
 * DEPENDENCIES: React, react-router-dom
 *
 * TODOs:
 *   - [ ] Add illustration or mascot for branding
 *   - [ ] Add logging for 404 events
 *   - [ ] Add i18n for error messages
 *   - [ ] Add tests for navigation buttons
 *
 * Patterns: Functional component, navigation hooks
 */
import React from 'react';
import { useNavigate } from 'react-router-dom';

// NotFoundPage: 404 error page with navigation options
export const NotFoundPage: React.FC = () => {
  const navigate = useNavigate();
  // TODO: Add analytics/logging for 404 navigation
  return (
    <div className="min-h-screen bg-gray-900 flex items-center justify-center">
      <div className="text-center">
        <div className="text-6xl font-bold text-blue-500 mb-4">404</div>
        <h1 className="text-3xl font-bold text-white mb-4">Page Not Found</h1>
        <p className="text-gray-400 mb-8 max-w-md">
          The page you're looking for doesn't exist or has been moved.
        </p>
        <div className="space-x-4">
          <button
            onClick={() => navigate('/dashboard')}
            className="bg-blue-600 hover:bg-blue-700 text-white px-6 py-3 rounded-lg transition-colors"
          >
            Go to Dashboard
          </button>
          <button
            onClick={() => navigate(-1)}
            className="bg-gray-600 hover:bg-gray-700 text-white px-6 py-3 rounded-lg transition-colors"
          >
            Go Back
          </button>
        </div>
      </div>
    </div>
  );
};
