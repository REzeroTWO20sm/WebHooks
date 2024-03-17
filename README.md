# <div style="background-color: black;"> <strong style="color: white">Лаборатрная</strong>  <strong style="color: #FF0400">W</strong><strong style="color: #FF9D00">e</strong><strong style="color: #FFFF00">b</strong> <strong style="color: #22FF00">h</strong><strong style="color: #00FF8C">o</strong><strong style="color: #00FFFF">o</strong><strong style="color: #0004FF">k</strong><strong style="color: #FF00F2">s</strong></div>

1. Что делает лаборатоная:
   - Запускает backend Яндекс навыка на локальный сервер (лс) 
   - Подключает лс к всемирной сети с помощю ngrok
   - Запускает клиентское приложение на python

2. Процесс работы подробнее:
   - backend навыка постоянно прослушивает сервер
   - пользователь вводит команду в навыке
   - команду видит backend, если есть ответ отправляет его на сервер
   - навык постит ответ пользователю
3. Суть main-на коротко 
   - создается сервер 
  ```httplib::Server svr;```
   - post-запросом постояно слушаем и отвечаем, если можем 
  ```svr.Post("/", [](const httplib::Request& req, httplib::Response& res) { "Здесь условия каки на что отвечаем" });```
   - создаем другой сервер на <strong style="color: green">python</strong> и 
  клиентское туда же, которое постоянно слушает этот сервер и записывает то, 
  что приходит, а в это время backend на <strong style="color: blue">C++</strong> отправляет данные на туда, 

![Govno](https://media.giphy.com/media/v1.Y2lkPTc5MGI3NjExODk0ZDZhZWVtMWczOTAzaHpqYm11ZWplcHVrdTc2Nmk0M25lOGxvZCZlcD12MV9naWZzX3NlYXJjaCZjdD1n/3o7bug2wkdhpf7kbFS/giphy.gif)# WebHooks
# WebHooks
