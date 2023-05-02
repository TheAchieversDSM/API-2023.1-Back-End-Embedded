from firebase import firebase
from flask import Flask, request

firebase = firebase.FirebaseApplication('https://tecsus-59210-default-rtdb.firebaseio.com', None)

app = Flask(__name__)

@app.route('/submitToFirebase', methods=['GET', 'POST'])
def submit():
	content_type = request.headers.get('Content-type')

	if(content_type == "application/json"):
		try:
			json = dict(request.json)
			new_route = "/esp32TEMP/" + json["_uid"]
			new_data = {
				"_unixtime": json["_unixtime"],
				"_medida" : json["_medida"],
				"_nomeParametro": json["_nomeParametro"]
			}
			firebase.post(new_route, new_data)
			return "Envio com sucesso"
		except Exception as e: 
			return e

if __name__ == '__main__':
    # run app in debug mode on port 5000
    app.run(debug=True, port=5000)