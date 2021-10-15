import os
import json
import requests
import base64

ip = '172.28.160.1' #'172.17.237.189' #"192.168.50.157"
port = '7579'
cse = 'Mobius'
ae = 'testAe'
cnt = 'testCnt'
url = f'http://{ip}:{port}/{cse}/{ae}/{cnt}/la'
payload={}
headers = {
  'Accept': 'application/json',
  'X-M2M-RI': '12345',
  'X-M2M-Origin': 'SOrigin'
}

def decodeFile(inputString):
    file_64_encode = inputString
    
    #change string to binary string
    file_64_encode = file_64_encode.encode('ascii')
    file_64_decode = base64.decodebytes(file_64_encode)

    # write output file
    file_result = open('re_saved_model.pb', 'wb')
    file_result.write(file_64_decode)


def getCinRequest():
    payload = {}
    response = requests.get(url, headers=headers, data=payload)
    return response

container = getCinRequest()
container = container.json()['m2m:cin']["con"]

# container value contain only file encoded string
decodeFile(container)