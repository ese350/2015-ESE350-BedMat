#!/usr/bin/python
from BaseHTTPServer import BaseHTTPRequestHandler,HTTPServer
import json

PORT_NUMBER = 80

plotdata = [[],[],[],[]]

class myHandler(BaseHTTPRequestHandler):

        #Handler for the GET requests
        def do_GET(self):
                global plotdata
                if "pushdata" in self.path:
                        self.send_response(200)
                        self.send_header('Content-type','text/html')
                        self.end_headers()
                        content_len = int(self.headers.getheader('content-length', 0))
                        post_body = self.rfile.read(content_len).split(",")
                        quadrant = post_body[0]
                        print post_body
                        post_body = post_body[1:]
                        step2 = [post_body[i:i+8] for i in range(0, 64, 8)]
                        plotdata[int(quadrant)] = step2
                else:
                        self.send_response(200)
                        self.send_header('Content-type','text/html');
                        self.send_header('Access-Control-Allow-Origin', '*')
                        self.end_headers()
                        self.wfile.write(json.dumps(plotdata))
                        self.wfile.close()

try:
        #Create a web server and define the handler to manage the
        #incoming request
        server = HTTPServer(('', PORT_NUMBER), myHandler)
        print 'Started httpserver on port ' , PORT_NUMBER

        #Wait forever for incoming htto requests
        server.serve_forever()

except KeyboardInterrupt:
        print '^C received, shutting down the web server'
        server.socket.close()
