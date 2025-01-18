import os
import psutil  # For getting process information
import pandas as pd
from sklearn.ensemble import IsolationForest
from sklearn.preprocessing import MinMaxScaler
import time

# Feature extraction
def get_process_metrics(pid):
    try:
        process = psutil.Process(pid)
        cpu_usage = process.cpu_percent(interval=1)  # CPU usage in percentage
        memory_usage = process.memory_info().rss / 1024  # Memory usage in KB
        num_threads = process.num_threads()
        io_counters = process.io_counters()
        return [cpu_usage, memory_usage, num_threads, io_counters.read_bytes, io_counters.write_bytes]
    except (psutil.NoSuchProcess, psutil.AccessDenied):
        return None

# Collect data for training
def collect_data():
    data = []
    for pid in psutil.pids():
        metrics = get_process_metrics(pid)
        if metrics:
            data.append(metrics)
    return pd.DataFrame(data, columns=['CPU', 'Memory', 'Threads', 'ReadBytes', 'WriteBytes'])

# Scaling the data
def scale_data(data):
    scaler = MinMaxScaler()
    return scaler.fit_transform(data)

# Training an Anomaly Detection Model
def train_anomaly_model(data):
    model = IsolationForest(contamination=0.1)  # 10% anomalies expected
    model.fit(data)
    return model

# Anomaly Detection
def detect_anomalies(model, data):
    predictions = model.predict(data)
    return predictions

# Main Function
if __name__ == '__main__':
    # Collect system data
    print("Collecting process data...")
    data = collect_data()

    # Preprocess the data (scaling)
    scaled_data = scale_data(data)

    # Train the anomaly detection model
    print("Training anomaly detection model...")
    model = train_anomaly_model(scaled_data)

    while True:
        # Collect new data
        print("Collecting real-time process data...")
        new_data = collect_data()
        scaled_new_data = scale_data(new_data)

        # Detect anomalies
        print("Detecting anomalies...")
        anomalies = detect_anomalies(model, scaled_new_data)

        # Print the results
        for pid, anomaly in zip(psutil.pids(), anomalies):
            if anomaly == -1:
                print(f"Anomaly detected in process {pid}")

        time.sleep(5)  # Wait before checking again
