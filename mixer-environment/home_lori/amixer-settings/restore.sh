#!/bin/bash

set -e
cd /home/lori/amixer-settings

amixer -c 2 --stdin < c2/restore.amixer
amixer --stdin < default/restore.amixer 
