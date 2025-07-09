/**
 * @file Header.tsx
 * @brief Header component for MetaLeague frontend
 * 
 */


import React from 'react';
import { Link } from 'react-router-dom';
import { useAuth } from '../context/AuthContext';
import { useCorporation } from '../context/CorporationContext';



/**
 * Header component that displays the app title and user actions
 * @returns JSX.Element
 */
export const Header: React.FC = () => {
    const { user, logout } = useAuth();
    const { currentCorporation } = useCorporation();

    return (
        <header className="bg-gray-800 text-white">
            <div className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8">
                <div className="flex justify-between items-center h-16">
                    <Link to="/" className="text-xl font-bold">
                        MetaLeague Manager
                    </Link>
                    <div className="flex items-center space-x-4">
                        {user && (
                            <>
                                <span>{user.username}</span>
                                {currentCorporation && (
                                    <span className="text-blue-400">{currentCorporation.name}</span>
                                )}
                                <button onClick={logout} className="text-red-400 hover:text-red-300">
                                    Logout
                                </button>
                            </>
                        )}
                    </div>
                </div>
            </div>
        </header>
    );
}