#!/bin/bash
PID_FILE_DIR="/etc/meteo-station"
CONFIG_FILES_DIR="/etc/meteo-station/config"
DATA_DIR="/var/meteo-data"
YAMAL_LIB_DIR=""
if [ -n "$PID_FILE_DIR" ]; then
mkdir "$PID_FILE_DIR"
echo " Został utworzony katalog $PID_FILE_DIR"
fi