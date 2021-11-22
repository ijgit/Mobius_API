const express = require('express');
const multer = require('multer');
const mongodb = require('mongodb');
const MongoClient = require('mongodb').MongoClient;
const {ObjectID} = require('mongodb');
const request = require('request')
const gridfs = require('gridfs-stream');
const {Readable} = require('stream');
const fs = require("fs");
const assert = require('assert');
const conf = require('./../conf')

const addr = `${conf.ip}:${conf.port}`
const mobius_url = `${conf.mp_url}/${conf.ae}/model`
console.log(mobius_url)

var router = express.Router();

let db;
MongoClient.connect(`mongodb://localhost:27017`, (err, client)=>{
  if(err){ return console.dir(err);}
  db = client.db('test');
});


/* GET */
router.get('/', function(req, res, next) {
  res.send('respond with a resource')
});


router.post('/', function(req, res, next) {
  res.send(req.body)
  console.log(req.body)

  if(!res.req.body)
    return res.status(400).json({message: 'req body cannot be empty'})
});


/* GET download */
router.get('/download', function(req, res, next) {
  var cnt, id, filename;
  try{
    cnt = req.query.cnt
    id = new ObjectID(req.query['id'])
    filename = req.query.filename
  }catch (err) {
    return res.status(400).json({ message: "Invalid id in URL parameter. Must be a single String of 12 bytes or a string of 24 hex characters" });
  }
  console.log(cnt, filename)

  var bucket = new mongodb.GridFSBucket(db, {
    bucketName: cnt
  });
  bucket.openDownloadStream(id).
  pipe(fs.createWriteStream(filename)).

  on('error', function(error){
    assert.ifError(error)
  }).
  on('finish', function(){
    fs.readFile(filename, function(err, content){
      if(err){  return res.status(400).json({ message: "No such file" });}
      else{
        res.setHeader('Content-disposition', 'attachment; filename=' + filename)
        res.set('Content-Type', 'Application/octet-stream')
        res.download(filename, filename)
        res.end(content)

        fs.unlink(filename, function(err){
          if(err) throw err
        })
      } 
    })    
  })
});


/* GET */
router.get('/', (req, res) => {
  let cnt, id
  try{
    cnt = req.params.cnt
    id = new ObjectID(req.query['id'])
  }catch(err){
    res.status(400).json({
      message: 'Invalid parameters and query in URL'
    })
  }
  res.set('content-type', 'Application/octet-stream');
  res.set('accept-ranges', 'bytes')

  let bucket = new mongodb.GridFSBucket(db, {
    bucketName: cnt
  });

  let downloadStream = bucket.openDownloadStream(id);
  downloadStream.on('data', (chunk) => {
    res.write(chunk);
  });
  downloadStream.on('error', () => {
    res.sendStatus(404)
  });
  downloadStream.on('end', () => {
    res.end()
  });
});




/* POST upload */
router.post('/upload', function(req, res, next) {
  // const ae = req.body.ae
  const cnt = req.body.cnt
  const filename = req.body.filename
  const file = req.files[0]

  const storage = multer.memoryStorage()
  const upload = multer({storage: storage})

  upload.single('file')(req, res, (err) => {
    if(err){  return res.status(400).json({ message: "Upload Request Validation Failed" });}
    // else if(!ae){ return res.status(400).json({ message: "No ae name in request body" });}
    else if(!cnt){ return res.status(400).json({ message: "No cnt name in request body" });}

    // convert buffer to readable stream
    const readableStream = new Readable();
    readableStream.push(file.buffer)
    readableStream.push(null)

    let bucket = new mongodb.GridFSBucket(db, {
      bucketName: cnt
    });

    let uploadStream = bucket.openUploadStream(cnt);
    readableStream.pipe(uploadStream)

    uploadStream.on('error', () => {
      return res.status(500).json({ message: "Error uploading file" });
    });

    uploadStream.on('finish', () => {

      const download_link = `${addr}/models/download?cnt=${cnt}&id=${uploadStream.id}&filename=${filename}`
      var up_data = {
        'id': uploadStream.id,
        'url': download_link,
        'filename': filename
      }
      create_cin_request(cnt, up_data)
      return res.status(201).json({ message: `File uploaded successfully, download link: ${download_link}`});
    })
  });
});


function create_cin_request(cnt, input_data){
  var cin = {
    'm2m:cin':{
      'con': input_data
    }
  }
  const options = {
    'method': 'POST',
    'url': `http://${mobius_url}`,
    'headers': {
      'Accept': 'application/json',
      'X-M2M-RI': '12345',
      'X-M2M-Origin': '{{aei}}',
      'Content-Type': 'application/vnd.onem2m-res+json; ty=4'
    },
    body: JSON.stringify(cin)
  }
  request(options, function (error, _response, body) {
    if (error) throw new Error(error);
    console.log(body)
  });
}

module.exports = router;
