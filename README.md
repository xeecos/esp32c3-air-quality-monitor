# esp32c3-pm25
This is a project to measure PM2.5 with an ESP32-C3 and a SGP30 sensor.

### Recording data to a CSV file:
```
cd scripts
npm install
npm i pm2 --global
pm2 start record.js --port=COM5 --during=10
```

### Generating a plot to a PNG file:
```
cd scripts
node preview.js --name=2024-6-11_08_57_19.csv
```