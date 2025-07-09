// src/components/Layout.tsx
import React from 'react';
import { Header } from './Header';

const Layout: React.FC<{ children: React.ReactNode }> = ({ children }) => (
  <>
    <Header />
    <main>{children}</main>
  </>
);

export default Layout;