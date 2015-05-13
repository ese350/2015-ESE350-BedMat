import json
from threading import Thread
import serial
from BaseHTTPServer import HTTPServer, BaseHTTPRequestHandler
from SocketServer import ThreadingMixIn
import threading

matdata = None

def get_mat_data():
	global matdata
	print "Connected to Mat"
	s = serial.Serial(2)
	while True:
		matdata = s.readline()

	s.close()


class Handler(BaseHTTPRequestHandler):

    def do_GET(self):
		global matdata
		self.send_response(200)
		self.send_header('Access-Control-Allow-Origin', '*')
		self.end_headers()
		
		stuff = matdata.split(",")
		transformed = [stuff[i:i+16] for i in range(0, 256, 16)]
		transformed.append(stuff[-1])
		
		self.wfile.write(json.dumps(transformed))
		self.wfile.write('\n')
		return

class ThreadedHTTPServer(ThreadingMixIn, HTTPServer):
    """Handle requests in a separate thread."""

if __name__ == '__main__':
	server = ThreadedHTTPServer(('localhost', 8080), Handler)
	print 'Starting server, use <Ctrl-C> to stop'
	thread = Thread(target = get_mat_data)
	thread.start()
	server.serve_forever()
	thread.join()