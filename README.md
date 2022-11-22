# esp-pathfinder

## Get repository
```
git clone git@github.com:KaryFeever/esp-pathfinder.git
cd esp-pathfinder
git submodule update --recursive --init
```

## Usage
Set `WIFI_CONFIG_SSID` and `WIFI_CONFIG_PASSWORD` in `src/main/communication.c` 
```
idf.py set-target esp32s3
idf.py build
idf.py flash monitor
```

Get the ip of your board from output
```
I (4050) esp_netif_handlers: sta ip: 192.168.1.12, mask: 255.255.255.0, gw: 192.168.1.1
I (4050) TEST: SYSTEM_EVENT_STA_GOT_IP
I (4050) TEST: Got IP: '192.168.1.12'
I (4050) TEST: Starting server on port: '80'
I (4060) TEST: Registering URI handlers
```

Open your browser and go to:
- `board-ip/stream ` to get a video stream
- `board-ip/capture ` to get a photo
