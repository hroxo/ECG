// Definições dos pinos do sensor AD8232
const int ECG_PIN = A0;         // Pino analógico onde o AD8232 está conectado
const int ECG_LO_MINUS = 7;    // Pino digital para o LO-
const int ECG_LO_PLUS = 8;     // Pino digital para o LO+
const int GREEN_LED = 3;        // Pino para o LED verde (Z1)
const int YELLOW_LED = 4;       // Pino para o LED amarelo (Z2)
const int RED_LED = 5;          // Pino para o LED vermelho (Z3)
const int BLUE_LED = 6;         // Pino para o LED azul (Z4)


const int THRESHOLD = 600;      // Limiar para detecção de picos (ajuste conforme necessário)
const int PEAK_GAP = 50;       // Intervalo mínimo em ms entre picos
const int ECG_INTERVAL = 50;    // Intervalo de atualização para o Serial Plotter (em ms)

unsigned long lastPeakTime = 0; // Armazena o tempo do último pico detectado
unsigned long lastECGTime = 0;  // Armazena o tempo da última atualização do Serial Plotter
unsigned long lastCalcTime = 0; // Armazena o último momento de cálculo de média de BPM

float bpm = 0;                  // Variável para guardar o BPM calculado
int bpmSum = 0;                 // Soma dos BPMs para o cálculo da média
int bpmCount = 0;               // Contador de leituras de BPM para o cálculo da média

void setup() {
  Serial.begin(9600);
  pinMode(ECG_PIN, INPUT);

  // Configuração dos LEDs e pinos do AD8232 como saídas
  pinMode(GREEN_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
  pinMode(ECG_LO_MINUS, INPUT);
  pinMode(ECG_LO_PLUS, INPUT);
}

void loop() {
  int ecgSignal = analogRead(ECG_PIN); // Lê o valor do sinal do ECG

  

  // Envia o sinal ECG para o Serial Plotter a cada ECG_INTERVAL
  if (millis() - lastECGTime >= ECG_INTERVAL) {
    Serial.println(ecgSignal);
    lastECGTime = millis();
  }

  // Verifica se o sinal do ECG ultrapassa o limiar definido para detecção de pico
  if (ecgSignal > THRESHOLD) {
    unsigned long currentPeakTime = millis();

    // Calcula o intervalo RR apenas se o tempo desde o último pico for maior que o intervalo mínimo
    if (currentPeakTime - lastPeakTime > PEAK_GAP) {
      float rrInterval = (currentPeakTime - lastPeakTime) / 1000.0;
      bpm = 60.0 / rrInterval;

      // Adiciona o BPM à soma e incrementa o contador
      bpmSum += bpm;
      bpmCount++;

      lastPeakTime = currentPeakTime;

      // Controla os LEDs com base no BPM
      controlLEDs(bpm);
    }
  }

  // Calcula a média de BPM a cada segundo
  if (millis() - lastCalcTime >= 1000) {
    if (bpmCount > 0) {
      float avgBpm = bpmSum / bpmCount;
      Serial.print("Média de BPM: ");
      Serial.println(avgBpm);

      bpmSum = 0;
      bpmCount = 0;
    }
    lastCalcTime = millis();
  }

  delay(10);  // Pequeno delay para estabilidade da leitura
}

// Função para controlar os LEDs conforme a faixa de BPM
void controlLEDs(float bpm) {
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(RED_LED, LOW);
  digitalWrite(BLUE_LED, LOW);

  if (bpm < 115) {
    digitalWrite(GREEN_LED, HIGH);
  } else if (bpm >= 115 && bpm <= 133) {
    digitalWrite(YELLOW_LED, HIGH);
  } else if (bpm >= 134 && bpm <= 152) {
    digitalWrite(RED_LED, HIGH);
  } else if (bpm > 153) {
    digitalWrite(BLUE_LED, HIGH);
  }
}
