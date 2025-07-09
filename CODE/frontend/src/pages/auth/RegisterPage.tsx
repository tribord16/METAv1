/**
 * @file RegisterPage.tsx
 * @brief Registration page for MetaLeague frontend
 *
 * ROLE: Provides a form for users to create a new account.
 * PURPOSE: Handles registration logic, validation, and navigation after signup.
 * DEPENDENCIES: React, AuthContext, react-router-dom
 *
 * TODOs:
 *   - [ ] Add validation for all fields (username, email, password)
 *   - [ ] Add error messages for network/server errors
 *   - [ ] Add loading spinner for async actions
 *   - [ ] Add tests for registration logic and error states
 *   - [ ] Add accessibility improvements
 *
 * Patterns: Controlled form, async submit, context usage
 */
import React, { useState } from 'react';
import { useAuth } from '../../context/AuthContext';
import { useNavigate, Link } from 'react-router-dom';


// RegisterPage: Controlled registration form with async submit and validation
export const RegisterPage: React.FC = () => {
  // username: controlled input for username
  const [username, setUsername] = useState('');
  // email: controlled input for email
  const [email, setEmail] = useState('');
  // password: controlled input for password
  const [password, setPassword] = useState('');
  // confirmPassword: controlled input for password confirmation
  const [confirmPassword, setConfirmPassword] = useState('');
  // error: error message for registration failures
  const [error, setError] = useState('');
  // loading: true while registration request is in progress
  const [loading, setLoading] = useState(false);
  const { register } = useAuth();
  const navigate = useNavigate();

  // Handles form submission and registration logic
  const handleSubmit = async (e: React.FormEvent) => {
    e.preventDefault();
    setLoading(true);
    setError('');
    // TODO: Add more robust validation (email format, password strength, etc.)
    if (password !== confirmPassword) {
      setError('Passwords do not match');
      setLoading(false);
      return;
    }
    if (password.length < 6) {
      setError('Password must be at least 6 characters');
      setLoading(false);
      return;
    }
    try {
      await register(username, email, password);
      navigate('/dashboard');
    } catch (err) {
      // TODO: Improve error handling for different error types
      setError(err instanceof Error ? err.message : 'Registration failed');
    } finally {
      setLoading(false);
    }
  };

  return (
    <div className="min-h-screen flex items-center justify-center bg-gray-50">
      <div className="max-w-md w-full space-y-8">
        <div>
          <h2 className="mt-6 text-center text-3xl font-extrabold text-gray-900">
            MetaLeague Manager
          </h2>
          <p className="mt-2 text-center text-sm text-gray-600">
            Create your account
          </p>
        </div>
        <form className="mt-8 space-y-6" onSubmit={handleSubmit}>
          {error && (
            <div className="bg-red-100 border border-red-400 text-red-700 px-4 py-3 rounded">
              {error}
            </div>
          )}
          <div className="space-y-4">
            <div>
              <input
                type="text"
                required
                value={username}
                onChange={(e) => setUsername(e.target.value)}
                className="appearance-none rounded-md relative block w-full px-3 py-2 border border-gray-300 placeholder-gray-500 text-gray-900 focus:outline-none focus:ring-indigo-500 focus:border-indigo-500"
                placeholder="Username"
                minLength={3}
              />
            </div>
            <div>
              <input
                type="email"
                required
                value={email}
                onChange={(e) => setEmail(e.target.value)}
                className="appearance-none rounded-md relative block w-full px-3 py-2 border border-gray-300 placeholder-gray-500 text-gray-900 focus:outline-none focus:ring-indigo-500 focus:border-indigo-500"
                placeholder="Email"
              />
            </div>
            <div>
              <input
                type="password"
                required
                value={password}
                onChange={(e) => setPassword(e.target.value)}
                className="appearance-none rounded-md relative block w-full px-3 py-2 border border-gray-300 placeholder-gray-500 text-gray-900 focus:outline-none focus:ring-indigo-500 focus:border-indigo-500"
                placeholder="Password"
                minLength={6}
              />
            </div>
            <div>
              <input
                type="password"
                required
                value={confirmPassword}
                onChange={(e) => setConfirmPassword(e.target.value)}
                className="appearance-none rounded-md relative block w-full px-3 py-2 border border-gray-300 placeholder-gray-500 text-gray-900 focus:outline-none focus:ring-indigo-500 focus:border-indigo-500"
                placeholder="Confirm Password"
                minLength={6}
              />
            </div>
          </div>
          <div>
            <button
              type="submit"
              disabled={loading}
              className="group relative w-full flex justify-center py-2 px-4 border border-transparent text-sm font-medium rounded-md text-white bg-indigo-600 hover:bg-indigo-700 focus:outline-none focus:ring-2 focus:ring-offset-2 focus:ring-indigo-500 disabled:opacity-50"
            >
              {loading ? 'Creating account...' : 'Sign up'}
            </button>
          </div>
          <div className="text-center">
            <Link to="/login" className="text-indigo-600 hover:text-indigo-500">
              Already have an account? Sign in
            </Link>
          </div>
        </form>
      </div>
    </div>
  );
};