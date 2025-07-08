/** @type {import('tailwindcss').Config} */
module.exports = {
  content: [
    "./src/**/*.{js,ts,jsx,tsx}", // 🧠 tous les fichiers React/TSX dans src/
    "./public/index.html",        // (facultatif mais utile parfois)
  ],
  theme: {
    extend: {},
  },
  plugins: [],
}
