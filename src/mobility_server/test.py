#!/usr/bin/python

from subprocess import Popen, PIPE
import sys

#def main():
request_body = '\1\0\0\0'

# launch backend service passing the payload size as first param
process_res = Popen(['./mobility_server', 'exchange', '4', 'ACCL-TEST-APPLICATION'], stdout=PIPE, stdin=PIPE)

# send the buffer to the backend service
out, err = process_res.communicate(request_body)

if process_res.returncode == 0:
    print out
    sys.exit(0)
else:
    print "ERROR"
    sys.exit(1)