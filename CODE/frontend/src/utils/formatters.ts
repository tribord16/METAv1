/**
 * @file formatters.ts
 * @brief Utility functions for formatting numbers, currency, and percentages (MetaLeague frontend)
 *
 * ROLE: Centralizes all number/currency formatting logic for UI display
 * PURPOSE: Provides helpers for consistent and localized formatting
 * DEPENDENCIES: Intl.NumberFormat
 *
 * TODOs:
 *   - [ ] Add i18n/localization support
 *   - [ ] Add tests for edge cases (large/small numbers)
 *   - [ ] Add support for custom formats
 *
 * Patterns: Utility functions, formatter pattern
 */

export const formatCurrency = (
  amount: number,
  currency: string = 'EUR',
  locale: string = 'en-US'
) => {
  return new Intl.NumberFormat(locale, {
    style: 'currency',
    currency,
    minimumFractionDigits: 0,
    maximumFractionDigits: 0
  }).format(amount);
};

export const formatNumber = (
  value: number,
  locale: string = 'en-US'
) => {
  return new Intl.NumberFormat(locale).format(value);
};

export const formatPercentage = (
  value: number,
  decimals: number = 1,
  locale: string = 'en-US'
) => {
  return new Intl.NumberFormat(locale, {
    style: 'percent',
    minimumFractionDigits: decimals,
    maximumFractionDigits: decimals
  }).format(value / 100);
};

export const formatCompactNumber = (
  value: number,
  locale: string = 'en-US'
) => {
  return new Intl.NumberFormat(locale, {
    notation: 'compact',
    maximumFractionDigits: 1
  }).format(value);
};
