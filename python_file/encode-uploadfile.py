import os
import json
import requests
import base64


ip = '172.28.160.1' #'172.17.237.189' #"192.168.50.157"
port = '7579'
cse = 'Mobius'
ae = 'testAe'
cnt = 'testCnt'
url = f'http://{ip}:{port}/{cse}/{ae}/{cnt}'
headers = {
  'Accept': 'application/json',
  'X-M2M-RI': '12345',
  'X-M2M-Origin': 'SOrigin',
  'Content-Type': 'application/vnd.onem2m-res+json; ty=4'
}

input_file_name = 'saved_model.pb'
output_file_name = 're_saved_model.pb'



def encodeFile(inputFile):
    print(f'file size: {os.path.getsize(inputFile)} bytes')

    file_read = open(inputFile, 'rb').read()
    file_64_encode = base64.encodebytes(file_read) 

    # change binary string to string
    file_64_encode = file_64_encode.decode('ascii')
    print(file_64_encode[:10])

    return file_64_encode


def decodeFile(inputString):
    file_64_encode = inputString
    
    #change string to binary string
    file_64_encode = file_64_encode.encode('ascii')
    file_64_decode = base64.decodebytes(file_64_encode)

    # write output file
    file_result = open(output_file_name, 'wb')
    file_result.write(file_64_decode)



def cinCreateRequest(inputData):
    data = {
        "m2m:cin":{
            "con": inputData
        }
    }
    payload = json.dumps(data)      # json to string
    response = requests.request("POST", url, headers=headers, data=payload)
    
    return response.text



# change model file to string
fileToString = encodeFile(input_file_name)
result = cinCreateRequest(fileToString)