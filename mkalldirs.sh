#!/bin/bash
PID_FILE_DIR="/etc/meteo-station"
CONFIG_FILES_DIR="/etc/meteo-station/config"
DATA_DIR="/var/meteo-data"


if [ -n "$PID_FILE_DIR" ]; then
mkdir -v "$PID_FILE_DIR"
fi

if [ -n "$CONFIG_FILES_DIR" ]; then
mkdir -v "$CONFIG_FILES_DIR"
fi

if [ -n "$DATA_DIR" ]; then
mkdir -v "$DATA_DIR"
fi
