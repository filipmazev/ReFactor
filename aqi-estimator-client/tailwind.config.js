/** @type {import('tailwindcss').Config} */
module.exports = {
  content: [
    "./src/**/*.{html,ts}",
  ],
  theme: {
    colors: {
    },
    screens: {
      'xs': {'raw': '(max-width: 350px)'},
      'sm': '640px',
      'md': '1024px',
      'lg': '1400px',
      '2xl': '1930px',
      '3xl': '2208px',
      '4xl': '2560px',
      'ml': {'raw': 'only screen and (max-height: 575.98px) and (orientation: landscape)'},
    },
    extend: {
      backgroundColor: {
      },
      backgroundImage: {
      },
      textColor: {
      },
      fontFamily: {
        'inter': 'Inter',
        'alegreya': 'Alegreya',
      }
    },
  },
  plugins: [],
}

