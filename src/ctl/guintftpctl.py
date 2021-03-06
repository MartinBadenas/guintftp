#!/usr/bin/python
# -*- coding: utf-8 -*-

from __future__ import with_statement

__license__ = "GPL"
__copyright__ = """
Copyright (C) 2010 Dani Hernández Juárez, dhernandez0@gmail.com
Copyright (C) 2010 Martín Badenas Beltrán, martin.badenas@gmail.com

This file is part of Guintftp.

Guintftp is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Guintftp is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Guintftp.  If not, see <http://www.gnu.org/licenses/>.
"""

import os, sys, signal, subprocess, time

PID_FILE = '/var/run/guintftp.pid' # pid file path
PROGRAM = 'guintftp' # daemon executable name/path
# Used in stop()
WAIT_TIME = 3
MAX_ATTEMPS = 3

def pid_running(pid):
    '''Check whether a pid is running, returns a boolean'''
    try:
        # Send 0 signal to check the process exists
        os.kill(pid, 0)
    except OSError:
        return False
    else:
        return True

def get_pid():
    '''Returns daemon's pid, read from pid file'''
    try:
        with open(PID_FILE, 'r') as f:
            return int(f.read())
    except IOError, OSError:
        return None

def start(pid):
    '''Starts the daemon, this ensures that it's not running before and cleans pid file if needed'''
    worked = False
    if pid != None and pid_running(pid):
        sys.stderr.write("It's already running, pid: %s\n" % pid)
    else:
        # If pid isn't running and pid file exists remove it
        if pid != None:
            os.unlink(PID_FILE)
        print 'Starting...',
        sys.stdout.flush()
        try:
            retcode = subprocess.call(PROGRAM)
            time.sleep(WAIT_TIME)
            if retcode == 0 and pid_running(get_pid()):
                print 'OK'
                worked = True
            else:
                try:
                    os.unlink(PID_FILE)
                except:
                    pass
                print 'ERROR (return code: %s)' % str(retcode)
        except Exception as e:
            print 'ERROR (%s)' % str(e)
    return worked


def stop(pid, signal_kill=signal.SIGTERM):
    '''Stops the daemon, sends signal and waits until daemon has died and removes pid file'''
    worked = False
    if pid == None:
        sys.stderr.write("Daemon isn't running\n")
    else:
        print 'Stopping...',
        sys.stdout.flush()
        try:
            os.kill(pid, signal_kill)
            attemps = 0
            # TODO: Couldn't we wait(pid)?
            while pid_running(pid) or attemps < MAX_ATTEMPS:
                attemps += 1
                time.sleep(WAIT_TIME)
            # Check it's killed and remove pid file
            if not pid_running(pid):
                os.unlink(PID_FILE)
                print 'OK'
                worked = True
            else:
                print 'ERROR (Unknown, pid (%s) is still running)' % str(pid)
        except Exception as e:
            print 'ERROR (%s)' % str(e)
    return worked
        

def force_stop(pid):
    '''Stops the daemon, sends SIGKILL and waits until daemon has died and removes pid file'''
    return stop(pid, signal.SIGKILL)

def restart(pid):
    '''Restarts the daemon, stop() and start()'''
    stop(pid)
    return start(None)

def status(pid):
    '''Prints daemon status'''
    if pid_running(pid):
        print 'Running, pid: %s' % str(pid)
    else:
        print 'Not running'
    return True


if __name__ == '__main__':
    # We need to be root
    if os.geteuid() != 0:
        sys.stderr.write('You must be root!\n')
    else:
        # Make a dictionary {function_name, function_pointer}
        functions = [start, stop, force_stop, restart, status]
        actions = {}
        for i in functions:
            actions[i.func_name] = i
        # Check whether there're enough arguments and that they're correct  
        if len(sys.argv) == 1 or not sys.argv[1] in actions:
            sys.stderr.write('Usage %s {%s}\n' % (sys.argv[0], '|'.join(actions.keys())))
        else:
            # Execute action
            func = actions[sys.argv[1]]
            pid = get_pid()
            if func(pid):
                sys.exit()
            else:
                sys.exit(1)