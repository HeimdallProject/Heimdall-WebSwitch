#!/usr/bin/env bash

echo "------------------------------------------------------------"
echo "            HEIMDALL PROJECT  -  install wizard   "
echo "               University of Rome Tor Vergata     "
echo "                                       "
echo "               Authors: - Alessio Moretti            "
echo "                        - Andrea Cerra               "
echo "                        - Claudio Pastorini          "
echo "                                       "
echo "               last build : February 2016            "
echo "               https://github.com/HeimdallProject    "
echo "------------------------------------------------------------"
echo ""
mkdir build
echo "[OK] creating build folders... "

download_path=$(pwd)
socket_path=$download_path"sockets/"
log_path=$download_path"log/"

cd ${download_path}
echo "[OK] creating working directory"

mkdir ${socket_path};
echo "[OK] creating socket folder... "
mkdir ${log_path};
echo "[OK] creating log folder..."

echo "# Unix sockets path" >> ./code/config/heimdall_config.conf
echo "sockets_path "$socket_path >> ./code/config/heimdall_config.conf
echo "[OK] socket initialization completed"

echo "# File for request logging" >> ./code/config/heimdall_config.conf
echo "log_file_req "$log_path"heimdall_req.log" >> ./code/config/heimdall_config.conf
echo "" >> ./code/config/heimdall_config.conf
echo "# File for response logging" >> ./code/config/heimdall_config.conf
echo "log_file_req "$log_path"heimdall_resp.log" >> ./code/config/heimdall_config.conf
echo "[OK] log files initialization completed"

echo "[OK] configuration completed!"

echo ""
echo ""
echo "to run webswitch: >> cd ./build"
echo "                  >> ./WebSwitch"
echo "to change config: >> cd ./code/config"
echo "                  >> <editor> heimdall_config.conf"
echo "                  >> <editor> server_config.conf"



