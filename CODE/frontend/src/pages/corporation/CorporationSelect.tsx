/**
 * @file CorporationSelect.tsx
 * @brief Page de sélection de corporation (équivalent "Load Game")
 *
 * Rôle :
 *   - Affiche la liste des corporations de l'utilisateur connecté
 *   - Permet de sélectionner une corporation pour accéder au dashboard
 *   - Permet de créer une nouvelle corporation
 *   - Affiche des informations rapides (budget, réputation, saison, dernière session)
 *
 * Dépendances :
 *   - useCorporation (context global)
 *   - Corporation (type)
 *   - React Router (navigation)
 *
 * UX :
 *   - Affiche un loader pendant le chargement
 *   - Affiche les erreurs éventuelles
 *   - TODO: Améliorer l'accessibilité (focus, aria, etc.)
 *   - TODO: Ajouter des tests unitaires et d'intégration
 */

import React, { useState, useEffect } from 'react';
import { useNavigate } from 'react-router-dom';
import { Corporation } from '../../types';
import { useCorporation } from '../../context/CorporationContext';

interface CorporationSelectProps {}

/**
 * Composant principal de sélection de corporation
 * @returns JSX.Element
 */
const CorporationSelect: React.FC<CorporationSelectProps> = () => {
    const { corporations, loading, error, loadCorporations, selectCorporation } = useCorporation();
    const navigate = useNavigate();

    // Chargement des corporations à l'arrivée sur la page
    useEffect(() => {
        loadCorporations();
        // TODO: Ajouter gestion d'annulation si le composant est démonté
    }, [loadCorporations]);

    /**
     * Handler pour sélectionner une corporation et naviguer vers le dashboard
     * @param corpId ID de la corporation à sélectionner
     */
    const handleSelectCorporation = async (corpId: number) => {
        try {
            await selectCorporation(corpId);
            navigate('/dashboard');
        } catch (err) {
            // TODO: Afficher une notification utilisateur (snackbar/toast)
            console.error('Failed to select corporation:', err);
        }
    };

    /**
     * Handler pour naviguer vers la page de création de corporation
     */
    const handleCreateNew = () => {
        navigate('/corporation/create');
    };

    // Affichage d'un loader pendant le chargement
    if (loading) {
        return (
            <div className="min-h-screen bg-gray-900 flex items-center justify-center">
                <div className="text-white text-xl">Loading your corporations...</div>
            </div>
        );
    }

    return (
        <div className="min-h-screen bg-gray-900 p-8">
            <div className="max-w-6xl mx-auto">
                <h1 className="text-4xl font-bold text-white mb-8 text-center">
                    MetaLeague Manager
                </h1>
                
                {error && (
                    <div className="bg-red-600 text-white p-4 rounded mb-6">
                        {/* TODO: Afficher les erreurs de façon plus UX (snackbar/toast) */}
                        {error}
                    </div>
                )}

                <div className="grid grid-cols-1 lg:grid-cols-2 xl:grid-cols-3 gap-6">
                    {/* Bouton créer nouvelle corporation */}
                    <div 
                        onClick={handleCreateNew}
                        className="bg-blue-600 hover:bg-blue-700 cursor-pointer transition-colors p-6 rounded-lg border-2 border-dashed border-blue-400"
                    >
                        <div className="text-center">
                            <div className="text-6xl mb-4">+</div>
                            <h3 className="text-xl font-bold text-white mb-2">
                                New Corporation
                            </h3>
                            <p className="text-blue-200">
                                Start your esports empire
                            </p>
                        </div>
                    </div>

                    {/* Liste des corporations existantes */}
                    {corporations.map((corp) => (
                        <CorporationCard 
                            key={corp.id}
                            corporation={corp}
                            onSelect={() => handleSelectCorporation(corp.id)}
                        />
                    ))}
                </div>

                {corporations.length === 0 && !loading && (
                    <div className="text-center text-gray-400 mt-12">
                        <h2 className="text-2xl mb-4">No corporations yet</h2>
                        <p>Create your first corporation to start playing!</p>
                        {/* TODO: Ajouter un call-to-action plus visible pour créer une corporation */}
                    </div>
                )}
            </div>
        </div>
    );
};

interface CorporationCardProps {
    corporation: Corporation;
    onSelect: () => void;
}

/**
 * Carte d'affichage d'une corporation
 * @param corporation Données de la corporation à afficher
 * @param onSelect Callback pour sélectionner la corporation
 * @returns JSX.Element
 *
 * TODO: Centraliser le formatBudget dans un utilitaire commun (formatters)
 * TODO: Centraliser le formatLastPlayed dans un utilitaire commun (dateUtils)
 * TODO: Ajouter un tooltip sur le nom si trop long
 * TODO: Gérer les cas où les champs sont manquants (name, season, last_played)
 * TODO: Ajouter des tests unitaires pour ce composant
 * TODO: Ajouter l'accessibilité (aria, focus, etc.)
 */
const CorporationCard: React.FC<CorporationCardProps> = ({ corporation, onSelect }) => {
    // TODO: Centraliser ce formatter dans un utilitaire commun si besoin
    const formatBudget = (amount?: number) => {
        return new Intl.NumberFormat('en-US', {
            style: 'currency',
            currency: 'EUR',
            minimumFractionDigits: 0,
            maximumFractionDigits: 0
        }).format(amount || 0);
    };

    // TODO: Gérer le cas où la date n'est pas fournie ou mal formée
    const formatLastPlayed = (date?: string) => {
        if (!date) return 'N/A';
        const lastPlayed = new Date(date);
        if (isNaN(lastPlayed.getTime())) return 'N/A';
        const now = new Date();
        const diffMs = now.getTime() - lastPlayed.getTime();
        const diffDays = Math.floor(diffMs / (1000 * 60 * 60 * 24));
        if (diffDays === 0) return 'Today';
        if (diffDays === 1) return 'Yesterday';
        return `${diffDays} days ago`;
    };

    /**
     * Retourne la couleur de réputation selon le score
     * @param reputation Score de réputation (0-100)
     */
    const getReputationColor = (reputation: number) => {
        if (reputation >= 80) return 'text-green-400';
        if (reputation >= 60) return 'text-yellow-400';
        if (reputation >= 40) return 'text-orange-400';
        return 'text-red-400';
    };

    return (
        <div 
            onClick={onSelect}
            className="bg-gray-800 hover:bg-gray-700 cursor-pointer transition-colors p-6 rounded-lg border border-gray-600"
        >
            {/* TODO: Ajouter un tooltip sur le nom si trop long */}
            <h3 className="text-xl font-bold text-white mb-4">{corporation.name || 'No name' /* TODO: handle missing name */}</h3>
            <div className="space-y-3">
                <div className="flex justify-between">
                    <span className="text-gray-400">Budget:</span>
                    <span className="text-green-400 font-semibold">
                        {formatBudget(corporation.budget)}
                    </span>
                </div>
                <div className="flex justify-between">
                    <span className="text-gray-400">Reputation:</span>
                    <span className={`font-semibold ${getReputationColor(corporation.reputation || 0)}`}>
                        {(corporation.reputation ?? 0)}/100
                    </span>
                </div>
                <div className="flex justify-between">
                    <span className="text-gray-400">Season:</span>
                    <span className="text-white">{corporation.current_season || 'N/A' /* TODO: handle missing season */}</span>
                </div>
                <div className="flex justify-between">
                    <span className="text-gray-400">Last played:</span>
                    <span className="text-gray-300">
                        {formatLastPlayed(corporation.last_played) /* TODO: handle missing last_played */}
                    </span>
                </div>
            </div>
            <div className="mt-4 pt-4 border-t border-gray-600">
                <div className="flex space-x-2">
                    {corporation.esports_active && (
                        <span className="bg-blue-600 text-xs px-2 py-1 rounded">Esports</span>
                    )}
                    {corporation.racing_active && (
                        <span className="bg-red-600 text-xs px-2 py-1 rounded">Racing</span>
                    )}
                    {corporation.tactical_active && (
                        <span className="bg-green-600 text-xs px-2 py-1 rounded">Tactical</span>
                    )}
                    {corporation.innovation_active && (
                        <span className="bg-purple-600 text-xs px-2 py-1 rounded">Innovation</span>
                    )}
                </div>
            </div>
        </div>
    );
};

export default CorporationSelect;