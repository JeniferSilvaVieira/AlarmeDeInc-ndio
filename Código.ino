#include <WiFi.h> 

const char *ssid = "SATC IOT"; 
const char *password = "IOT2024@#";

WiFiServer server(80); 

const int pino_chamas = 27; 
const int led_chamas = 25;    
const int buzzer = 26;     

int chama_detectada = LOW; 

void setup() {
  Serial.begin(115200); 

  pinMode(pino_chamas, INPUT); 
  pinMode(led_chamas, OUTPUT);  
  pinMode(buzzer, OUTPUT);    

  Serial.println();
  Serial.println("Conectando ao WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado.");
  Serial.println("Endereço IP: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop() {
  
  chama_detectada = digitalRead(pino_chamas);
  
  if (chama_detectada == HIGH) {
    digitalWrite(led_chamas, HIGH);
    digitalWrite(buzzer, HIGH);    

  } else {
    digitalWrite(led_chamas, LOW);
    digitalWrite(buzzer, LOW);    
  }

  WiFiClient client = server.available();

  if (client) { 
    Serial.println("Novo Cliente.");
    String currentLine = ""; 

    while (client.connected()) { 
      if (client.available()) {  
        char c = client.read();  
        Serial.write(c);        

        if (c == '\n') { 
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            client.println("<!DOCTYPE html>");
            client.println("<html>");
            client.println("<head>");
            client.println("<script>");
            client.println("function updateStatus() {");
            client.println("  fetch('/status').then(response => response.text()).then(data => {");
            client.println("    document.getElementById('status').innerHTML = data;");
            client.println("  });");
            client.println("}");
            client.println("setInterval(updateStatus, 500);"); 
            client.println("</script>");
            client.println("</head>");
            client.println("<body>");
            client.println("<h1>Monitoramento de Chamas e Gás</h1>");
            client.println("<p id='status'>Aguardando status...</p>");
            client.println("</body>");
            client.println("</html>");

            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }

        if (currentLine.endsWith("GET /status")) {
          String status = "";
          if (chama_detectada == HIGH) {
            status += "Fogo detectado! LED de fogo ligado.<br>";
          } else {
            status += "Sem fogo. LED de fogo desligado.<br>";
          }

          client.println(status);
        }
      }
    }
    client.stop();
    Serial.println("Cliente desconectado.");
  }
}
