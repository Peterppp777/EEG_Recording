# EEG_Recording
 
A brief introduction to your project. Describe what your project does, its features, and its purpose.

## Table of Contents

- [Installation](#installation)
  - [Arduino Libraries](#arduino-libraries)
  - [Python Modules](#python-modules)
- [Setting Up Google Cloud SDK and Authentication](#setting-up-google-cloud-sdk-and-authentication)
- [Usage](#usage)
- [Contributing](#contributing)
- [License](#license)

## Installation

### Arduino Libraries

This project requires the following Arduino libraries:

#### LSM6DS3 Library

- **Description**: The LSM6DS3 is a 3D accelerometer and 3D gyroscope sensor.
- **Installation**: Available through the Arduino Library Manager or directly from the [manufacturer's or a third-party GitHub repository](https://github.com/Seeed-Studio/Seeed_Arduino_LSM6DS3).

#### CircularBuffer Library

- **Description**: A library for creating circular buffer functionality.
- **Installation**: Find it in the Arduino Library Manager or [CircularBuffer](https://www.arduino.cc/reference/en/libraries/circularbuffer/).

#### NRF52_MBED_TimerInterrupt Library

- **Description**: Used for handling timer interrupts on NRF52 devices.
- **Installation**: Search for it in the Arduino Library Manager or on [GitHub](https://github.com/khoih-prog/NRF52_MBED_TimerInterrupt).

#### Arduino_CRC32 Library

- **Description**: For calculating CRC32 checksums.
- **Installation**: Look for it in the Arduino Library Manager or on [GitHub](https://github.com/arduino-libraries/Arduino_CRC32).

### Python Modules

#### pycrc

- **Description**: Not a standard Python module and may need to be installed from a script or module for calculating CRCs.
- **Installation**: `pip install pycrc`. More details at [pycrc on PyPI](https://pypi.org/project/pycrc/).

#### google-cloud-storage

- **Description**: Used to interact with Google Cloud Storage.
- **Installation**: `pip install google-cloud-storage`. Documentation available at [Google Cloud Storage Python Client](https://cloud.google.com/python/docs/reference/storage/latest).

## Setting Up Google Cloud SDK and Authentication

### Google Cloud SDK

- **Installation**: Optional but recommended for managing credentials and interacting with Google Cloud services from the command line. [Google Cloud SDK Documentation](https://cloud.google.com/sdk/docs/install).

### Authentication

1. **Initialize the SDK**: Run `gcloud init` in your terminal and follow the instructions.
2. **Service Account and Key File**:
   - Navigate to IAM & Admin > Service Accounts in the Google Cloud Console.
   - Create a new service account, assign it the necessary roles (e.g., Storage Object Admin), and download a JSON key file.
3. **Set Environment Variable**:
   - **Linux/macOS**: `export GOOGLE_APPLICATION_CREDENTIALS="/path/to/your/keyfile.json"`
   - **Windows Command Prompt**: `set GOOGLE_APPLICATION_CREDENTIALS=C:\path\to\your\keyfile.json`
   - **Windows PowerShell**: `$env:GOOGLE_APPLICATION_CREDENTIALS="C:\path\to\your\keyfile.json"`

## Usage

Provide instructions on how to use your project, including setup steps and examples.

## Contributing

Contributions are welcome! For major changes, please open an issue first to discuss what you would like to change.

## License

[MIT](https://choosealicense.com/licenses/mit/)


