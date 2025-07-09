/**
 * @file LoadingSpinner.tsx
 * @brief Loading spinner and full-page loading components for MetaLeague frontend
 *
 * ROLE: Shows a loading indicator (spinner or full page) during async operations.
 * PURPOSE: Improves UX by providing feedback during loading states.
 * DEPENDENCIES: React
 *
 * TODOs:
 *   - [ ] Add animation customization (speed, color)
 *   - [ ] Add accessibility improvements (aria-busy, etc.)
 *   - [ ] Add tests for loading states
 *   - [ ] Add i18n for loading messages
 *
 * Patterns: Functional component, props for size/message
 */

import React from 'react';

// LoadingSpinnerProps: size (small/medium/large), message, className
interface LoadingSpinnerProps {
  size?: 'small' | 'medium' | 'large';
  message?: string;
  className?: string;
}

// LoadingSpinner: shows a spinning loader with optional message
export const LoadingSpinner: React.FC<LoadingSpinnerProps> = ({ 
  size = 'medium', 
  message = 'Loading...', 
  className = '' 
}) => {
  const sizeClasses = {
    small: 'h-6 w-6',
    medium: 'h-12 w-12',
    large: 'h-32 w-32'
  };
  return (
    <div className={`flex flex-col items-center justify-center ${className}`}>
      <div 
        className={`animate-spin rounded-full border-b-2 border-blue-600 ${sizeClasses[size]}`}
      />
      {message && (
        <p className="mt-4 text-gray-600 dark:text-gray-400">{message}</p>
      )}
    </div>
  );
};

// FullPageLoadingProps: message (optional)
interface FullPageLoadingProps {
  message?: string;
}

// FullPageLoading: covers the whole page with a loading spinner
export const FullPageLoading: React.FC<FullPageLoadingProps> = ({ 
  message = 'Loading...'
}) => {
  return (
    <div className="min-h-screen flex items-center justify-center bg-gray-50 dark:bg-gray-900">
      <LoadingSpinner size="large" message={message} />
    </div>
  );
};
