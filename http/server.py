from http.server import HTTPServer, SimpleHTTPRequestHandler
import ssl
import os
import argparse

if __name__ == '__main__':

  top_parser = argparse.ArgumentParser(description='Simple HTTPS server')
  top_parser.add_argument('--port', action="store", dest="port", type=int, help="The port to listen on", default="443")
  args = top_parser.parse_args()

  os.system("openssl req -nodes -x509 -newkey rsa:4096 -keyout key.pem -out cert.pem -days 365 -subj '/CN=mylocalhost'")
  httpd = HTTPServer(('0.0.0.0', args.port), SimpleHTTPRequestHandler)
  sslctx = ssl.SSLContext()
  sslctx.check_hostname = False 
  sslctx.load_cert_chain(certfile='cert.pem', keyfile="key.pem")
  httpd.socket = sslctx.wrap_socket(httpd.socket, server_side=True)
  print(f"Server running on https://0.0.0.0:{args.port}")
  httpd.serve_forever()

