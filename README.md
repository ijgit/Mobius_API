# Mobius_API

[Mobius](https://github.com/IoTKETI/Mobius) 활용 예제

- arduino_m0_wifi_upload: [Adafruit의 feather m0 WiFi](https://www.adafruit.com/product/3010) 보드를 사용하여 센서 값을 Mobius 에 업로드
- python: 
  - `graph.py`는 Mobius 에서 가져온 데이터를 시각화, Pyqt 사용 (pyqt: python GUI 라이브러리)
  - `LoRa_data.py`는 Mobius 에서 가져온 데이터를 파싱, 결측값 처리, 정렬하여 파일로 저장
- web_API: 각각 GET, POST 에 대한 API 예제
