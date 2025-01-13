#!/bin/bash

for a in controls contents scontrols scontents; do
  amixer "$@" $a > amixer-${a}.txt
done
