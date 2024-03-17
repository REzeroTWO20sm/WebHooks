import asyncio
import websockets
import json

from flask import Flask, request, jsonify
from openpyxl import Workbook,load_workbook

app = Flask(__name__)

@app.route('/', methods=['POST', 'GET'])
def index():
    data = request.get_json()
    if data:
        wb = load_workbook('data.xlsx')
        sheet = wb.active
        sheet.append(list(data.keys()))  # Записываем ключи как заголовки
        sheet.append(list(data.values()))  # Записываем значения
        wb.save('data.xlsx')
        return jsonify({'message': 'Данные успешно сохранены в файле data.xlsx'}), 200
    else:
        return jsonify({'error': 'Ошибка в данных'}), 400

if __name__ == '__main__':
    app.run(debug=True)