# Mobius_API

[mobius](https://github.com/IoTKETI/Mobius) api for project

- arduino_m0_wifi_upload는 [Adafruit의 feather m0 WiFi](https://www.adafruit.com/product/3010) 보드를 사용하여 센서 값을 Mobius 에 업로드 합니다.
- python: 
  - `graph.py`는 Mobius 에서 가져온 데이터를 시각화하여 Pyqt로 보여줍니다. pyqt는 python GUI 라이브러리입니다.
  - `LoRa_data.py`는 Mobius 에서 가져온 데이터를 파싱, 결측값 처리, 정렬하여 파일로 저장합니다.
- web_API: 각각 GET, POST 에 대한 API 예제입니다.
