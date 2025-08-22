#!/bin/bash
set -e

SERVICE=lostmud
INSTALL_DIR=/opt/lostmud
BINARY=rot

echo "[*] Building MUD..."
make clean
make

echo "[*] Stopping service..."
sudo systemctl stop $SERVICE || true

echo "[*] Installing new binary..."
sudo cp -f $BINARY $INSTALL_DIR/
sudo chown mud:mud $INSTALL_DIR/$BINARY

echo "[*] Starting service..."
sudo systemctl start $SERVICE

echo "[*] Checking status..."
sudo systemctl --no-pager --full status $SERVICE
