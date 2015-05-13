# 2015-ESE350-BedMat

## Usage

The mat can operate in two different modes: realtime and long-term collection.

### Long-term

`index.html` connects to a server running `server.py`.

`upload.cpp` runs on an mbed and pushes data from the mat to a server running `server.py`

### Realtime

`serial.cpp` runs on an mbed and pushes data over serial (via USB) to a computer running `serial.py` and `serial.html`