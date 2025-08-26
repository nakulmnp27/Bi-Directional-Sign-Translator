from flask import Flask, request, jsonify
from googletrans import Translator

app = Flask(__name__)
translator = Translator()

@app.route('/translate', methods=['POST'])
def translate():
    try:
        data = request.json
        text = data.get("text", "")
        from_lang = data.get("fromLang", "en")
        to_lang = data.get("toLang", "hi")

        if not text:
            return jsonify({"error": "No text provided"}), 400

        translated_text = translator.translate(text, src=from_lang, dest=to_lang).text
        return jsonify({"translatedText": translated_text})
    
    except Exception as e:
        return jsonify({"error": str(e)}), 500

if __name__ == '__main__':
    app.run(debug=True)
import requests
from flask import Flask, jsonify, request

app = Flask(__name__)

@app.route('/translate', methods=['GET'])
def translate_text():
    text = request.args.get('text')
    from_lang = request.args.get('from_lang')
    to_lang = request.args.get('to_lang')

    api_url = f"https://api.mymemory.translated.net/get?q={text}&langpair={from_lang}|{to_lang}"

    try:
        response = requests.get(api_url)
        data = response.json()
        return jsonify(data)
    except Exception as e:
        return jsonify({"error": str(e)})

if __name__ == '__main__':
    app.run(debug=True)
