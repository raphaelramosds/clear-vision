# THIS IS A DEVELOPMENT IMAGE! SHOULD NOT BE DEPLOYED ON PRODUCTION
FROM node:22.13.1-alpine

WORKDIR /app

COPY package*.json ./

RUN npm install

COPY . .

EXPOSE 3000

CMD ["npm", "run", "dev:web"]