export const validateEmail = (email: string): boolean => {
  const emailRegex = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;
  return emailRegex.test(email);
};

export const validateUsername = (username: string): string[] => {
  const errors: string[] = [];
  
  if (username.length < 3) {
    errors.push('Username must be at least 3 characters long');
  }
  
  if (username.length > 30) {
    errors.push('Username must be less than 30 characters long');
  }
  
  if (!/^[a-zA-Z0-9_-]+$/.test(username)) {
    errors.push('Username can only contain letters, numbers, underscores, and hyphens');
  }
  
  return errors;
};

export const validatePassword = (password: string): string[] => {
  const errors: string[] = [];
  
  if (password.length < 6) {
    errors.push('Password must be at least 6 characters long');
  }
  
  if (password.length > 100) {
    errors.push('Password must be less than 100 characters long');
  }
  
  if (!/(?=.*[a-z])/.test(password)) {
    errors.push('Password must contain at least one lowercase letter');
  }
  
  if (!/(?=.*[A-Z])/.test(password)) {
    errors.push('Password must contain at least one uppercase letter');
  }
  
  if (!/(?=.*\d)/.test(password)) {
    errors.push('Password must contain at least one number');
  }
  
  return errors;
};

export const validateCorporationName = (name: string): string[] => {
  const errors: string[] = [];
  
  if (name.length < 3) {
    errors.push('Corporation name must be at least 3 characters long');
  }
  
  if (name.length > 50) {
    errors.push('Corporation name must be less than 50 characters long');
  }
  
  if (!/^[a-zA-Z0-9\s\-_.']+$/.test(name)) {
    errors.push('Corporation name contains invalid characters');
  }
  
  return errors;
};
