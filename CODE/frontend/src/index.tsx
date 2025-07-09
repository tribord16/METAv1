/**
 * @file index.tsx
 * @brief React root file for MetaLeague frontend
 *
 * ROLE: Mounts the React app to the DOM and sets up global CSS and performance reporting.
 * PURPOSE: Entry point for the SPA, ensures strict mode and performance hooks are enabled.
 * DEPENDENCIES: React, ReactDOM, App, reportWebVitals
 *
 * TODOs:
 *   - [ ] Add error boundary at root level
 *   - [ ] Add global loading indicator for suspense
 *   - [ ] Add accessibility checks for root
 *   - [ ] Add tests for root rendering
 *
 * Patterns: ReactDOM.createRoot, StrictMode
 */
import React from 'react';
import ReactDOM from 'react-dom/client';
import './index.css';
import App from './App';
import reportWebVitals from './reportWebVitals';

const root = ReactDOM.createRoot(
  document.getElementById('root') as HTMLElement
);
root.render(
  <React.StrictMode>
    <App />
  </React.StrictMode>
);

// If you want to start measuring performance in your app, pass a function
// to log results (for example: reportWebVitals(console.log))
// or send to an analytics endpoint. Learn more: https://bit.ly/CRA-vitals
reportWebVitals();
