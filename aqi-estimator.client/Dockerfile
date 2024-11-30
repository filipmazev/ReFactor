# Use Node.js as the base image
FROM node:alpine

# Set the working directory inside the container
WORKDIR /app

# Copy the package.json and install dependencies
COPY package.json package-lock.json ./
RUN npm install

# Copy the rest of the application code
COPY . .

# Build the Angular application
# RUN npm run build --prod
RUN npm install

# Use Nginx to serve the Angular app
FROM nginx:alpine

# Copy the build files from the build image to Nginx's public folder
# Make sure to adjust the 'dist/aqi-estimator.client' path if necessary
# COPY --from=build /app/dist/aqi-estimator.client /usr/share/nginx/html

# Expose port 80 to access the application
EXPOSE 4200

# Start Nginx
# CMD ["nginx", "-g", "daemon off;"]
CMD ["npm", "start"]