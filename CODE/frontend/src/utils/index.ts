/**
 * @file index.ts
 * @brief Barrel file for utility functions (MetaLeague frontend)
 *
 * ROLE: Centralizes exports for all utility functions in the project.
 * PURPOSE: Simplifies imports and improves maintainability.
 * DEPENDENCIES: All utils in this directory
 *
 * TODOs:
 *   - [ ] Add tests to ensure all utils are exported
 *   - [ ] Document each util in its own file
 *
 * Patterns: Barrel export
 */

export { formatCurrency, formatNumber, formatPercentage, formatCompactNumber } from './formatters';
export { formatDate, formatDateTime, formatRelativeTime, isToday, isYesterday } from './dateUtils';
export { validateEmail, validateUsername, validatePassword, validateCorporationName } from './validation';
