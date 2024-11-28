/** @type {import('tailwindcss').Config} */
module.exports = {
  content: [
    "./src/**/*.{html,ts}",
  ],
  theme: {
    colors: {
      'primary': 'var(--primary)',
      'secondary': 'var(--secondary)',
      'accent': 'var(--accent)',
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
        'primary': 'var(--bg_primary)',
        'secondary': 'var(--bg_secondary)',
        'accent': 'var(--bg_accent)',
        'accent_alt': 'var(--bg_accent_alt)',
        'capture_button_primary': 'var(--capture_button_primary)',
        'capture_button_primary_hover': 'var(--capture_button_primary_hover)',
        'capture_button_secondary': 'var(--capture_button_secondary)',
        'capture_button_secondary_hover': 'var(--capture_button_secondary_hover)',
      },
      backgroundImage: {
      },
      textColor: {
        'primary': 'var(--text_primary)',
        'secondary': 'var(--text_secondary)',
        'accent': 'var(--text_accent)',
        'accent_alt': 'var(--text_accent_alt)',
      },
      fontFamily: {
        'inter': 'Inter',
      }
    },
  },
  plugins: [],
}

