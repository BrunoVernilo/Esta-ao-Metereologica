#include <WiFi.h>
#include <DHTesp.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
// Configurações Wi-Fi
const char* ssid = "BIGDATA";
const char* password = "yuriyuri";
LiquidCrystal_I2C lcd(0x27, 16, 2); 
#define LDR_PIN 34  // GPIO do LDR
#define PINO_DHT 25  // GPIO do DHT11
#define PINO_SOLO 13 // GPIO do sensor de umidade do solo

DHTesp dht;
WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  analogReadResolution(12); // resolução 12 bits ADC

  dht.setup(PINO_DHT, DHTesp::DHT11);

  WiFi.begin(ssid, password);
  Serial.print("Conectando-se ao WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi conectado!");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());

  server.begin();
  Wire.begin(26, 27); // SDA, SCL (pinos padrão do ESP32)
  lcd.init();       
  lcd.backlight();    
   lcd.setCursor(0, 0); 

     lcd.print(WiFi.localIP());
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    String req = client.readStringUntil('\r');
    client.read(); // lê o '\n'

    if (req.indexOf("GET /data") >= 0) {
      // Leitura dos sensores
      int ldrValue = analogRead(LDR_PIN);
      int valorSolo = analogRead(PINO_SOLO);
      TempAndHumidity dados = dht.getTempAndHumidity();

      int porcentLDR = map(ldrValue, 0, 4095, 0, 100);
      porcentLDR = constrain(porcentLDR, 0, 100);
      int porcentSolo = map(valorSolo, 0, 4095, 0, 100);
      porcentSolo = constrain(porcentSolo, 0, 100);

      // Formata JSON com os dados
      String json = "{";
      json += "\"temperatura\": " + String(dados.temperature, 1) + ",";
      json += "\"umidadeAr\": " + String(dados.humidity, 1) + ",";
      json += "\"luminosidade\": " + String(porcentLDR) + ",";
      json += "\"umidadeSolo\": " + String(porcentSolo);
      json += "}";

      // Envia resposta JSON
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: application/json");
      client.println("Access-Control-Allow-Origin: *");
      client.println("Connection: close");
      client.println();
      client.println(json);
      client.stop();
    } else {
      // Página HTML principal
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/html");
      client.println("Connection: close\n");

      client.println(R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<title>ESP32 Sensor Dashboard</title>
<script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
<style>
  body { font-family: Arial, sans-serif; background:#f4f4f4; padding:20px; }
  h1 { color:#333; }
  .card { background: white; padding: 10px; margin: 10px 0; border-radius: 8px; box-shadow: 0 0 10px rgba(0,0,0,0.1); }
  .icon { margin-right: 8px; color: #2c3e50; }
  #indicador { font-size: 16px; margin: 10px 0; }
  #led { color: green; transition: 0.3s; }
  #chart { background: white; border-radius: 8px; padding: 10px; box-shadow: 0 0 10px rgba(0,0,0,0.1); }
</style>
</head>
<body>
<h1>ESP32 Sensor Dashboard</h1>
<div id="indicador">🔴 <b>Dados sendo atualizados...</b> <span id="ip"></span></div>
<div class="card">☀️ <b>Luminosidade:</b> <span id="luminosidade">--</span> %</div>
<div class="card">🌱 <b>Umidade do solo:</b> <span id="solo">--</span> %</div>
<div class="card">🌡️ <b>Temperatura:</b> <span id="temperatura">--</span> °C</div>
<div class="card">💧 <b>Umidade do ar:</b> <span id="umidade">--</span> %</div>

<canvas id="chart" height="200"></canvas>

<script>
  const ctx = document.getElementById('chart').getContext('2d');
  let dataPoints = [];

  const chart = new Chart(ctx, {
    type: 'line',
    data: {
      labels: [],
      datasets: [
        { label: 'Temperatura (°C)', borderColor: 'red', data: [], fill: false, tension: 0.3 },
        { label: 'Umidade do ar (%)', borderColor: 'blue', data: [], fill: false, tension: 0.3 },
        { label: 'Luminosidade (%)', borderColor: 'orange', data: [], fill: false, tension: 0.3 },
        { label: 'Umidade do solo (%)', borderColor: 'green', data: [], fill: false, tension: 0.3 }
      ]
    },
    options: {
      responsive: true,
      plugins: { legend: { position: 'bottom' } },
      scales: { y: { beginAtZero: true, max: 100 } }
    }
  });

  // Indicador piscante
  let led = document.getElementById('indicador');
  setInterval(() => {
    if (led.style.visibility === 'hidden') {
      led.style.visibility = 'visible';
    } else {
      led.style.visibility = 'hidden';
    }
  }, 1000);

  // Exibir IP do ESP32
  document.getElementById('ip').textContent = "IP: " + window.location.hostname;

  function atualizarDados() {
    fetch('/data')
      .then(response => response.json())
      .then(json => {
        document.getElementById('temperatura').textContent = json.temperatura.toFixed(1);
        document.getElementById('umidade').textContent = json.umidadeAr.toFixed(1);
        document.getElementById('luminosidade').textContent = json.luminosidade;
        document.getElementById('solo').textContent = json.umidadeSolo;

        // Atualizar gráfico
        if (dataPoints.length >= 50) dataPoints.shift();
        dataPoints.push(json);

        chart.data.labels = dataPoints.map((_, i) => i + 1);
        chart.data.datasets[0].data = dataPoints.map(d => d.temperatura);
        chart.data.datasets[1].data = dataPoints.map(d => d.umidadeAr);
        chart.data.datasets[2].data = dataPoints.map(d => d.luminosidade);
        chart.data.datasets[3].data = dataPoints.map(d => d.umidadeSolo);
        chart.update();
      })
      .catch(err => console.error('Erro ao obter dados:', err));
  }

  atualizarDados();
  setInterval(atualizarDados, 3000);
</script>
</body>
</html>
      )rawliteral");

      client.stop();
    }
  }
}