#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

QueueHandle_t xLuxQueue;
LiquidCrystal_I2C lcd(0x27, 16, 2); // адрес 0x27, 16x2

// -----------------------
// Прототипы задач
// -----------------------
void TaskDisplay(void *pvParameters);
void TaskLDR(void *pvParameters);
void TaskSerialTime(void *pvParameters);
float calculateLuxFromADC(int D);

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();

  xLuxQueue = xQueueCreate(5, sizeof(float));
  if (xLuxQueue == NULL) {
    Serial.println("Queue cannot be created!");
  }

  // Создание задач
  xTaskCreate(TaskDisplay, "Display_task", 128, NULL, 1, NULL);
  xTaskCreate(TaskLDR, "LDR_task", 128, NULL, 1, NULL);
  xTaskCreate(TaskSerialTime, "Serial_task", 128, NULL, 1, NULL);

  // Запуск планировщика
  vTaskStartScheduler();
}

void loop() {
  // Не нужен, FreeRTOS управляет задачами
}

// -----------------------
// Задача измерения
// -----------------------
void TaskLDR(void *pvParameters) {
  float lux;
  int adcValue;

  for (;;) {
    adcValue = analogRead(A0);
    lux = calculateLuxFromADC(adcValue);

    xQueueSend(xLuxQueue, &lux, portMAX_DELAY);
    vTaskDelay(pdMS_TO_TICKS(500)); // обновление каждые 500 мс
  }
}

// -----------------------
// Задача дисплея
// -----------------------
void TaskDisplay(void *pvParameters) {
  float receivedLux;

  for (;;) {
    if (xQueueReceive(xLuxQueue, &receivedLux, portMAX_DELAY) == pdPASS) {
      lcd.setCursor(0, 0);
      lcd.print("Lux: ");
      lcd.print(receivedLux, 1);
      lcd.print("     "); // очищаем хвост старого числа
    }
  }
}

// -----------------------
// Задача вывода времени
// -----------------------
void TaskSerialTime(void *pvParameters) {
  (void)pvParameters;
  unsigned long seconds = 0;

  for (;;) {
    Serial.print("Time: ");
    Serial.print(seconds);
    Serial.println(" s");
    seconds++;

    vTaskDelay(pdMS_TO_TICKS(1000)); // каждую секунду
  }
}

// -----------------------
// Функция расчёта Lux
// -----------------------
float calculateLuxFromADC(int D) {
  return 1096.7f * expf(-0.007f * (float)D);
}