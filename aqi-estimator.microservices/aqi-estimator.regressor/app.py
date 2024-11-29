import pandas as pd
import pickle as pkl
from flask import Flask, request, jsonify

app = Flask(__name__)


# Load the saved model and scaler
with open('prediction_model_xgb_regressor.pkl', 'rb') as file:
    saved_data = pkl.load(file)

model = saved_data['model']
scaler = saved_data['scaler']

# Desired feature names
FEATURE_NAMES = [
    "fogImpactIndex", "airLightRed", "airLightGreen", "airLightBlue",
    "originalRedMean", "originalRedVariance", "originalRedSkewness",
    "originalGreenMean", "originalGreenVariance", "originalGreenSkewness",
    "originalBlueMean", "originalBlueVariance", "originalBlueSkewness",
    "originalRedToGreen", "originalRedToBlue", "originalGreenToBlue",
    "deHazedRedMean", "deHazedRedVariance", "deHazedRedSkewness",
    "deHazedGreenMean", "deHazedGreenVariance", "deHazedGreenSkewness",
    "deHazedBlueMean", "deHazedBlueVariance", "deHazedBlueSkewness"
]

@app.route('/predict', methods=['POST'])
def predict():
    # Parse the input JSON
    data = request.get_json()
    print(data)

    if not isinstance(data, list) or len(data) != len(FEATURE_NAMES):
        return jsonify({'error': 'Invalid input format. Expecting a list of values.'}), 400

    # Map the input data to the feature names
    input_data = pd.DataFrame([data], columns=FEATURE_NAMES)

    # Scale the input data
    input_data_scaled = scaler.transform(input_data)

    # Predict using the model
    prediction = model.predict(input_data_scaled)[0]  # Single prediction

    # Return the result
    return jsonify({'prediction': float(prediction)})


if __name__ == '__main__':
    app.run()
