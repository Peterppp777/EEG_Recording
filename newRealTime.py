import serial
import struct
import pycrc.algorithms
import csv
import datetime
import time
import os
from google.cloud import storage
import datetime
from datetime import datetime

os.environ['GOOGLE_APPLICATION_CREDENTIALS'] = r'C:\Users\jiesh\Desktop\summer\EEG\Serial\plucky-alliance-401418-4132694034de.json'

storage_client = storage.Client()


# Configure the serial connection
ser = serial.Serial('COM5', 115200, timeout=0.1)  # Adjust COM port as needed

# Prepare the CRC32 function
crc32_algo = pycrc.algorithms.Crc(width=32, poly=0x04C11DB7,
                                  reflect_in=True, xor_in=0xFFFFFFFF,
                                  reflect_out=True, xor_out=0xFFFFFFFF)

# CSV file setup
output_csv = 'capture_realTime.csv'
csvfile = open(output_csv, 'w', newline='')
csvwriter = csv.writer(csvfile)
csvwriter.writerow(['Time', 'Counter', 'ADC Reading', 'Accel X', 'Accel Y', 'Accel Z', 'Gyro X', 'Gyro Y', 'Gyro Z'])

# Global counter
global_counter = 1

# Duration for recording (in seconds)
# record_duration = 20  

# Function to read a packet
def read_packet():
    buffer = b''
    while True:
        data = ser.read(ser.in_waiting or 1)
        if not data:
            continue

        buffer += data

        if (buffer.startswith(b'\xAA') or buffer.startswith(b'\xEE')) and len(buffer) > 3:
            length = struct.unpack('<H', buffer[1:3])[0]
            if len(buffer) >= length + 4:
                packet_data = buffer[:length + 4]
                buffer = buffer[length + 4:]

                if (packet_data[0] == 0xAA and packet_data[-1] == 0x55) or (packet_data[0] == 0xEE and packet_data[-1] == 0x99):
                    received_crc = packet_data[-5:-1]
                    calculated_crc = calculate_crc32(packet_data[3:-5])
                    if received_crc == calculated_crc:
                        return packet_data[0], packet_data[3:-5]  # Return packet type and data
                    else:
                        print("Error: CRC mismatch")
                else:
                    print("Error: Incorrect footer")

def calculate_crc32(data):
    crc = crc32_algo.bit_by_bit_fast(data)
    return struct.pack('<I', crc)

def process_packet(packet_type, data):
    if packet_type == 0xAA:  # EEG Packet
        sequence_number = struct.unpack('<H', data[:2])[0]
        readings = extract_readings(data[2:12])
        return sequence_number, readings, None
    elif packet_type == 0xEE:  # IMU Packet
        accel = struct.unpack('<fff', data[:12])
        gyro = struct.unpack('<fff', data[12:24])
        return None, None, (accel, gyro)

def extract_readings(data):
    readings = []
    num_readings = len(data) // 2  # Assuming each reading is 2 bytes
    for i in range(0, num_readings * 2, 2):
        combined = data[i] | (data[i + 1] << 8)
        reading = combined & 0xFFF  # Extract 12-bit reading
        readings.append(reading)
    return readings

def record_data(sequence_number, readings, imu_data, csvwriter):
    global global_counter
    current_time = datetime.now().strftime("%H:%M:%S.%f")[:-3]
    if readings:
        for reading in readings:
            csvwriter.writerow([current_time, global_counter, reading] + [''] * 6)
            global_counter += 1
    if imu_data:
        accel, gyro = imu_data
        csvwriter.writerow([current_time, 'IMU', ''] + list(accel) + list(gyro))

        
def upload_to_gcs(bucket_name, source_file_name, destination_blob_name):
    """Uploads a file to Google Cloud Storage"""
    storage_client = storage.Client()
    bucket = storage_client.bucket(bucket_name)
    blob = bucket.blob(destination_blob_name)

    blob.upload_from_filename(source_file_name)

    print(f"File {source_file_name} uploaded to {destination_blob_name}.")



def handle_packets(output_csv, record_duration, upload_start, upload_end):
    # CSV file setup
    csvfile = open(output_csv, 'w', newline='')
    csvwriter = csv.writer(csvfile)
    csvwriter.writerow(['Time', 'Counter', 'ADC Reading', 'Accel X', 'Accel Y', 'Accel Z', 'Gyro X', 'Gyro Y', 'Gyro Z'])

    start_time = time.time()
    upload_start_time = start_time + upload_start * 60  # minutes    
    upload_end_time = start_time + upload_end * 60  # minutes
    uploaded = False  # Flag to track if the file has been uploaded

    try:
        while True:
            current_time = time.time()
            if current_time - start_time > record_duration:
                print("Recording duration reached. Stopping.")
                break

            print("Waiting for packet...")
            packet_type, packet = read_packet()
            if packet:
                print("Packet received.")
                sequence_number, readings, imu_data = process_packet(packet_type, packet)
                record_data(sequence_number, readings, imu_data, csvwriter)
            else:
                print("No packet received.")

            # Check if the current time is past the upload end time and the file has not been uploaded yet
        if current_time > upload_end_time and not uploaded:
            try:
                # Upload the file
                upload_to_gcs('eeg_realtime', output_csv, output_csv)
                uploaded = True  # Set the flag to True to prevent further uploads
            except Exception as e:
                print(f"An error occurred during file upload: {e}")

    finally:
        csvfile.close()
        ser.close()
        print("Program ended.")

if __name__ == "__main__":
    # unique_output_csv_name = generate_unique_filename('capture_realTime')
    output_csv_name = 'capture_realTime.csv'  # Name of the CSV file
    recording_time = 60  # Total recording time in seconds
    upload_start = 0  # Start uploading from the beginning
    upload_end = 1  # Stop uploading after 10 minutes
    handle_packets(output_csv_name, recording_time, upload_start, upload_end)