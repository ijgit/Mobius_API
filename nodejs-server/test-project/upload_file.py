import requests

url = "http://localhost:3000/users"

payload={'cnt': 'cnt name', 'ae': 'ae name'}

files = {'file': open('C:/Users/jeong/Downloads/frozen_graph.pb','rb')}

headers = {}

response = requests.post(url, headers=headers, data=payload, files=files)

print(response.text)
