/**
 * @file useAsync.ts
 * @brief Custom React hook for async data fetching/state
 *
 * ROLE: Simplifies async logic in components (loading, error, data, refetch)
 * PURPOSE: Centralizes async state management for API calls or async tasks
 * DEPENDENCIES: React (useState, useEffect, useCallback)
 *
 * TODOs:
 *   - [ ] Add cancellation support to avoid state updates on unmounted components
 *   - [ ] Add retry/backoff logic for failed requests
 *   - [ ] Add tests for edge cases (errors, rapid refetch, etc.)
 *   - [ ] Add support for aborting previous requests
 *
 * Patterns: Custom hook, stateful async, dependency array
 */

import { useState, useEffect, useCallback } from 'react';

interface UseAsyncState<T> {
  data: T | null;
  loading: boolean;
  error: string | null;
}

// useAsync: hook to manage async state (data, loading, error, refetch)
export function useAsync<T>(
  asyncFunction: () => Promise<T>,
  dependencies: any[] = []
): UseAsyncState<T> & { refetch: () => Promise<void> } {
  const [state, setState] = useState<UseAsyncState<T>>({
    data: null,
    loading: true,
    error: null,
  });

  // execute: runs the async function and updates state
  const execute = useCallback(async () => {
    setState(prev => ({ ...prev, loading: true, error: null }));
    
    try {
      const result = await asyncFunction();
      setState({ data: result, loading: false, error: null });
    } catch (error) {
      setState({
        data: null,
        loading: false,
        error: error instanceof Error ? error.message : 'An unknown error occurred',
      });
    }
  }, dependencies);

  // Run on mount and when dependencies change
  useEffect(() => {
    execute();
  }, [execute]);

  return {
    ...state,
    refetch: execute,
  };
}

// useAsyncCallback: hook for async logic in event handlers or callbacks
export function useAsyncCallback<T, Args extends any[]>(
  asyncFunction: (...args: Args) => Promise<T>
): [
  (...args: Args) => Promise<void>,
  UseAsyncState<T>
] {
  const [state, setState] = useState<UseAsyncState<T>>({
    data: null,
    loading: false,
    error: null,
  });

  const execute = useCallback(async (...args: Args) => {
    setState(prev => ({ ...prev, loading: true, error: null }));
    
    try {
      const result = await asyncFunction(...args);
      setState({ data: result, loading: false, error: null });
    } catch (error) {
      setState({
        data: null,
        loading: false,
        error: error instanceof Error ? error.message : 'An unknown error occurred',
      });
      throw error; // Re-throw so calling code can handle if needed
    }
  }, [asyncFunction]);

  return [execute, state];
}
