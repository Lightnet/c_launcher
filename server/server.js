import http from 'http';
import path from 'path';
import fs from 'fs';
import mime from 'mime';
import util from 'util';
import { Transform } from 'stream';

function LimitStream(limit) {
  Transform.call(this);
  this.limit = limit ? (limit * 1024) / 1000 : null; // Convert KiB/s to bytes/ms
  this.sentBytes = 0;
  this.startTime = Date.now();
}

util.inherits(LimitStream, Transform);

LimitStream.prototype._transform = function (chunk, encoding, callback) {
  this.sentBytes += chunk.length;

  if (this.limit) {
    const elapsedTime = Date.now() - this.startTime;
    const expectedTime = this.sentBytes / this.limit;
    const lag = expectedTime - elapsedTime;

    if (lag > 0) {
      setTimeout(() => {
        this.push(chunk);
        callback();
      }, lag);
      return;
    }
  }

  this.push(chunk);
  callback();
};

LimitStream.prototype._flush = function (callback) {
  callback();
};


const hostname = '127.0.0.1';
const port = 3000;

const server = http.createServer((req, res) => {
  const file = 'raylib-5.5_win32_msvc16.zip';
  const filePath = path.resolve(file);
  const filename = path.basename(file);
  const mimetype = mime.getType(filePath) || 'application/octet-stream';

  try {
    const stats = fs.statSync(filePath);
    if (!stats.isFile()) {
      res.statusCode = 404;
      res.setHeader('Content-Type', 'text/plain');
      res.end('Error: Not a file\n');
      return;
    }

    res.setHeader('Content-Disposition', `attachment; filename="${filename}"`);
    res.setHeader('Content-Type', mimetype);
    res.setHeader('Content-Length', stats.size);

    const filestream = fs.createReadStream(filePath);
    //const limitStream = new LimitStream(120); // Limit to 120 KiB/s
    const limitStream = new LimitStream(50); // Limit to 50 KiB/s

    filestream.on('error', (err) => {
      console.error('Stream error:', err);
      if (!res.headersSent) {
        res.statusCode = 500;
        res.setHeader('Content-Type', 'text/plain');
        res.end('Error reading file\n');
      }
    });

    filestream.pipe(limitStream).pipe(res);
  } catch (err) {
    console.error('File error:', err);
    res.statusCode = 404;
    res.setHeader('Content-Type', 'text/plain');
    res.end('File not found\n');
  }
});

server.listen(port, hostname, () => {
  console.log(`Server running at http://${hostname}:${port}/`);
});