# 安装Flask 和 Flask-CORS
# pip install flask
# pip install flask-cors

from flask import Flask, jsonify
from flask_cors import CORS

app = Flask(__name__)
CORS(app) # 启用CORS

@app.route('/weather', methods=['GET'])
def get_weather():
    weather_info = {
        'city': '北京',
        'temperature': '15°C',
        'condition': '晴朗'
    }
    return jsonify(weather_info)

if __name__ == '__main__':
    app.run(debug=True)
