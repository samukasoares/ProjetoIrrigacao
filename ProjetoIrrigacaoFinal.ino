#include <RTClib.h>
#include <LiquidCrystal.h>

#define area1 8
#define area2 9
#define area3 10
#define area4 11
#define area5 12
#define bomba 13
#define pinoSensor A0

RTC_DS1307 rtc; // criando a instância RTC
const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//variaveis
const int limiarSeco = 60;
const int tempSeg = 5000;       //5 segundos de segurança
const int tempoIrrigacao = 12;   // Tempo irrigação em minutos
int umidadeSolo = 0;
const int horaIrrigar = 17;     //Horario para irrigar
String hora;

void setup() {
  
  //Define os pinos
  pinMode(area1, OUTPUT);
  pinMode(area2, OUTPUT);
  pinMode(area3, OUTPUT);
  pinMode(area4, OUTPUT);
  pinMode(area5, OUTPUT);
  pinMode(bomba, OUTPUT);
  
  // Desliga as válvulas
  digitalWrite(bomba, HIGH);
  digitalWrite(area1, HIGH);
  digitalWrite(area2, HIGH);
  digitalWrite(area3, HIGH);
  digitalWrite(area4, HIGH);
  digitalWrite(area5, HIGH);
  
  lcd.begin(16, 2);
  Serial.begin(9600);

  //Configuração Módulo RTC
  if (! rtc.begin()) // se o RTC não foi inicializado
  {
  Serial.println("RTC nao pode ser encontrado!"); 
  while (1);
  }
  if (! rtc.isrunning()) // se o RTC não estiver rodando
  {
  Serial.println("RTC nao esta funcionando!");
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // ajusta relógio com o relógio do seu PC
  }
}

void loop() {
  //Mede a umidade e atualiza o display a cada minuto.
  for (int i = 0; i<1440; i++){                     
    DateTime now = rtc.now();
    imprimirHorario();
    calcularUmidade();
    
    //Se o solo estiver seco e for 17 horas
    if (now.hour() == horaIrrigar && umidadeSolo<limiarSeco){
      irrigar();
    }else{
      lcd.setCursor(6,1);
      lcd.print("Regando: N");
      delay(60000);                 //Delay de 1 minuto
    }
  }
}

//Metodo para irrigar
void irrigar(){
  int i;
  for (i = area1; i<area5+1; i++){
    digitalWrite(i,LOW);
    if (i == area1){
      delay(tempSeg);
      digitalWrite(bomba,LOW);
    }
    
    for(int j = 0; j<tempoIrrigacao; j++){
      calcularUmidade();
      imprimirHorario();
      lcd.setCursor(6,1);
      lcd.print("Regando:A");
      lcd.setCursor(15,1);
      lcd.print(i-7);
      delay(60000);                            //Delay de 1 minuto
    }

    if(i == area5){
      digitalWrite(bomba,HIGH);
      delay(15000);                            //Tempo de segurança para bomba desligaar
      digitalWrite(i, HIGH);
      break;
    }

    digitalWrite(i+1, LOW);
    delay(tempSeg);
    digitalWrite(i, HIGH);
  }
}

void imprimirHorario (){
  DateTime now = rtc.now();
  lcd.setCursor(0,1);
  hora = hora + now.hour() + ":" + now.minute() + " ";
  lcd.print(hora);
  hora=("");
}

void calcularUmidade(){
    lcd.setCursor(0,0);
    lcd.print("Umidade: ");
    umidadeSolo = analogRead(pinoSensor);
    umidadeSolo = map(umidadeSolo, 1023, 0, 0, 100);
    lcd.print(umidadeSolo);
    lcd.print("% ");
}
