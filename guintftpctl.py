#!/usr/bin/python
# -*- coding: utf-8 -*-
from __future__ import with_statement
import os, sys, signal, subprocess, time

PID_FILE = '/var/run/guintftp.pid'
PROGRAM = 'guintftp'

def pid_running(pid):
    try:
        os.kill(pid, 0)
    except OSError:
        return False
    else:
        return True

def get_pid():
    try:
        with open(PID_FILE, 'r') as f:
            return int(f.read())
    except IOError, OSError:
        return None

def start(pid):
    if pid != None and pid_running(pid):
        sys.stderr.write("It's already running, pid: %s\n" % pid)
    else:
        if pid != None:
            os.unlink(PID_FILE)
        print 'Starting...',
        sys.stdout.flush()
        try:
            retcode = subprocess.call(PROGRAM)
            if retcode == 0:
                print 'OK'
            else:
                os.unlink(PID_FILE)
                print 'ERROR'
        except Exception as e:
            print 'ERROR (%s)' % str(e)


def stop(pid, signal_kill=signal.SIGTERM):
    if pid == None:
        sys.stderr.write("Daemon isn't running\n")
    else:
        print 'Stopping...',
        sys.stdout.flush()
        try:
            os.kill(pid, signal_kill)
            attemps = 0
            while pid_running(pid) or attemps < 3:
                attemps += 1
                time.sleep(3)
            if not pid_running(pid):
                os.unlink(PID_FILE)
                print 'OK'
            else:
                print 'ERROR (Unknown, pid (%s) is still running)' % str(pid)
        except Exception as e:
            print 'ERROR (%s)' % str(e)
        

def force_stop(pid):
    stop(pid, signal.SIGKILL)

def restart(pid):
    stop(pid)
    start(pid)

def status(pid):
    if pid_running(pid):
        print 'Running, pid: %s' % str(pid)
        process_group = os.getpgid(pid)
        
    else:
        print 'Not running'


if __name__ == '__main__':
    if os.getuid() != 0:
        sys.exit('You must be root!')
    else:
        actions = {'start':start, 'stop':stop, 'force_stop':force_stop, 'restart':restart, 'status':status}
        if len(sys.argv) == 1 or not sys.argv[1] in actions:
            sys.exit('Usage %s {%s}' % (sys.argv[0], '|'.join(actions.keys())))
        else:
            func = actions[sys.argv[1]]
            pid = get_pid()
            func(pid)
            sys.exit()