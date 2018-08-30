from __future__ import print_function
from apiclient.discovery import build
from httplib2 import Http
from oauth2client import file, client, tools

import time 

import serial
ser = serial.Serial("/dev/ttyUSB0",115200)

SCOPES = 'https://www.googleapis.com/auth/spreadsheets'
store = file.Storage('credentials.json')
creds = store.get()
if not creds or creds.invalid:
    flow = client.flow_from_clientsecrets('client_secret.json', SCOPES)
    creds = tools.run_flow(flow, store)
service = build('sheets', 'v4', http=creds.authorize(Http()))

# Call the Sheets API
spreadsheet_id = '1RaGPlEJKQeg_xnUGi1mlUt95-Gc6n-XF_czwudIP5Qk'
token_range = 'LED!A1:A'
data_range = 'LED!B1:B'
value_input_option = 'USER_ENTERED'
insert_data_option = 'INSERT_ROWS'

def get_state():
	result = service.spreadsheets().values().get(spreadsheetId=spreadsheet_id,range=token_range).execute()
	values = result.get('values', [])
	if not values:
		print('No data found.')
	else:
		for row in values:
			state = int(row[0])
		# Print columns A and E, which correspond to indices 0 and 4
		return state

signal = 0
while(signal >= 0):
	state = get_state()
	print("state : "+str(state))
	heart_rate = int(ser.readline())
	print("heart_rate : "+str(heart_rate))
	values = [
    [
       heart_rate
        # Cell values ...
    ],

	]
	body = {
    'values': values
}
	result = service.spreadsheets().values().update(
	    spreadsheetId=spreadsheet_id, range=data_range,
	    valueInputOption=value_input_option, body=body).execute()
	print('{0} cells updated.'.format(result.get('updatedCells')));
	time.sleep(2)