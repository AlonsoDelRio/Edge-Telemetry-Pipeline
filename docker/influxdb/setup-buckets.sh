#!/bin/bash
set -e

influx bucket create -n ${INFLUXDB_BUCKET_SIM} -o ${INFLUXDB_ORG} -r 24h
influx bucket create -n ${INFLUXDB_BUCKET_HW} -o ${INFLUXDB_ORG} -r 0 