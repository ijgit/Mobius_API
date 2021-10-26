var express = require('express');
const multer = require('multer');
var gridfs = require('gridfs-stream');
var fs = require("fs");
var assert = require('assert');

var router = express.Router();
// const { Readable } = require('stream');

/* GET  listing. */
router.get('/', function(req, res, next) {
  res.send('respond with a resource');
});


/* Download model */
router.get('/download/:cnt', (req, res) => {
  res.send('get download request');

  cnt_name = req.params.cnt;
  console.log(typeof(cnt_name))
  console.log(cnt_name)
  
  //  res.send(`req for ${cnt_name}`)
}); 



router.post('/', function(req, res, next){
  
  console.log(req.body)
  console.log(req.body.cnt)
  console.log(req.body.ae)

  res.send('post request')
});

router.delete('/', function(req, res, next){
    res.send('delete request')
});




module.exports = router;
