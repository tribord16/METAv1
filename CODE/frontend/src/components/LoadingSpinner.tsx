import React from 'react';

interface LoadingSpinnerProps {
  size?: 'small' | 'medium' | 'large';
  message?: string;
  className?: string;
}

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

interface FullPageLoadingProps {
  message?: string;
}

export const FullPageLoading: React.FC<FullPageLoadingProps> = ({ 
  message = 'Loading...' 
}) => {
  return (
    <div className="min-h-screen flex items-center justify-center bg-gray-50 dark:bg-gray-900">
      <LoadingSpinner size="large" message={message} />
    </div>
  );
};
