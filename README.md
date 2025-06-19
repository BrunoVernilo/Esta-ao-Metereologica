# 🌡️ ESP32 Sensor Dashboard

Projeto de IoT utilizando um *ESP32, sensores físicos e um **Dashboard web* para visualização em tempo real dos dados de:

* 🌞 *Luminosidade (LDR)*
* 🌱 *Umidade do Solo*
* 🌡️ *Temperatura (DHT11)*
* 💧 *Umidade do Ar (DHT11)*

---

## 📡 Funcionalidades

✅ Conexão via Wi-Fi
✅ API REST (/data) com resposta JSON dos sensores
✅ Página Web com *Chart.js* para gráficos em tempo real
✅ Indicador de status piscante
✅ Leitura de sensores via *GPIO do ESP32*

---

## 🛠️ Tecnologias utilizadas

* ESP32 (Arduino Framework)
* DHT11 (Temperatura e Umidade)
* LDR (Luminosidade)
* Sensor de Umidade de Solo
* Chart.js (Frontend)
* HTML + CSS + JS (Dashboard)
* WiFiServer (Servidor Web no ESP32)
* LCD I2C para exibir o IP local

---

## 🚀 Como usar

1. Suba o código no seu *ESP32*
2. Conecte os sensores nos pinos:

| Sensor             | Pino ESP32                  |
| ------------------ | --------------------------- |
| DHT11              | GPIO 25                     |
| Umidade do solo    | GPIO 13                     |
| LDR (Luminosidade) | GPIO 34                     |
| LCD I2C            | SDA: GPIO 26 / SCL: GPIO 27 |

3. Conecte o ESP32 à sua rede Wi-Fi (configure SSID e senha no código)

4. No navegador, acesse o IP mostrado no LCD ou no Serial Monitor (exemplo: http://192.168.x.x)

---

## 📷 Imagem de exemplo (opcional)

> (Se quiser, depois pode adicionar uma foto do seu circuito ou do dashboard rodando)

---

## 📋 Endpoints disponíveis

| Endpoint | Função                      |
| -------- | --------------------------- |
| /      | Página HTML com o dashboard |
| /data  | Retorna os dados em JSON    |

---

## ✨ Exemplo de JSON retornado

json
{
  "temperatura": 24.5,
  "umidadeAr": 60.2,
  "luminosidade": 75,
  "umidadeSolo": 40
}
