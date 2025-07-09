/**
 * @file CreateCorporation.tsx
 * @brief Page création de corporation - assistant "New Game"
 * @author MetaLeague Frontend Team
 * @date 2025
 * @version 1.0
 *
 * RESPONSABILITÉS :
 * - Assistant création corporation étape par étape
 * - Validation en temps réel des données
 * - Choix des divisions à activer
 * - Calcul du budget initial selon choix
 */

import React, { useState } from 'react';
import { useNavigate } from 'react-router-dom';
import { CreateCorporationRequest, corporationService } from '../../services/corporationService';
import { useCorporation } from '../../context/CorporationContext';
import { ErrorList, LoadingSpinner } from '../../components';
import { validateCorporationName, formatCurrency } from '../../utils';

const CreateCorporation: React.FC = () => {
    const [step, setStep] = useState(1);
    const [formData, setFormData] = useState<CreateCorporationRequest>({
        name: '',
        initial_budget: 1000000,
        esports_active: true,
        racing_active: false,
        tactical_active: false,
        innovation_active: false
    });
    const [errors, setErrors] = useState<string[]>([]);
    const [loading, setLoading] = useState(false);
    const navigate = useNavigate();
    const { createCorporation } = useCorporation();

    const handleNameChange = (e: React.ChangeEvent<HTMLInputElement>) => {
        setFormData({ ...formData, name: e.target.value });
        validateName(e.target.value);
    };

    const validateName = (name: string) => {
        const newErrors = validateCorporationName(name);
        setErrors(newErrors);
    };

    const handleDivisionToggle = (division: 'racing_active' | 'tactical_active' | 'innovation_active') => {
        const newFormData = { ...formData };
        newFormData[division] = !newFormData[division];
        
        // Calculer nouveau budget selon divisions
        const activeDivisions = [
            newFormData.esports_active,
            newFormData.racing_active,
            newFormData.tactical_active,
            newFormData.innovation_active
        ].filter(Boolean).length;
        
        newFormData.initial_budget = activeDivisions * 500000 + 500000; // Base 500k + 500k par division
        
        setFormData(newFormData);
    };

    const handleSubmit = async () => {
        if (errors.length > 0) return;
        
        try {
            setLoading(true);
            const corporation = await createCorporation(formData);
            
            // Rediriger vers le dashboard
            navigate('/dashboard');
        } catch (err) {
            console.error('Failed to create corporation:', err);
            setErrors(['Failed to create corporation. Please try again.']);
        } finally {
            setLoading(false);
        }
    };

    const canProceed = () => {
        if (step === 1) return formData.name.length >= 3 && errors.length === 0;
        if (step === 2) return true; // Au moins esports toujours actif
        return true;
    };

    return (
        <div className="min-h-screen bg-gray-900 p-8">
            <div className="max-w-4xl mx-auto">
                <h1 className="text-4xl font-bold text-white mb-8 text-center">
                    Create Your Corporation
                </h1>

                {/* Progress bar */}
                <div className="mb-8">
                    <div className="flex justify-between text-sm text-gray-400 mb-2">
                        <span className={step >= 1 ? 'text-blue-400' : ''}>Name</span>
                        <span className={step >= 2 ? 'text-blue-400' : ''}>Divisions</span>
                        <span className={step >= 3 ? 'text-blue-400' : ''}>Confirm</span>
                    </div>
                    <div className="bg-gray-700 h-2 rounded">
                        <div 
                            className="bg-blue-600 h-2 rounded transition-all duration-300"
                            style={{ width: `${(step / 3) * 100}%` }}
                        />
                    </div>
                </div>

                {errors.length > 0 && (
                    <ErrorList errors={errors} onDismiss={() => setErrors([])} />
                )}

                {/* Step 1: Name */}
                {step === 1 && (
                    <div className="bg-gray-800 p-8 rounded-lg">
                        <h2 className="text-2xl font-bold text-white mb-6">Choose Your Corporation Name</h2>
                        
                        <div className="mb-6">
                            <label className="block text-gray-300 mb-2">Corporation Name</label>
                            <input
                                type="text"
                                value={formData.name}
                                onChange={handleNameChange}
                                placeholder="e.g., Team Quantum, Digital Legends, Cyber Dragons"
                                className="w-full p-3 bg-gray-700 text-white rounded border border-gray-600 focus:border-blue-500 focus:outline-none"
                                maxLength={50}
                            />
                            <div className="text-sm text-gray-400 mt-1">
                                {formData.name.length}/50 characters
                            </div>
                        </div>

                        <div className="text-gray-400 mb-6">
                            <p>Your corporation name will be displayed in:</p>
                            <ul className="list-disc list-inside mt-2 space-y-1">
                                <li>Competition broadcasts and results</li>
                                <li>Transfer market and player negotiations</li>
                                <li>Sponsor partnerships and contracts</li>
                                <li>Global leaderboards and rankings</li>
                            </ul>
                        </div>
                    </div>
                )}

                {/* Step 2: Divisions */}
                {step === 2 && (
                    <div className="bg-gray-800 p-8 rounded-lg">
                        <h2 className="text-2xl font-bold text-white mb-6">Select Your Divisions</h2>
                        
                        <div className="grid grid-cols-1 md:grid-cols-2 gap-6">
                            <DivisionCard
                                title="Esports Division"
                                description="MOBA, FPS, and Strategy games. Start here for your esports empire."
                                cost="500,000€"
                                active={formData.esports_active}
                                required={true}
                                onToggle={() => {}} // Esports always active
                                icon="🎮"
                            />
                            
                            <DivisionCard
                                title="Racing Division"
                                description="Hover F1, Drone Racing, and Autonomous competitions."
                                cost="500,000€"
                                active={formData.racing_active}
                                onToggle={() => handleDivisionToggle('racing_active')}
                                icon="🏁"
                            />
                            
                            <DivisionCard
                                title="Tactical Division"
                                description="Mech Battles, Strategy Warfare, and Team Raids."
                                cost="500,000€"
                                active={formData.tactical_active}
                                onToggle={() => handleDivisionToggle('tactical_active')}
                                icon="⚔️"
                            />
                            
                            <DivisionCard
                                title="Innovation Division"
                                description="R&D, Patent Racing, and Technology Development."
                                cost="500,000€"
                                active={formData.innovation_active}
                                onToggle={() => handleDivisionToggle('innovation_active')}
                                icon="🧬"
                            />
                        </div>

                        <div className="mt-8 p-4 bg-gray-700 rounded">
                            <div className="flex justify-between text-lg">
                                <span className="text-gray-300">Total Initial Budget:</span>
                                <span className="text-green-400 font-bold">
                                    {new Intl.NumberFormat('en-US', {
                                        style: 'currency',
                                        currency: 'EUR',
                                        minimumFractionDigits: 0
                                    }).format(formData.initial_budget)}
                                </span>
                            </div>
                        </div>
                    </div>
                )}

                {/* Step 3: Confirm */}
                {step === 3 && (
                    <div className="bg-gray-800 p-8 rounded-lg">
                        <h2 className="text-2xl font-bold text-white mb-6">Confirm Your Corporation</h2>
                        
                        <div className="space-y-4 mb-8">
                            <div className="flex justify-between">
                                <span className="text-gray-400">Name:</span>
                                <span className="text-white font-semibold">{formData.name}</span>
                            </div>
                            
                            <div className="flex justify-between">
                                <span className="text-gray-400">Initial Budget:</span>
                                <span className="text-green-400 font-semibold">
                                    {new Intl.NumberFormat('en-US', {
                                        style: 'currency',
                                        currency: 'EUR',
                                        minimumFractionDigits: 0
                                    }).format(formData.initial_budget)}
                                </span>
                            </div>
                            
                            <div>
                                <span className="text-gray-400">Active Divisions:</span>
                                <div className="flex flex-wrap gap-2 mt-2">
                                    {formData.esports_active && (
                                        <span className="bg-blue-600 px-3 py-1 rounded text-sm">🎮 Esports</span>
                                    )}
                                    {formData.racing_active && (
                                        <span className="bg-red-600 px-3 py-1 rounded text-sm">🏁 Racing</span>
                                    )}
                                    {formData.tactical_active && (
                                        <span className="bg-green-600 px-3 py-1 rounded text-sm">⚔️ Tactical</span>
                                    )}
                                    {formData.innovation_active && (
                                        <span className="bg-purple-600 px-3 py-1 rounded text-sm">🧬 Innovation</span>
                                    )}
                                </div>
                            </div>
                        </div>

                        <div className="bg-blue-900 p-4 rounded mb-6">
                            <h3 className="text-blue-300 font-semibold mb-2">What happens next?</h3>
                            <ul className="text-blue-200 text-sm space-y-1">
                                <li>• Your corporation will be created with the selected budget</li>
                                <li>• You'll enter the game in Week 1 of Season 1</li>
                                <li>• You can start recruiting players immediately</li>
                                <li>• Additional divisions can be unlocked later</li>
                            </ul>
                        </div>
                    </div>
                )}

                {/* Navigation buttons */}
                <div className="flex justify-between mt-8">
                    <button
                        onClick={() => step > 1 ? setStep(step - 1) : navigate('/corporation/select')}
                        className="px-6 py-3 bg-gray-600 hover:bg-gray-700 text-white rounded transition-colors"
                    >
                        {step === 1 ? 'Cancel' : 'Previous'}
                    </button>
                    
                    <button
                        onClick={() => step < 3 ? setStep(step + 1) : handleSubmit()}
                        disabled={!canProceed() || loading}
                        className="px-6 py-3 bg-blue-600 hover:bg-blue-700 disabled:bg-gray-600 disabled:cursor-not-allowed text-white rounded transition-colors flex items-center space-x-2"
                    >
                        {loading && <LoadingSpinner size="small" message="" />}
                        <span>{loading ? 'Creating...' : step < 3 ? 'Next' : 'Create Corporation'}</span>
                    </button>
                </div>
            </div>
        </div>
    );
};

interface DivisionCardProps {
    title: string;
    description: string;
    cost: string;
    active: boolean;
    required?: boolean;
    onToggle: () => void;
    icon: string;
}

const DivisionCard: React.FC<DivisionCardProps> = ({
    title, description, cost, active, required, onToggle, icon
}) => {
    return (
        <div 
            onClick={required ? undefined : onToggle}
            className={`p-6 rounded-lg border-2 transition-all ${
                active 
                    ? 'bg-blue-900 border-blue-500' 
                    : 'bg-gray-700 border-gray-600 hover:border-gray-500'
            } ${required ? 'cursor-default' : 'cursor-pointer'}`}
        >
            <div className="flex items-center justify-between mb-3">
                <div className="flex items-center space-x-3">
                    <span className="text-2xl">{icon}</span>
                    <h3 className="text-lg font-semibold text-white">{title}</h3>
                </div>
                {required ? (
                    <span className="text-xs bg-yellow-600 px-2 py-1 rounded">REQUIRED</span>
                ) : (
                    <div className={`w-6 h-6 rounded border-2 ${
                        active ? 'bg-blue-500 border-blue-500' : 'border-gray-400'
                    }`}>
                        {active && <div className="text-white text-sm text-center">✓</div>}
                    </div>
                )}
            </div>
            
            <p className="text-gray-300 text-sm mb-3">{description}</p>
            
            <div className="text-green-400 font-semibold">{cost}</div>
        </div>
    );
};

export default CreateCorporation;