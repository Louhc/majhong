/** @type {import('tailwindcss').Config} */
export default {
  content: [
    "./index.html",
    "./src/**/*.{js,ts,jsx,tsx}",
  ],
  theme: {
    extend: {
      colors: {
        'mahjong-green': '#1a472a',
        'mahjong-felt': '#2d5a3f',
      },
    },
  },
  plugins: [],
}
