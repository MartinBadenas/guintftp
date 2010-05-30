#!/bin/sh
pod2man -u -c="TFTP Daemon" -n="guintftp" -r="guintftp 0.1" -s=1 -v guintftp.pod > guintftp.8
pod2man -u -c="guintftp control interface" -n="guintftpctl" -r="guintftpctl 0.1" -s=1 -v guintftpctl.pod > guintftpctl.8